#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Voice Implementation
//==============================================================================

SektorAudioProcessor::Voice::Voice()
    : grainPhase(0.0f)
    , grainTrigger(0.0f)
    , playing(false)
    , midiNoteNumber(0)
    , noteVelocity(0.0f)
    , currentSampleRate(44100.0)
    , maxGrainSamples(0)
{
}

void SektorAudioProcessor::Voice::prepare(double sampleRate, int maxGrainSize)
{
    currentSampleRate = sampleRate;
    maxGrainSamples = maxGrainSize;

    // Generate test sample (1 second of 440Hz sine wave)
    generateTestSample(sampleRate);

    // Pre-generate Hann window for common grain sizes
    // We'll regenerate dynamically in processBlock for now
    hannWindow.resize(maxGrainSize);
}

void SektorAudioProcessor::Voice::generateTestSample(double sampleRate)
{
    // Create 1 second of 440Hz sine wave for testing
    int sampleLength = static_cast<int>(sampleRate);
    sampleBuffer.setSize(1, sampleLength);  // Mono test sample

    float* samples = sampleBuffer.getWritePointer(0);
    float frequency = 440.0f;  // A4
    float phase = 0.0f;
    float phaseIncrement = (juce::MathConstants<float>::twoPi * frequency) / static_cast<float>(sampleRate);

    for (int i = 0; i < sampleLength; ++i)
    {
        samples[i] = std::sin(phase) * 0.5f;  // 50% amplitude to prevent clipping
        phase += phaseIncrement;
    }
}

void SektorAudioProcessor::Voice::generateHannWindow(int grainSize)
{
    // Resize and regenerate Hann window for current grain size
    hannWindow.resize(grainSize);

    for (int i = 0; i < grainSize; ++i)
    {
        // Hann window formula: 0.5 * (1 - cos(2π * i / (N-1)))
        float phase = juce::MathConstants<float>::twoPi * static_cast<float>(i) / static_cast<float>(grainSize - 1);
        hannWindow[i] = 0.5f * (1.0f - std::cos(phase));
    }
}

void SektorAudioProcessor::Voice::startNote(int midiNote, float velocity)
{
    midiNoteNumber = midiNote;
    noteVelocity = velocity;
    playing = true;
    grainPhase = 0.0f;
    grainTrigger = 0.0f;
}

void SektorAudioProcessor::Voice::stopNote()
{
    playing = false;
}

void SektorAudioProcessor::Voice::processBlock(juce::AudioBuffer<float>& output, int numSamples,
                                                float grainSizeMs, float pitchShiftSemitones)
{
    if (!playing || sampleBuffer.getNumSamples() == 0)
        return;

    // Calculate grain size in samples
    int grainSamples = static_cast<int>((grainSizeMs / 1000.0f) * static_cast<float>(currentSampleRate));
    grainSamples = juce::jlimit(100, maxGrainSamples, grainSamples);  // Clamp to reasonable range

    // Regenerate Hann window if grain size changed
    if (hannWindow.size() != static_cast<size_t>(grainSamples))
    {
        generateHannWindow(grainSamples);
    }

    // Calculate pitch shift rate (semitones to playback rate)
    float pitchRate = std::pow(2.0f, pitchShiftSemitones / 12.0f);

    // Get sample buffer info
    int sampleLength = sampleBuffer.getNumSamples();
    const float* sampleData = sampleBuffer.getReadPointer(0);

    // Process each output sample
    for (int i = 0; i < numSamples; ++i)
    {
        // Check if we need to trigger new grain (continuous retriggering)
        if (grainTrigger <= 0.0f)
        {
            grainPhase = 0.0f;  // Reset grain to start
            grainTrigger = static_cast<float>(grainSamples);  // Next grain triggers after current grain completes
        }

        // Generate current grain sample
        float outputSample = 0.0f;

        if (grainPhase < static_cast<float>(grainSamples))
        {
            // Read from sample buffer with pitch shift (simple playback rate, no interpolation yet)
            int sampleIndex = static_cast<int>(grainPhase * pitchRate) % sampleLength;
            float sample = sampleData[sampleIndex];

            // Apply Hann window
            int windowIndex = static_cast<int>(grainPhase);
            if (windowIndex >= 0 && windowIndex < grainSamples)
            {
                float windowValue = hannWindow[windowIndex];
                outputSample = sample * windowValue * noteVelocity;
            }

            grainPhase += 1.0f;
        }

        // Write to output buffer (mono to stereo)
        output.addSample(0, i, outputSample);  // Left channel
        output.addSample(1, i, outputSample);  // Right channel

        grainTrigger -= 1.0f;
    }
}

//==============================================================================
// AudioProcessor Implementation
//==============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout SektorAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // GRAIN_SIZE - Grain duration (10-500 ms)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "GRAIN_SIZE", 1 },
        "Grain Size",
        juce::NormalisableRange<float>(10.0f, 500.0f, 0.1f),
        100.0f,
        "ms"
    ));

    // DENSITY - Grain trigger rate (1-200 grains/sec)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "DENSITY", 1 },
        "Density",
        juce::NormalisableRange<float>(1.0f, 200.0f, 0.1f),
        50.0f,
        "grains/sec"
    ));

    // PITCH_SHIFT - Transposition (-12 to +12 semitones)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "PITCH_SHIFT", 1 },
        "Pitch Shift",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f,
        "semitones"
    ));

    // SPACING - Grain position advancement multiplier (0.1-2.0)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "SPACING", 1 },
        "Spacing",
        juce::NormalisableRange<float>(0.1f, 2.0f, 0.01f),
        1.0f
    ));

    // REGION_START - Playback region start (0.0-1.0 normalized)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "REGION_START", 1 },
        "Region Start",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
        0.0f
    ));

    // REGION_END - Playback region end (0.0-1.0 normalized)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "REGION_END", 1 },
        "Region End",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
        1.0f
    ));

    // POLYPHONY_MODE - Mono (false) or Poly (true)
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "POLYPHONY_MODE", 1 },
        "Polyphony Mode",
        true  // Default: Polyphonic
    ));

    return layout;
}

SektorAudioProcessor::SektorAudioProcessor()
    : AudioProcessor(BusesProperties()
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true))  // Output-only (instrument)
    , parameters(*this, nullptr, "Parameters", createParameterLayout())
{
}

SektorAudioProcessor::~SektorAudioProcessor()
{
}

void SektorAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Prepare voice with maximum grain size (500ms at current sample rate)
    int maxGrainSize = static_cast<int>((500.0 / 1000.0) * sampleRate);
    monoVoice.prepare(sampleRate, maxGrainSize);

    juce::ignoreUnused(samplesPerBlock);
}

void SektorAudioProcessor::releaseResources()
{
    // No resources to release in Phase 2.1 (all preallocated)
}

void SektorAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Clear output buffer
    buffer.clear();

    // Process MIDI messages (note-on/note-off)
    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();

        if (message.isNoteOn())
        {
            int noteNumber = message.getNoteNumber();
            float velocity = message.getVelocity() / 127.0f;
            monoVoice.startNote(noteNumber, velocity);
        }
        else if (message.isNoteOff())
        {
            monoVoice.stopNote();
        }
    }

    // Read parameters (atomic, real-time safe)
    auto* grainSizeParam = parameters.getRawParameterValue("GRAIN_SIZE");
    auto* pitchShiftParam = parameters.getRawParameterValue("PITCH_SHIFT");

    float grainSizeMs = grainSizeParam->load();
    float pitchShiftSemitones = pitchShiftParam->load();

    // Process voice if playing
    if (monoVoice.isPlaying())
    {
        monoVoice.processBlock(buffer, buffer.getNumSamples(), grainSizeMs, pitchShiftSemitones);
    }
}

juce::AudioProcessorEditor* SektorAudioProcessor::createEditor()
{
    return new SektorAudioProcessorEditor(*this);
}

void SektorAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void SektorAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// Factory function
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SektorAudioProcessor();
}
