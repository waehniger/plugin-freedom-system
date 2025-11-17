#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <juce_audio_formats/juce_audio_formats.h>

// ============================================================================
// SampleLoaderThread - Background thread for loading audio files
// ============================================================================
class MuSamAudioProcessor::SampleLoaderThread : public juce::Thread
{
public:
    SampleLoaderThread(MuSamAudioProcessor& processor, const juce::File& file)
        : Thread("SampleLoaderThread"), processorRef(processor), fileToLoad(file)
    {
    }

    void run() override
    {
        juce::Logger::writeToLog("MuSam: SampleLoaderThread started for: " + fileToLoad.getFullPathName());
        
        auto* reader = processorRef.formatManager.createReaderFor(fileToLoad);
        
        if (reader != nullptr)
        {
            auto numChannels = static_cast<int>(reader->numChannels);
            auto numSamples = static_cast<int>(reader->lengthInSamples);
            auto sampleRate = reader->sampleRate;

            juce::Logger::writeToLog("MuSam: File loaded successfully - " +
                                     juce::String(numChannels) + " channels, " +
                                     juce::String(numSamples) + " samples, " +
                                     juce::String(sampleRate) + " Hz");

            auto newBuffer = std::make_unique<juce::AudioBuffer<float>>(numChannels, numSamples);
            reader->read(newBuffer.get(), 0, numSamples, 0, true, true);

            // Atomic swap: Load new buffer, mark as ready
            processorRef.fileSampleRate = sampleRate;
            processorRef.fileNumChannels = numChannels;
            processorRef.activeSampleBuffer.store(newBuffer.get());
            processorRef.sampleBuffer = std::move(newBuffer);
            processorRef.sampleReady.store(true);

            // Update region boundaries on message thread after sample is loaded
            juce::MessageManager::callAsync([this]() {
                processorRef.updateRegionBoundaries();
            });

            juce::Logger::writeToLog("MuSam: Sample buffer ready for playback");
            delete reader;
        }
        else
        {
            juce::Logger::writeToLog("MuSam: ERROR - Could not create reader for file: " + 
                                     fileToLoad.getFullPathName() + 
                                     ". Format may not be supported or file may be corrupted.");
        }
    }

private:
    MuSamAudioProcessor& processorRef;
    juce::File fileToLoad;
};

juce::AudioProcessorValueTreeState::ParameterLayout MuSamAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // ============================================================================
    // GLOBAL PARAMETERS (2)
    // ============================================================================

    // speed - Float parameter (0.5 to 2.0, default 1.0)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "speed", 1 },
        "Speed",
        juce::NormalisableRange<float>(0.5f, 2.0f, 0.01f, 1.0f),
        1.0f,
        "x"
    ));

    // volume - Float parameter (-60.0 to 0.0 dB, default -6.0)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "volume", 1 },
        "Volume",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f, 1.0f),
        -6.0f,
        "dB"
    ));

    // ============================================================================
    // REGION PARAMETERS (5 regions × 8 parameters = 40 parameters)
    // ============================================================================

    // Helper lambda to add region parameters
    auto addRegionParameters = [&layout](int regionNum, float startDefault, float endDefault) {
        juce::String regionPrefix = "region_" + juce::String(regionNum) + "_";

        // Start position (0.0 to 100.0%)
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { regionPrefix + "start", 1 },
            "Region " + juce::String(regionNum) + " Start",
            juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f, 1.0f),
            startDefault,
            "%"
        ));

        // End position (0.0 to 100.0%)
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { regionPrefix + "end", 1 },
            "Region " + juce::String(regionNum) + " End",
            juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f, 1.0f),
            endDefault,
            "%"
        ));

        // Pitch (-24.0 to +24.0 semitones)
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { regionPrefix + "pitch", 1 },
            "Region " + juce::String(regionNum) + " Pitch",
            juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f, 1.0f),
            0.0f,
            "st"
        ));

        // Filter cutoff (20.0 to 20000.0 Hz, logarithmic skew 0.3)
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { regionPrefix + "filter_cutoff", 1 },
            "Region " + juce::String(regionNum) + " Filter Cutoff",
            juce::NormalisableRange<float>(20.0f, 20000.0f, 0.1f, 0.3f),
            20000.0f,
            "Hz"
        ));

        // Filter resonance (0.0 to 100.0%)
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { regionPrefix + "filter_resonance", 1 },
            "Region " + juce::String(regionNum) + " Filter Resonance",
            juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f, 1.0f),
            0.0f,
            "%"
        ));

        // Attack (0.0 to 1000.0 ms)
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { regionPrefix + "attack", 1 },
            "Region " + juce::String(regionNum) + " Attack",
            juce::NormalisableRange<float>(0.0f, 1000.0f, 0.1f, 1.0f),
            10.0f,
            "ms"
        ));

        // Decay (0.0 to 5000.0 ms)
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { regionPrefix + "decay", 1 },
            "Region " + juce::String(regionNum) + " Decay",
            juce::NormalisableRange<float>(0.0f, 5000.0f, 0.1f, 1.0f),
            500.0f,
            "ms"
        ));

        // Pan (-100.0 to +100.0%)
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { regionPrefix + "pan", 1 },
            "Region " + juce::String(regionNum) + " Pan",
            juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f, 1.0f),
            0.0f,
            "%"
        ));
    };

    // Add parameters for all 5 regions
    addRegionParameters(1, 0.0f, 20.0f);   // Region 1: 0-20%
    addRegionParameters(2, 20.0f, 40.0f);  // Region 2: 20-40%
    addRegionParameters(3, 40.0f, 60.0f);  // Region 3: 40-60%
    addRegionParameters(4, 60.0f, 80.0f);  // Region 4: 60-80%
    addRegionParameters(5, 80.0f, 100.0f); // Region 5: 80-100%

    // ============================================================================
    // SEQUENCER PARAMETERS (11)
    // ============================================================================

    // Step region assignments (8 steps)
    juce::StringArray stepChoices = { "None", "Region 1", "Region 2", "Region 3", "Region 4", "Region 5" };
    
    for (int step = 1; step <= 8; ++step) {
        int defaultIndex = (step <= 5) ? step : 0; // Steps 1-5 default to their region, 6-8 default to None
        
        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID { "step_" + juce::String(step) + "_region", 1 },
            "Step " + juce::String(step) + " Region",
            stepChoices,
            defaultIndex
        ));
    }

    // Playback mode (Sequential/Random/Custom)
    juce::StringArray playbackModeChoices = { "Sequential", "Random", "Custom" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "playback_mode", 1 },
        "Playback Mode",
        playbackModeChoices,
        0 // Default: Sequential
    ));

    // Loop mode (Bool)
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "loop_mode", 1 },
        "Loop Mode",
        true // Default: On
    ));

    // Crossfade time (0.0 to 500.0 ms)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "crossfade_time", 1 },
        "Crossfade Time",
        juce::NormalisableRange<float>(0.0f, 500.0f, 0.1f, 1.0f),
        50.0f,
        "ms"
    ));

    return layout;
}

MuSamAudioProcessor::MuSamAudioProcessor()
    : AudioProcessor(BusesProperties()
                        .withInput("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , parameters(*this, nullptr, "Parameters", createParameterLayout())
{
    // Register audio formats (WAV, AIFF, MP3 via system codecs)
    formatManager.registerBasicFormats();
}

MuSamAudioProcessor::~MuSamAudioProcessor()
{
    // Stop loader thread if running
    if (loaderThread != nullptr)
    {
        loaderThread->stopThread(5000);
        loaderThread.reset();
    }
}

void MuSamAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    updateRegionBoundaries();
    
    // Phase 4.2: Prepare DSP processors
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 2;  // Stereo processing
    
    panner.prepare(spec);
    
    // Prepare filters for each region
    for (int i = 0; i < 5; ++i)
    {
        regionStates[i].filter.prepare(spec);
        updateFilterCoefficients(i);
        
        // Phase 4.3: Initialize granular synthesis buffers
        const int grainSize = 1024;  // ~21ms @ 48kHz
        const int grainBufferSize = grainSize * 2;  // Double buffer for overlap
        regionStates[i].grainBuffer.setSize(static_cast<int>(spec.numChannels), grainBufferSize);
        regionStates[i].grainBuffer.clear();
        regionStates[i].grainPosition = 0.0f;
        regionStates[i].grainWritePosition = 0.0f;
        regionStates[i].pitchRatio = 1.0f;
    }
    
    // Phase 4.4: Initialize sequencer
    sequencerState.currentStep = 0;
    sequencerState.stepTime = 0.0f;
    sequencerState.stepDuration = 0.0f;
    sequencerState.isPlaying = false;
    sequencerState.previousRegion = -1;
    sequencerState.crossfadePosition = 0.0f;
    
    // Reset playback states
    for (int i = 0; i < 5; ++i)
    {
        regionStates[i].samplePosition = 0.0f;
        regionStates[i].isActive = false;
        regionStates[i].envelopeAmplitude = 0.0f;
        regionStates[i].envelopeTime = 0.0f;
        regionStates[i].envelopePhase = RegionPlaybackState::Idle;
    }
}

void MuSamAudioProcessor::releaseResources()
{
    // Stop all playback
    isPlaying = false;
    for (int i = 0; i < 5; ++i)
    {
        regionStates[i].isActive = false;
    }
}

void MuSamAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    // Clear output buffer
    buffer.clear();
    
    // Check if sample is ready
    if (!sampleReady.load() || activeSampleBuffer.load() == nullptr)
        return;
    
    auto* activeBuffer = activeSampleBuffer.load();
    if (activeBuffer == nullptr || activeBuffer->getNumSamples() == 0)
        return;
    
    // Note: Region boundaries are updated on parameter changes and state load,
    // not in processBlock() to avoid race conditions with setStateInformation()
    
    // Phase 4.2: Update filter coefficients (may change during playback)
    for (int i = 0; i < 5; ++i)
    {
        updateFilterCoefficients(i);
    }
    
    // Process MIDI messages
    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();
        
        if (message.isNoteOn())
        {
            // Phase 4.4: Start sequencer from step 1
            isPlaying = true;
            sequencerState.isPlaying = true;
            sequencerState.currentStep = 0;
            sequencerState.stepTime = 0.0f;
            sequencerState.previousRegion = -1;
            sequencerState.crossfadePosition = 0.0f;
            
            // Calculate step duration (host tempo sync or internal timing)
            auto* playhead = getPlayHead();
            if (playhead != nullptr)
            {
                juce::AudioPlayHead::CurrentPositionInfo posInfo;
                if (playhead->getCurrentPosition(posInfo))
                {
                    if (posInfo.bpm > 0.0)
                    {
                        // Host tempo sync: 1/16th note per step
                        const float beatsPerStep = 0.25f;  // 1/16th note
                        sequencerState.stepDuration = (beatsPerStep * 60.0f / posInfo.bpm) * static_cast<float>(currentSampleRate);
                    }
                }
            }
            
            // Fallback to internal timing (120 BPM, 1/16th note)
            if (sequencerState.stepDuration <= 0.0f)
            {
                sequencerState.stepDuration = (0.25f * 60.0f / 120.0f) * static_cast<float>(currentSampleRate);
            }
            
            // Activate first step's region
            advanceSequencerStep();
        }
        else if (message.isNoteOff())
        {
            // Phase 4.4: Stop sequencer
            isPlaying = false;
            sequencerState.isPlaying = false;
            
            // Let envelopes decay naturally
            for (int i = 0; i < 5; ++i)
            {
                // Don't force stop - let envelope complete decay
            }
        }
    }
    
    // Phase 4.4: Update sequencer
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    
    if (sequencerState.isPlaying)
    {
        updateSequencer(numSamples);
    }
    
    // Create temporary buffers for region processing
    juce::AudioBuffer<float> tempBuffer(numChannels, numSamples);
    juce::AudioBuffer<float> outgoingBuffer(numChannels, numSamples);  // For crossfade
    
    for (int i = 0; i < 5; ++i)
    {
        if (regionStates[i].isActive)
        {
            // Clear temp buffer
            tempBuffer.clear();
            
            // Phase 4.1: Sample playback
            processRegionPlayback(i, tempBuffer, 0, numSamples);
            
            // Phase 4.3: Apply pitch shifting
            applyPitchShift(i, tempBuffer, 0, numSamples);
            
            // Phase 4.2: Apply envelope
            updateEnvelope(i, numSamples);
            tempBuffer.applyGain(regionStates[i].envelopeAmplitude);
            
            // Phase 4.2: Apply filter
            juce::dsp::AudioBlock<float> block(tempBuffer);
            juce::dsp::ProcessContextReplacing<float> context(block);
            regionStates[i].filter.process(context);
            
            // Phase 4.2: Apply pan
            applyPan(i, tempBuffer, 0, numSamples);
            
            // Phase 4.4: Apply crossfade if transitioning
            if (sequencerState.previousRegion >= 0 && sequencerState.previousRegion != i && sequencerState.crossfadePosition < 1.0f)
            {
                applyCrossfade(tempBuffer, 0, numSamples, sequencerState.previousRegion, i);
            }
            
            // Mix into output buffer
            for (int channel = 0; channel < numChannels; ++channel)
            {
                buffer.addFrom(channel, 0, tempBuffer, channel, 0, numSamples);
            }
        }
    }
    
    // Apply master volume
    auto* volumeParam = parameters.getRawParameterValue("volume");
    if (volumeParam != nullptr)
    {
        const float volumeDb = volumeParam->load();
        const float volumeLinear = juce::Decibels::decibelsToGain(volumeDb);
        
        for (int channel = 0; channel < numChannels; ++channel)
        {
            buffer.applyGain(channel, 0, numSamples, volumeLinear);
        }
    }
}

juce::AudioProcessorEditor* MuSamAudioProcessor::createEditor()
{
    return new MuSamAudioProcessorEditor(*this);
}

void MuSamAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MuSamAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    
    if (xmlState != nullptr && xmlState->hasTagName(parameters.state.getType()))
    {
        // CRITICAL: Suspend audio processing to prevent race conditions
        // when replaceState() modifies APVTS while processBlock() reads parameters
        suspendProcessing(true);
        
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
        
        // Resume audio processing after state is loaded
        suspendProcessing(false);
        
        // Update region boundaries after state is loaded (if sample is ready)
        if (sampleReady.load() && activeSampleBuffer.load() != nullptr)
        {
            updateRegionBoundaries();
        }
    }
}

// ============================================================================
// Phase 4.1: Core Sample Playback Implementation
// ============================================================================

void MuSamAudioProcessor::loadSampleFromFile(const juce::File& file)
{
    // Stop any existing loader thread
    if (loaderThread != nullptr)
    {
        loaderThread->stopThread(5000);
        loaderThread.reset();
    }
    
    // Mark sample as not ready
    sampleReady.store(false);
    activeSampleBuffer.store(nullptr);
    
    // Start background loading thread
    loaderThread = std::make_unique<SampleLoaderThread>(*this, file);
    loaderThread->startThread();
}

void MuSamAudioProcessor::updateRegionBoundaries()
{
    auto* activeBuffer = activeSampleBuffer.load();
    if (activeBuffer == nullptr || activeBuffer->getNumSamples() == 0)
        return;
    
    const int totalSamples = activeBuffer->getNumSamples();
    
    for (int i = 0; i < 5; ++i)
    {
        juce::String prefix = "region_" + juce::String(i + 1) + "_";
        
        auto* startParam = parameters.getRawParameterValue(prefix + "start");
        auto* endParam = parameters.getRawParameterValue(prefix + "end");
        
        if (startParam != nullptr && endParam != nullptr)
        {
            const float startPercent = startParam->load();
            const float endPercent = endParam->load();
            
            regionStates[i].startSample = static_cast<int>((startPercent / 100.0f) * totalSamples);
            regionStates[i].endSample = static_cast<int>((endPercent / 100.0f) * totalSamples);
            
            // Clamp to valid range
            regionStates[i].startSample = juce::jlimit(0, totalSamples - 1, regionStates[i].startSample);
            regionStates[i].endSample = juce::jlimit(regionStates[i].startSample + 1, totalSamples, regionStates[i].endSample);
        }
    }
}

void MuSamAudioProcessor::processRegionPlayback(int regionIndex, juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    auto* activeBuffer = activeSampleBuffer.load();
    if (activeBuffer == nullptr)
        return;
    
    auto& region = regionStates[regionIndex];
    
    // Get speed parameter
    auto* speedParam = parameters.getRawParameterValue("speed");
    const float speed = (speedParam != nullptr) ? speedParam->load() : 1.0f;
    
    // Calculate playback increment (accounts for sample rate conversion)
    const float playbackIncrement = speed * (static_cast<float>(currentSampleRate) / static_cast<float>(fileSampleRate));
    
    const int numChannels = juce::jmin(buffer.getNumChannels(), activeBuffer->getNumChannels());
    
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Check if we've reached the end of the region
        if (region.samplePosition >= static_cast<float>(region.endSample))
        {
            region.isActive = false;
            break;
        }
        
        // Read sample with linear interpolation
        for (int channel = 0; channel < numChannels; ++channel)
        {
            const float sampleValue = linearInterpolateSample(*activeBuffer, channel, region.samplePosition);
            buffer.addSample(channel, startSample + sample, sampleValue);
        }
        
        // Advance playback position
        region.samplePosition += playbackIncrement;
    }
}

float MuSamAudioProcessor::linearInterpolateSample(const juce::AudioBuffer<float>& buffer, int channel, float position)
{
    const int numSamples = buffer.getNumSamples();
    const int posInt = static_cast<int>(position);
    const float posFrac = position - static_cast<float>(posInt);
    
    if (posInt >= numSamples - 1)
        return buffer.getSample(channel, numSamples - 1);
    
    const float sample1 = buffer.getSample(channel, posInt);
    const float sample2 = buffer.getSample(channel, posInt + 1);
    
    return sample1 + (sample2 - sample1) * posFrac;
}

// ============================================================================
// Phase 4.2: Per-Region Processing Implementation
// ============================================================================

void MuSamAudioProcessor::updateFilterCoefficients(int regionIndex)
{
    juce::String prefix = "region_" + juce::String(regionIndex + 1) + "_";
    
    auto* cutoffParam = parameters.getRawParameterValue(prefix + "filter_cutoff");
    auto* resonanceParam = parameters.getRawParameterValue(prefix + "filter_resonance");
    
    if (cutoffParam != nullptr && resonanceParam != nullptr)
    {
        const float cutoffHz = cutoffParam->load();
        const float resonancePercent = resonanceParam->load();
        
        // Map resonance (0-100%) to Q factor (0.5 to 10.0)
        const float Q = 0.5f + (resonancePercent / 100.0f) * 9.5f;
        
        // Create 2-pole lowpass filter coefficients
        auto coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, cutoffHz, Q);
        regionStates[regionIndex].filter.coefficients = coeffs;
    }
}

void MuSamAudioProcessor::updateEnvelope(int regionIndex, int numSamples)
{
    auto& region = regionStates[regionIndex];
    
    juce::String prefix = "region_" + juce::String(regionIndex + 1) + "_";
    
    auto* attackParam = parameters.getRawParameterValue(prefix + "attack");
    auto* decayParam = parameters.getRawParameterValue(prefix + "decay");
    
    if (attackParam == nullptr || decayParam == nullptr)
        return;
    
    const float attackMs = attackParam->load();
    const float decayMs = decayParam->load();
    
    // Convert ms to samples
    const float attackSamples = (attackMs / 1000.0f) * static_cast<float>(currentSampleRate);
    const float decaySamples = (decayMs / 1000.0f) * static_cast<float>(currentSampleRate);
    
    for (int sample = 0; sample < numSamples; ++sample)
    {
        if (region.envelopePhase == RegionPlaybackState::Attack)
        {
            if (attackSamples > 0.0f)
            {
                region.envelopeAmplitude = region.envelopeTime / attackSamples;
                
                if (region.envelopeTime >= attackSamples)
                {
                    region.envelopeAmplitude = 1.0f;
                    region.envelopePhase = RegionPlaybackState::Decay;
                    region.envelopeTime = 0.0f;
                }
            }
            else
            {
                // Zero attack time - jump to decay
                region.envelopeAmplitude = 1.0f;
                region.envelopePhase = RegionPlaybackState::Decay;
                region.envelopeTime = 0.0f;
            }
        }
        else if (region.envelopePhase == RegionPlaybackState::Decay)
        {
            if (decaySamples > 0.0f)
            {
                region.envelopeAmplitude = 1.0f - (region.envelopeTime / decaySamples);
                
                if (region.envelopeTime >= decaySamples || region.envelopeAmplitude <= 0.0f)
                {
                    region.envelopeAmplitude = 0.0f;
                    region.envelopePhase = RegionPlaybackState::Idle;
                    region.isActive = false;  // Stop playback when envelope completes
                }
            }
            else
            {
                // Zero decay time - stop immediately
                region.envelopeAmplitude = 0.0f;
                region.envelopePhase = RegionPlaybackState::Idle;
                region.isActive = false;
            }
        }
        else  // Idle
        {
            region.envelopeAmplitude = 0.0f;
        }
        
        region.envelopeTime += 1.0f;
    }
}

void MuSamAudioProcessor::applyPan(int regionIndex, juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    if (buffer.getNumChannels() < 2)
        return;  // Panning only works for stereo
    
    juce::String prefix = "region_" + juce::String(regionIndex + 1) + "_";
    
    auto* panParam = parameters.getRawParameterValue(prefix + "pan");
    if (panParam == nullptr)
        return;
    
    const float panPercent = panParam->load();  // -100% to +100%
    const float panNormalized = panPercent / 100.0f;  // -1.0 to +1.0
    
    // Equal-power panning
    const float leftGain = std::cos((panNormalized + 1.0f) * juce::MathConstants<float>::pi / 4.0f);
    const float rightGain = std::sin((panNormalized + 1.0f) * juce::MathConstants<float>::pi / 4.0f);
    
    buffer.applyGain(0, startSample, numSamples, leftGain);
    buffer.applyGain(1, startSample, numSamples, rightGain);
}

// ============================================================================
// Phase 4.3: Pitch Shifting Implementation (Granular Synthesis)
// ============================================================================

float MuSamAudioProcessor::calculatePitchRatio(int regionIndex)
{
    juce::String prefix = "region_" + juce::String(regionIndex + 1) + "_";
    auto* pitchParam = parameters.getRawParameterValue(prefix + "pitch");
    
    if (pitchParam != nullptr)
    {
        const float pitchSemitones = pitchParam->load();
        return std::pow(2.0f, pitchSemitones / 12.0f);
    }
    
    return 1.0f;
}

float MuSamAudioProcessor::hannWindow(float position)
{
    // Hann window: 0.5 * (1 - cos(2π * position))
    return 0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * position));
}

void MuSamAudioProcessor::applyPitchShift(int regionIndex, juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    auto& region = regionStates[regionIndex];
    
    // Update pitch ratio
    region.pitchRatio = calculatePitchRatio(regionIndex);
    
    // If no pitch shift, skip processing
    if (std::abs(region.pitchRatio - 1.0f) < 0.001f)
        return;
    
    const int grainSize = 1024;  // ~21ms @ 48kHz
    const int overlap = 512;     // 50% overlap
    const int numChannels = buffer.getNumChannels();
    const int grainBufferSize = region.grainBuffer.getNumSamples();
    
    // Grain spacing based on pitch ratio
    const float grainSpacing = static_cast<float>(grainSize) / region.pitchRatio;
    
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Check if we need to start a new grain
        if (region.grainPosition >= grainSpacing)
        {
            // Start new grain: copy from input buffer to grain buffer
            const int grainStart = static_cast<int>(region.grainWritePosition) % grainBufferSize;
            
            for (int channel = 0; channel < numChannels; ++channel)
            {
                // Copy grainSize samples from input (with wrapping)
                for (int i = 0; i < grainSize; ++i)
                {
                    const int srcIndex = (startSample + sample + i) % buffer.getNumSamples();
                    const int dstIndex = (grainStart + i) % grainBufferSize;
                    region.grainBuffer.setSample(channel, dstIndex, buffer.getSample(channel, srcIndex));
                }
            }
            
            region.grainPosition = 0.0f;
            region.grainWritePosition = std::fmod(region.grainWritePosition + grainSpacing, static_cast<float>(grainBufferSize));
        }
        
        // Read from grain buffer with windowing
        const float grainPos = region.grainPosition / static_cast<float>(grainSize);
        const float window = hannWindow(grainPos);
        
        const int readPos = static_cast<int>(region.grainWritePosition + region.grainPosition) % grainBufferSize;
        
        for (int channel = 0; channel < numChannels; ++channel)
        {
            const float grainSample = region.grainBuffer.getSample(channel, readPos);
            const float windowedSample = grainSample * window;
            
            // Overlap-add to output
            buffer.addSample(channel, startSample + sample, windowedSample);
        }
        
        region.grainPosition += 1.0f;
    }
}

// ============================================================================
// Phase 4.4: Sequencer Implementation
// ============================================================================

int MuSamAudioProcessor::getActiveRegionForStep(int step)
{
    juce::String paramId = "step_" + juce::String(step + 1) + "_region";
    auto* param = parameters.getRawParameterValue(paramId);
    
    if (param != nullptr)
    {
        const int choiceIndex = static_cast<int>(param->load());
        // Choice 0 = None, 1-5 = Region 1-5
        if (choiceIndex > 0 && choiceIndex <= 5)
        {
            return choiceIndex - 1;  // Convert to 0-based index
        }
    }
    
    return -1;  // No region assigned
}

void MuSamAudioProcessor::advanceSequencerStep()
{
    // Get playback mode
    auto* playbackModeParam = parameters.getRawParameterValue("playback_mode");
    int playbackMode = 0;  // Default: Sequential
    if (playbackModeParam != nullptr)
    {
        playbackMode = static_cast<int>(playbackModeParam->load());
    }
    
    int nextStep = sequencerState.currentStep;
    
    if (playbackMode == 0)  // Sequential
    {
        nextStep = (sequencerState.currentStep + 1) % 8;
    }
    else if (playbackMode == 1)  // Random
    {
        nextStep = juce::Random::getSystemRandom().nextInt(8);
    }
    else  // Custom (same as Sequential for now)
    {
        nextStep = (sequencerState.currentStep + 1) % 8;
    }
    
    // Check loop mode
    auto* loopModeParam = parameters.getRawParameterValue("loop_mode");
    bool loopEnabled = true;
    if (loopModeParam != nullptr)
    {
        loopEnabled = loopModeParam->load() > 0.5f;
    }
    
    if (!loopEnabled && nextStep == 0 && sequencerState.currentStep == 7)
    {
        // End of sequence, stop playback
        sequencerState.isPlaying = false;
        isPlaying = false;
        return;
    }
    
    // Deactivate previous region
    int previousRegion = getActiveRegionForStep(sequencerState.currentStep);
    if (previousRegion >= 0)
    {
        sequencerState.previousRegion = previousRegion;
    }
    
    // Activate new step's region
    sequencerState.currentStep = nextStep;
    sequencerState.stepTime = 0.0f;
    sequencerState.crossfadePosition = 0.0f;
    
    int newRegion = getActiveRegionForStep(nextStep);
    
    // Deactivate all regions
    for (int i = 0; i < 5; ++i)
    {
        regionStates[i].isActive = false;
    }
    
    // Activate new region
    if (newRegion >= 0)
    {
        regionStates[newRegion].isActive = true;
        regionStates[newRegion].samplePosition = static_cast<float>(regionStates[newRegion].startSample);
        
        // Reset envelope to attack
        regionStates[newRegion].envelopeAmplitude = 0.0f;
        regionStates[newRegion].envelopeTime = 0.0f;
        regionStates[newRegion].envelopePhase = RegionPlaybackState::Attack;
    }
}

void MuSamAudioProcessor::updateSequencer(int numSamples)
{
    sequencerState.stepTime += static_cast<float>(numSamples);
    
    // Check if we need to advance to next step
    if (sequencerState.stepTime >= sequencerState.stepDuration)
    {
        advanceSequencerStep();
    }
    
    // Update crossfade position
    auto* crossfadeTimeParam = parameters.getRawParameterValue("crossfade_time");
    float crossfadeTimeMs = 50.0f;  // Default
    if (crossfadeTimeParam != nullptr)
    {
        crossfadeTimeMs = crossfadeTimeParam->load();
    }
    
    const float crossfadeSamples = (crossfadeTimeMs / 1000.0f) * static_cast<float>(currentSampleRate);
    
    if (sequencerState.crossfadePosition < 1.0f && crossfadeSamples > 0.0f)
    {
        sequencerState.crossfadePosition += static_cast<float>(numSamples) / crossfadeSamples;
        sequencerState.crossfadePosition = juce::jmin(1.0f, sequencerState.crossfadePosition);
    }
}

void MuSamAudioProcessor::applyCrossfade(juce::AudioBuffer<float>& buffer, int startSample, int numSamples, int outgoingRegion, int incomingRegion)
{
    // Equal-power crossfade
    const float fadePos = sequencerState.crossfadePosition;
    const float incomingGain = std::sqrt(fadePos);
    
    // Apply crossfade gain (incoming buffer already has correct gain)
    buffer.applyGain(startSample, numSamples, incomingGain);
    
    // Note: Outgoing region processing would need to be stored separately
    // For now, simplified crossfade on incoming buffer only
    juce::ignoreUnused(outgoingRegion, incomingRegion);
}

// ============================================================================
// JUCE Plugin Factory
// ============================================================================

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MuSamAudioProcessor();
}

