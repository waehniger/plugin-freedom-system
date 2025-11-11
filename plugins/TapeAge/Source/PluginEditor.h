#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"

/**
 * TapeAge Plugin Editor (WebView-based)
 *
 * CRITICAL: Member declaration order prevents release build crashes.
 *
 * Destruction order (reverse of declaration):
 * 1. Attachments destroyed FIRST (stop using relays and WebView)
 * 2. WebView destroyed SECOND (safe, attachments are gone)
 * 3. Relays destroyed LAST (safe, nothing using them)
 */

class TapeAgeAudioProcessorEditor : public juce::AudioProcessorEditor,
                                     private juce::Timer  // For VU meter updates
{
public:
    TapeAgeAudioProcessorEditor(TapeAgeAudioProcessor& p);
    ~TapeAgeAudioProcessorEditor() override;

    // AudioProcessorEditor overrides
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // Timer callback for VU meter updates
    void timerCallback() override;

    /**
     * Resource provider (JUCE 8 pattern)
     * Maps URLs to embedded binary data.
     */
    std::optional<juce::WebBrowserComponent::Resource> getResource(
        const juce::String& url
    );

    // Reference to audio processor
    TapeAgeAudioProcessor& processorRef;

    // ========================================================================
    // ⚠️ CRITICAL MEMBER DECLARATION ORDER ⚠️
    //
    // Order: Relays → WebView → Attachments
    // DO NOT REORDER without understanding destructor sequence!
    // ========================================================================

    // ------------------------------------------------------------------------
    // 1️⃣ RELAYS FIRST (created first, destroyed last)
    // ------------------------------------------------------------------------
    std::unique_ptr<juce::WebSliderRelay> driveRelay;
    std::unique_ptr<juce::WebSliderRelay> ageRelay;
    std::unique_ptr<juce::WebSliderRelay> mixRelay;

    // ------------------------------------------------------------------------
    // 2️⃣ WEBVIEW SECOND
    // ------------------------------------------------------------------------
    std::unique_ptr<juce::WebBrowserComponent> webView;

    // ------------------------------------------------------------------------
    // 3️⃣ PARAMETER ATTACHMENTS LAST (created last, destroyed first)
    // ------------------------------------------------------------------------
    std::unique_ptr<juce::WebSliderParameterAttachment> driveAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> ageAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> mixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TapeAgeAudioProcessorEditor)
};
