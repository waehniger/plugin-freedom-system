#include "PluginProcessor.h"
#include "PluginEditor.h"

OrganicHatsAudioProcessor::OrganicHatsAudioProcessor()
    : AudioProcessor(BusesProperties()
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

OrganicHatsAudioProcessor::~OrganicHatsAudioProcessor()
{
}

void OrganicHatsAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Initialization will be added in Stage 4
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void OrganicHatsAudioProcessor::releaseResources()
{
    // Cleanup will be added in Stage 4
}

void OrganicHatsAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);

    // Clear output buffer (silence for Stage 2 - DSP added in Stage 4)
    buffer.clear();
}

juce::AudioProcessorEditor* OrganicHatsAudioProcessor::createEditor()
{
    return new OrganicHatsAudioProcessorEditor(*this);
}

void OrganicHatsAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // State management will be added in Stage 3
    juce::ignoreUnused(destData);
}

void OrganicHatsAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // State management will be added in Stage 3
    juce::ignoreUnused(data, sizeInBytes);
}

// Factory function
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OrganicHatsAudioProcessor();
}
