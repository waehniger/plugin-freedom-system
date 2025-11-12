#pragma once
#include "PluginProcessor.h"

class OrganicHatsAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit OrganicHatsAudioProcessorEditor(OrganicHatsAudioProcessor&);
    ~OrganicHatsAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    OrganicHatsAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OrganicHatsAudioProcessorEditor)
};
