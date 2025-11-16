#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class MuSamAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    MuSamAudioProcessorEditor(MuSamAudioProcessor&);
    ~MuSamAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    MuSamAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MuSamAudioProcessorEditor)
};

