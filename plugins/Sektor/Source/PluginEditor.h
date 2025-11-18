#pragma once
#include "PluginProcessor.h"
#include <juce_gui_extra/juce_gui_extra.h>

class SektorAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit SektorAudioProcessorEditor(SektorAudioProcessor&);
    ~SektorAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    SektorAudioProcessor& processorRef;

    // ⚠️ MEMBER DECLARATION ORDER IS CRITICAL ⚠️
    // Members are destroyed in REVERSE order of declaration
    // Declare dependencies AFTER what they depend on

    // 1️⃣ RELAYS FIRST (no dependencies)
    std::unique_ptr<juce::WebSliderRelay> grainSizeRelay;
    std::unique_ptr<juce::WebSliderRelay> densityRelay;
    std::unique_ptr<juce::WebSliderRelay> pitchShiftRelay;
    std::unique_ptr<juce::WebSliderRelay> spacingRelay;

    // 2️⃣ WEBVIEW SECOND (depends on relays via withOptionsFrom)
    std::unique_ptr<juce::WebBrowserComponent> webView;

    // 3️⃣ ATTACHMENTS LAST (depend on both relays and webView)
    std::unique_ptr<juce::WebSliderParameterAttachment> grainSizeAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> densityAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> pitchShiftAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> spacingAttachment;

    // Helper for resource serving
    std::optional<juce::WebBrowserComponent::Resource> getResource(const juce::String& url);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SektorAudioProcessorEditor)
};
