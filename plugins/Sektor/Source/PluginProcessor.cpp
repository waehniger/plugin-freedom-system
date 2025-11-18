#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Voice Implementation
//==============================================================================

SektorAudioProcessor::Voice::Voice()
    : grainPhase(0.0f)
    , samplesUntilNextGrain(0)
    , playing(false)
    , midiNoteNumber(0)
    , noteVelocity(0.0f)
    , currentSampleRate(44100.0)
    , maxGrainSamples(0)
{
    activeGrains.resize(MAX_ACTIVE_GRAINS);
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
    samplesUntilNextGrain = 0;  // Trigger first grain immediately

    // Clear all active grains
    for (auto& grain : activeGrains)
    {
        grain.isActive = false;
    }
}

void SektorAudioProcessor::Voice::stopNote()
{
    playing = false;
}

void SektorAudioProcessor::Voice::generateGrain(int grainSamples, float spacing)
{
    // Find free grain slot (or kill oldest)
    ActiveGrain* targetGrain = nullptr;

    // First, try to find an inactive grain
    for (auto& grain : activeGrains)
    {
        if (!grain.isActive)
        {
            targetGrain = &grain;
            break;
        }
    }

    // If no free slots, kill the oldest (first active grain in list)
    if (targetGrain == nullptr)
    {
        targetGrain = &activeGrains[0];
    }

    // Initialize new grain
    targetGrain->readPosition = 0.0f;
    targetGrain->samplesRemaining = grainSamples;
    targetGrain->grainStartPhase = grainPhase;
    targetGrain->isActive = true;

    // Advance grain phase for next grain (spacing controls advancement)
    grainPhase += static_cast<float>(grainSamples) * spacing;

    // Wrap grain phase at sample buffer length
    int sampleLength = sampleBuffer.getNumSamples();
    while (grainPhase >= static_cast<float>(sampleLength))
    {
        grainPhase -= static_cast<float>(sampleLength);
    }
}

float SektorAudioProcessor::Voice::readFractionalSample(float position)
{
    int sampleLength = sampleBuffer.getNumSamples();
    const float* sampleData = sampleBuffer.getReadPointer(0);

    // Wrap position
    while (position >= static_cast<float>(sampleLength))
    {
        position -= static_cast<float>(sampleLength);
    }
    while (position < 0.0f)
    {
        position += static_cast<float>(sampleLength);
    }

    // Linear interpolation
    int index1 = static_cast<int>(position);
    int index2 = (index1 + 1) % sampleLength;
    float frac = position - static_cast<float>(index1);

    float sample1 = sampleData[index1];
    float sample2 = sampleData[index2];

    return sample1 + frac * (sample2 - sample1);
}

void SektorAudioProcessor::Voice::processBlock(juce::AudioBuffer<float>& output, int numSamples,
                                                float grainSizeMs, float density, float pitchShiftSemitones, float spacing)
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

    // Calculate grain trigger interval from density (grains per second)
    int grainInterval = static_cast<int>(currentSampleRate / density);
    grainInterval = juce::jmax(1, grainInterval);  // Prevent division by zero

    // Process each output sample
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Check if we need to trigger new grain
        if (samplesUntilNextGrain <= 0)
        {
            generateGrain(grainSamples, spacing);
            samplesUntilNextGrain = grainInterval;
        }

        // Mix all active grains
        float outputSample = 0.0f;

        for (auto& grain : activeGrains)
        {
            if (!grain.isActive)
                continue;

            // Calculate source position with pitch shift
            float sourcePosition = grain.grainStartPhase + (grain.readPosition * pitchRate);

            // Read sample with linear interpolation
            float sampleValue = readFractionalSample(sourcePosition);

            // Apply Hann window
            int windowIndex = static_cast<int>(grain.readPosition);
            if (windowIndex >= 0 && windowIndex < grainSamples)
            {
                float windowValue = hannWindow[windowIndex];
                outputSample += sampleValue * windowValue;
            }

            // Advance grain read position
            grain.readPosition += 1.0f;
            grain.samplesRemaining--;

            // Deactivate grain when finished
            if (grain.samplesRemaining <= 0)
            {
                grain.isActive = false;
            }
        }

        // Apply velocity and write to output buffer (mono to stereo)
        outputSample *= noteVelocity;
        output.addSample(0, sample, outputSample);  // Left channel
        output.addSample(1, sample, outputSample);  // Right channel

        samplesUntilNextGrain--;
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
    auto* densityParam = parameters.getRawParameterValue("DENSITY");
    auto* pitchShiftParam = parameters.getRawParameterValue("PITCH_SHIFT");
    auto* spacingParam = parameters.getRawParameterValue("SPACING");

    float grainSizeMs = grainSizeParam->load();
    float density = densityParam->load();
    float pitchShiftSemitones = pitchShiftParam->load();
    float spacing = spacingParam->load();

    // Process voice if playing
    if (monoVoice.isPlaying())
    {
        monoVoice.processBlock(buffer, buffer.getNumSamples(), grainSizeMs, density, pitchShiftSemitones, spacing);
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
