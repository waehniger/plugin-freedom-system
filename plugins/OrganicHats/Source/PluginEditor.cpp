#include "PluginEditor.h"

OrganicHatsAudioProcessorEditor::OrganicHatsAudioProcessorEditor(OrganicHatsAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(600, 590);
}

OrganicHatsAudioProcessorEditor::~OrganicHatsAudioProcessorEditor()
{
}

void OrganicHatsAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawFittedText("OrganicHats - Stage 2", getLocalBounds(), juce::Justification::centred, 1);
}

void OrganicHatsAudioProcessorEditor::resized()
{
    // Layout will be added in Stage 5
}
