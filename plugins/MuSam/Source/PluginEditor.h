#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"

/**
 * WebView-based Plugin Editor for MuSam
 *
 * CRITICAL: Member order prevents 90% of release build crashes.
 *
 * Destruction order (reverse of declaration):
 * 1. Attachments destroyed FIRST (stop using relays and WebView)
 * 2. WebView destroyed SECOND (safe, attachments are gone)
 * 3. Relays destroyed LAST (safe, nothing using them)
 */

// Forward declaration
class MuSamAudioProcessorEditor;

//==============================================================================
// Drag & Drop Overlay Component
//==============================================================================
// This invisible component sits on top of the WebView and intercepts drag events
// while allowing mouse clicks to pass through to the WebView for UI interactions.
class DragDropOverlay : public juce::Component,
                         public juce::FileDragAndDropTarget
{
public:
    DragDropOverlay(MuSamAudioProcessorEditor& editor);
    
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void fileDragEnter(const juce::StringArray& files, int x, int y) override;
    void fileDragMove(const juce::StringArray& files, int x, int y) override;
    void fileDragExit(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;
    void paint(juce::Graphics& g) override;
    bool hitTest(int x, int y) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseMove(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    
private:
    MuSamAudioProcessorEditor& editorRef;
};

class MuSamAudioProcessorEditor : public juce::AudioProcessorEditor,
                                   public juce::FileDragAndDropTarget,
                                   public juce::Timer,
                                   public juce::AudioProcessorValueTreeState::Listener
{
public:
    /**
     * Constructor
     * @param p Reference to the audio processor
     */
    explicit MuSamAudioProcessorEditor(MuSamAudioProcessor& p);

    /**
     * Destructor
     * Members destroyed in reverse order of declaration.
     */
    ~MuSamAudioProcessorEditor() override;

    // AudioProcessorEditor overrides
    void paint(juce::Graphics&) override;
    void resized() override;
    
    // Timer override (for debug logging test)
    void timerCallback() override;
    
    // Mouse event overrides (for debug logging test)
    void mouseDown(const juce::MouseEvent& e) override;
    
    // AudioProcessorValueTreeState::Listener override (for parameter change logging)
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    // FileDragAndDropTarget overrides
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void fileDragEnter(const juce::StringArray& files, int x, int y) override;
    void fileDragMove(const juce::StringArray& files, int x, int y) override;
    void fileDragExit(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

private:
    /**
     * Resource provider (JUCE 8 required pattern)
     * Maps URLs to embedded binary data.
     *
     * @param url Requested resource URL (e.g., "/", "/js/juce/index.js")
     * @return Resource data and MIME type, or std::nullopt for 404
     */
    std::optional<juce::WebBrowserComponent::Resource> getResource(
        const juce::String& url
    );

    /**
     * Debug helper: Write log message to file
     * @param message Log message to write
     */
    void writeDebugLog(const juce::String& message);

    // Reference to audio processor
    MuSamAudioProcessor& processorRef;

    // ========================================================================
    // ⚠️ CRITICAL MEMBER DECLARATION ORDER ⚠️
    //
    // Order: Relays → WebView → Attachments
    //
    // Members are destroyed in REVERSE order of declaration.
    // DO NOT REORDER without understanding destructor sequence!
    // ========================================================================

    // ------------------------------------------------------------------------
    // 1️⃣ RELAYS FIRST (created first, destroyed last)
    // Phase 5.2: All 53 parameters
    // ------------------------------------------------------------------------
    
    // Global parameters (2)
    std::unique_ptr<juce::WebSliderRelay> speedRelay;
    std::unique_ptr<juce::WebSliderRelay> volumeRelay;
    
    // Region parameters (5 regions × 8 parameters = 40)
    // Region 1
    std::unique_ptr<juce::WebSliderRelay> region_1_startRelay;
    std::unique_ptr<juce::WebSliderRelay> region_1_endRelay;
    std::unique_ptr<juce::WebSliderRelay> region_1_pitchRelay;
    std::unique_ptr<juce::WebSliderRelay> region_1_filter_cutoffRelay;
    std::unique_ptr<juce::WebSliderRelay> region_1_filter_resonanceRelay;
    std::unique_ptr<juce::WebSliderRelay> region_1_attackRelay;
    std::unique_ptr<juce::WebSliderRelay> region_1_decayRelay;
    std::unique_ptr<juce::WebSliderRelay> region_1_panRelay;
    
    // Region 2
    std::unique_ptr<juce::WebSliderRelay> region_2_startRelay;
    std::unique_ptr<juce::WebSliderRelay> region_2_endRelay;
    std::unique_ptr<juce::WebSliderRelay> region_2_pitchRelay;
    std::unique_ptr<juce::WebSliderRelay> region_2_filter_cutoffRelay;
    std::unique_ptr<juce::WebSliderRelay> region_2_filter_resonanceRelay;
    std::unique_ptr<juce::WebSliderRelay> region_2_attackRelay;
    std::unique_ptr<juce::WebSliderRelay> region_2_decayRelay;
    std::unique_ptr<juce::WebSliderRelay> region_2_panRelay;
    
    // Region 3
    std::unique_ptr<juce::WebSliderRelay> region_3_startRelay;
    std::unique_ptr<juce::WebSliderRelay> region_3_endRelay;
    std::unique_ptr<juce::WebSliderRelay> region_3_pitchRelay;
    std::unique_ptr<juce::WebSliderRelay> region_3_filter_cutoffRelay;
    std::unique_ptr<juce::WebSliderRelay> region_3_filter_resonanceRelay;
    std::unique_ptr<juce::WebSliderRelay> region_3_attackRelay;
    std::unique_ptr<juce::WebSliderRelay> region_3_decayRelay;
    std::unique_ptr<juce::WebSliderRelay> region_3_panRelay;
    
    // Region 4
    std::unique_ptr<juce::WebSliderRelay> region_4_startRelay;
    std::unique_ptr<juce::WebSliderRelay> region_4_endRelay;
    std::unique_ptr<juce::WebSliderRelay> region_4_pitchRelay;
    std::unique_ptr<juce::WebSliderRelay> region_4_filter_cutoffRelay;
    std::unique_ptr<juce::WebSliderRelay> region_4_filter_resonanceRelay;
    std::unique_ptr<juce::WebSliderRelay> region_4_attackRelay;
    std::unique_ptr<juce::WebSliderRelay> region_4_decayRelay;
    std::unique_ptr<juce::WebSliderRelay> region_4_panRelay;
    
    // Region 5
    std::unique_ptr<juce::WebSliderRelay> region_5_startRelay;
    std::unique_ptr<juce::WebSliderRelay> region_5_endRelay;
    std::unique_ptr<juce::WebSliderRelay> region_5_pitchRelay;
    std::unique_ptr<juce::WebSliderRelay> region_5_filter_cutoffRelay;
    std::unique_ptr<juce::WebSliderRelay> region_5_filter_resonanceRelay;
    std::unique_ptr<juce::WebSliderRelay> region_5_attackRelay;
    std::unique_ptr<juce::WebSliderRelay> region_5_decayRelay;
    std::unique_ptr<juce::WebSliderRelay> region_5_panRelay;
    
    // Sequencer parameters (11)
    std::unique_ptr<juce::WebComboBoxRelay> step_1_regionRelay;
    std::unique_ptr<juce::WebComboBoxRelay> step_2_regionRelay;
    std::unique_ptr<juce::WebComboBoxRelay> step_3_regionRelay;
    std::unique_ptr<juce::WebComboBoxRelay> step_4_regionRelay;
    std::unique_ptr<juce::WebComboBoxRelay> step_5_regionRelay;
    std::unique_ptr<juce::WebComboBoxRelay> step_6_regionRelay;
    std::unique_ptr<juce::WebComboBoxRelay> step_7_regionRelay;
    std::unique_ptr<juce::WebComboBoxRelay> step_8_regionRelay;
    std::unique_ptr<juce::WebComboBoxRelay> playback_modeRelay;
    std::unique_ptr<juce::WebToggleButtonRelay> loop_modeRelay;
    std::unique_ptr<juce::WebSliderRelay> crossfade_timeRelay;

    // ------------------------------------------------------------------------
    // 2️⃣ WEBVIEW SECOND (created after relays, destroyed before relays)
    // ------------------------------------------------------------------------
    std::unique_ptr<juce::WebBrowserComponent> webView;
    
    // ------------------------------------------------------------------------
    // DRAG & DROP OVERLAY (invisible component that intercepts drag events)
    // ------------------------------------------------------------------------
    std::unique_ptr<DragDropOverlay> dragDropOverlay;

    // ------------------------------------------------------------------------
    // 3️⃣ PARAMETER ATTACHMENTS LAST (created last, destroyed first)
    // Phase 5.2: All 53 parameters
    // ------------------------------------------------------------------------
    
    // Global parameters (2)
    std::unique_ptr<juce::WebSliderParameterAttachment> speedAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> volumeAttachment;
    
    // Region parameters (40)
    // Region 1
    std::unique_ptr<juce::WebSliderParameterAttachment> region_1_startAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_1_endAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_1_pitchAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_1_filter_cutoffAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_1_filter_resonanceAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_1_attackAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_1_decayAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_1_panAttachment;
    
    // Region 2
    std::unique_ptr<juce::WebSliderParameterAttachment> region_2_startAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_2_endAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_2_pitchAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_2_filter_cutoffAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_2_filter_resonanceAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_2_attackAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_2_decayAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_2_panAttachment;
    
    // Region 3
    std::unique_ptr<juce::WebSliderParameterAttachment> region_3_startAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_3_endAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_3_pitchAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_3_filter_cutoffAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_3_filter_resonanceAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_3_attackAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_3_decayAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_3_panAttachment;
    
    // Region 4
    std::unique_ptr<juce::WebSliderParameterAttachment> region_4_startAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_4_endAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_4_pitchAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_4_filter_cutoffAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_4_filter_resonanceAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_4_attackAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_4_decayAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_4_panAttachment;
    
    // Region 5
    std::unique_ptr<juce::WebSliderParameterAttachment> region_5_startAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_5_endAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_5_pitchAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_5_filter_cutoffAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_5_filter_resonanceAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_5_attackAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_5_decayAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region_5_panAttachment;
    
    // Sequencer parameters (11)
    std::unique_ptr<juce::WebComboBoxParameterAttachment> step_1_regionAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> step_2_regionAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> step_3_regionAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> step_4_regionAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> step_5_regionAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> step_6_regionAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> step_7_regionAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> step_8_regionAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> playback_modeAttachment;
    std::unique_ptr<juce::WebToggleButtonParameterAttachment> loop_modeAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> crossfade_timeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MuSamAudioProcessorEditor)
};
