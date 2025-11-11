#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout TapeAgeAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // drive - Tape saturation amount
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "drive", 1 },
        "Drive",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f, 1.0f),  // 0-100%, linear
        0.5f  // Default: 50%
    ));

    // age - Tape degradation amount
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "age", 1 },
        "Age",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f, 1.0f),  // 0-100%, linear
        0.25f  // Default: 25%
    ));

    // mix - Dry/wet blend
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "mix", 1 },
        "Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f, 1.0f),  // 0-100%, linear
        1.0f  // Default: 100% wet
    ));

    return layout;
}

TapeAgeAudioProcessor::TapeAgeAudioProcessor()
    : AudioProcessor(BusesProperties()
                        .withInput("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , parameters(*this, nullptr, "Parameters", createParameterLayout())
{
}

TapeAgeAudioProcessor::~TapeAgeAudioProcessor()
{
}

void TapeAgeAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Initialization will be added in Stage 4
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void TapeAgeAudioProcessor::releaseResources()
{
    // Cleanup will be added in Stage 4
}

void TapeAgeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);

    // Parameter access example (for Stage 4 DSP implementation):
    // auto* driveParam = parameters.getRawParameterValue("drive");
    // float driveValue = driveParam->load();  // Atomic read (real-time safe)

    // Pass-through for Stage 3 (DSP implementation happens in Stage 4)
    // Audio routing is already handled by JUCE
}

juce::AudioProcessorEditor* TapeAgeAudioProcessor::createEditor()
{
    return new TapeAgeAudioProcessorEditor(*this);
}

void TapeAgeAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void TapeAgeAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// Factory function
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TapeAgeAudioProcessor();
}
