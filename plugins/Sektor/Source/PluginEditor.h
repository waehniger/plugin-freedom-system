#pragma once
#include "PluginProcessor.h"

class SektorAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit SektorAudioProcessorEditor(SektorAudioProcessor&);
    ~SektorAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    SektorAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SektorAudioProcessorEditor)
};
