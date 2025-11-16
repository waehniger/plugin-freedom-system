#include "PluginProcessor.h"
#include "PluginEditor.h"

MuSamAudioProcessorEditor::MuSamAudioProcessorEditor(MuSamAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    // Set plugin window size (will be replaced with WebView in Stage 3)
    setSize(400, 300);
}

MuSamAudioProcessorEditor::~MuSamAudioProcessorEditor()
{
}

void MuSamAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Simple placeholder UI for Stage 1
    g.fillAll(juce::Colours::black);
    
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawText("MuSam", getLocalBounds(), juce::Justification::centred);
    
    g.setFont(14.0f);
    g.drawText("Stage 1: Foundation + Shell Complete", 
               10, getHeight() - 30, getWidth() - 20, 20,
               juce::Justification::centredLeft);
}

void MuSamAudioProcessorEditor::resized()
{
    // Layout will be implemented in Stage 3 with WebView
}

