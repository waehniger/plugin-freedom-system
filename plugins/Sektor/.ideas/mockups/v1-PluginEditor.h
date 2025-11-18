#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

/**
 * Sektor WebView-based Plugin Editor
 *
 * CRITICAL: Member declaration order prevents 90% of release build crashes.
 * Order: Relays → WebView → Attachments
 *
 * This editor implements a granular sampler UI with:
 * - 4 continuous parameters (grain_size, density, pitch_shift, spacing)
 * - 2 region marker parameters (region_start, region_end)
 * - 1 boolean parameter (polyphony_mode)
 * - Sample loading (drag & drop + file browser)
 * - Interactive waveform display with region highlighting
 * - Real-time CPU monitoring
 */

class SektorAudioProcessorEditor : public juce::AudioProcessorEditor,
                                    private juce::Timer
{
public:
    /**
     * Constructor
     * @param p Reference to the Sektor audio processor
     */
    SektorAudioProcessorEditor(SektorAudioProcessor& p);

    /**
     * Destructor
     * Members destroyed in reverse order of declaration.
     */
    ~SektorAudioProcessorEditor() override;

    // AudioProcessorEditor overrides
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    /**
     * Timer callback for real-time updates
     * - CPU usage monitoring
     * - Waveform display updates (when sample changes)
     */
    void timerCallback() override;

    /**
     * Resource provider for WebView
     * Maps URLs to embedded binary data.
     *
     * @param url Requested resource URL
     * @return Resource data and MIME type, or std::nullopt for 404
     */
    std::optional<juce::WebBrowserComponent::Resource> getResource(
        const juce::String& url
    );

    // Reference to audio processor
    SektorAudioProcessor& audioProcessor;

    // ========================================================================
    // ⚠️ CRITICAL MEMBER DECLARATION ORDER ⚠️
    //
    // Order: Relays → WebView → Attachments
    //
    // Members are destroyed in REVERSE order of declaration:
    // 1. Attachments destroyed FIRST (stop calling evaluateJavascript)
    // 2. WebView destroyed SECOND (safe, attachments are gone)
    // 3. Relays destroyed LAST (safe, nothing using them)
    //
    // DO NOT REORDER!
    // ========================================================================

    // ------------------------------------------------------------------------
    // 1️⃣ RELAYS FIRST (created first, destroyed last)
    // ------------------------------------------------------------------------
    //
    // 7 parameters total:
    // - 4 continuous sliders (grain_size, density, pitch_shift, spacing)
    // - 2 region markers (region_start, region_end)
    // - 1 boolean toggle (polyphony_mode)
    //
    std::unique_ptr<juce::WebSliderRelay> grainSizeRelay;
    std::unique_ptr<juce::WebSliderRelay> densityRelay;
    std::unique_ptr<juce::WebSliderRelay> pitchShiftRelay;
    std::unique_ptr<juce::WebSliderRelay> spacingRelay;
    std::unique_ptr<juce::WebSliderRelay> regionStartRelay;
    std::unique_ptr<juce::WebSliderRelay> regionEndRelay;
    std::unique_ptr<juce::WebToggleButtonRelay> polyphonyModeRelay;

    // ------------------------------------------------------------------------
    // 2️⃣ WEBVIEW SECOND (created after relays, destroyed before relays)
    // ------------------------------------------------------------------------
    //
    // WebBrowserComponent is the HTML rendering engine.
    // Depends on relays (registered via withOptionsFrom).
    //
    std::unique_ptr<juce::WebBrowserComponent> webView;

    // ------------------------------------------------------------------------
    // 3️⃣ PARAMETER ATTACHMENTS LAST (created last, destroyed first)
    // ------------------------------------------------------------------------
    //
    // Attachments synchronize APVTS parameters with relay state.
    // Depend on BOTH the relay AND the WebView.
    //
    // MUST be declared AFTER WebView to ensure correct destruction order.
    //
    std::unique_ptr<juce::WebSliderParameterAttachment> grainSizeAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> densityAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> pitchShiftAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> spacingAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> regionStartAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> regionEndAttachment;
    std::unique_ptr<juce::WebToggleButtonParameterAttachment> polyphonyModeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SektorAudioProcessorEditor)
};
