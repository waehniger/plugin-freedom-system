#include "PluginEditor.h"

SektorAudioProcessorEditor::SektorAudioProcessorEditor(SektorAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(900, 600);  // Match mockup size
}

SektorAudioProcessorEditor::~SektorAudioProcessorEditor()
{
}

void SektorAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawFittedText("Sektor - Stage 1", getLocalBounds(), juce::Justification::centred, 1);

    g.setFont(14.0f);
    g.drawFittedText("7 parameters implemented",
                     getLocalBounds().reduced(20).removeFromBottom(30),
                     juce::Justification::centred, 1);
}

void SektorAudioProcessorEditor::resized()
{
    // Layout will be added in Stage 3 (GUI)
}
