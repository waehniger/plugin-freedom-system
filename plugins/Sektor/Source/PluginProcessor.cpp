#include "PluginProcessor.h"
#include "PluginEditor.h"

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
    // DSP initialization will be added in Stage 2
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void SektorAudioProcessor::releaseResources()
{
    // Cleanup will be added in Stage 2
}

void SektorAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Clear output buffer (silence for Stage 1)
    buffer.clear();

    // MIDI processing will be added in Stage 2
    juce::ignoreUnused(midiMessages);

    // Parameter access example (for Stage 2 DSP implementation):
    // auto* grainSizeParam = parameters.getRawParameterValue("GRAIN_SIZE");
    // float grainSizeMs = grainSizeParam->load();  // Atomic read (real-time safe)
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
