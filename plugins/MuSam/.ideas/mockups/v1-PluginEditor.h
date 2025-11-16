#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

/**
 * MuSam WebView-based Plugin Editor
 *
 * CRITICAL: Member declaration order prevents release build crashes.
 * Order: Relays → WebView → Attachments
 *
 * Destruction sequence (reverse of declaration):
 * 1. Attachments destroyed FIRST (stop using relays and WebView)
 * 2. WebView destroyed SECOND (safe, attachments are gone)
 * 3. Relays destroyed LAST (safe, nothing using them)
 */

class MuSamAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    MuSamAudioProcessorEditor(MuSamAudioProcessor& p);
    ~MuSamAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    std::optional<juce::WebBrowserComponent::Resource> getResource(const juce::String& url);

    // Reference to audio processor
    MuSamAudioProcessor& audioProcessor;

    // ========================================================================
    // CRITICAL MEMBER DECLARATION ORDER
    // Order: Relays → WebView → Attachments
    // ========================================================================

    // ------------------------------------------------------------------------
    // 1. RELAYS FIRST (53 parameters)
    // ------------------------------------------------------------------------

    // Global parameters (2)
    std::unique_ptr<juce::WebSliderRelay> speedRelay;
    std::unique_ptr<juce::WebSliderRelay> volumeRelay;

    // Region 1 parameters (8)
    std::unique_ptr<juce::WebSliderRelay> region1StartRelay;
    std::unique_ptr<juce::WebSliderRelay> region1EndRelay;
    std::unique_ptr<juce::WebSliderRelay> region1PitchRelay;
    std::unique_ptr<juce::WebSliderRelay> region1FilterCutoffRelay;
    std::unique_ptr<juce::WebSliderRelay> region1FilterResonanceRelay;
    std::unique_ptr<juce::WebSliderRelay> region1AttackRelay;
    std::unique_ptr<juce::WebSliderRelay> region1DecayRelay;
    std::unique_ptr<juce::WebSliderRelay> region1PanRelay;

    // Region 2 parameters (8)
    std::unique_ptr<juce::WebSliderRelay> region2StartRelay;
    std::unique_ptr<juce::WebSliderRelay> region2EndRelay;
    std::unique_ptr<juce::WebSliderRelay> region2PitchRelay;
    std::unique_ptr<juce::WebSliderRelay> region2FilterCutoffRelay;
    std::unique_ptr<juce::WebSliderRelay> region2FilterResonanceRelay;
    std::unique_ptr<juce::WebSliderRelay> region2AttackRelay;
    std::unique_ptr<juce::WebSliderRelay> region2DecayRelay;
    std::unique_ptr<juce::WebSliderRelay> region2PanRelay;

    // Region 3 parameters (8)
    std::unique_ptr<juce::WebSliderRelay> region3StartRelay;
    std::unique_ptr<juce::WebSliderRelay> region3EndRelay;
    std::unique_ptr<juce::WebSliderRelay> region3PitchRelay;
    std::unique_ptr<juce::WebSliderRelay> region3FilterCutoffRelay;
    std::unique_ptr<juce::WebSliderRelay> region3FilterResonanceRelay;
    std::unique_ptr<juce::WebSliderRelay> region3AttackRelay;
    std::unique_ptr<juce::WebSliderRelay> region3DecayRelay;
    std::unique_ptr<juce::WebSliderRelay> region3PanRelay;

    // Region 4 parameters (8)
    std::unique_ptr<juce::WebSliderRelay> region4StartRelay;
    std::unique_ptr<juce::WebSliderRelay> region4EndRelay;
    std::unique_ptr<juce::WebSliderRelay> region4PitchRelay;
    std::unique_ptr<juce::WebSliderRelay> region4FilterCutoffRelay;
    std::unique_ptr<juce::WebSliderRelay> region4FilterResonanceRelay;
    std::unique_ptr<juce::WebSliderRelay> region4AttackRelay;
    std::unique_ptr<juce::WebSliderRelay> region4DecayRelay;
    std::unique_ptr<juce::WebSliderRelay> region4PanRelay;

    // Region 5 parameters (8)
    std::unique_ptr<juce::WebSliderRelay> region5StartRelay;
    std::unique_ptr<juce::WebSliderRelay> region5EndRelay;
    std::unique_ptr<juce::WebSliderRelay> region5PitchRelay;
    std::unique_ptr<juce::WebSliderRelay> region5FilterCutoffRelay;
    std::unique_ptr<juce::WebSliderRelay> region5FilterResonanceRelay;
    std::unique_ptr<juce::WebSliderRelay> region5AttackRelay;
    std::unique_ptr<juce::WebSliderRelay> region5DecayRelay;
    std::unique_ptr<juce::WebSliderRelay> region5PanRelay;

    // Sequencer parameters (11)
    std::unique_ptr<juce::WebComboBoxRelay> step1RegionRelay;
    std::unique_ptr<juce::WebComboBoxRelay> step2RegionRelay;
    std::unique_ptr<juce::WebComboBoxRelay> step3RegionRelay;
    std::unique_ptr<juce::WebComboBoxRelay> step4RegionRelay;
    std::unique_ptr<juce::WebComboBoxRelay> step5RegionRelay;
    std::unique_ptr<juce::WebComboBoxRelay> step6RegionRelay;
    std::unique_ptr<juce::WebComboBoxRelay> step7RegionRelay;
    std::unique_ptr<juce::WebComboBoxRelay> step8RegionRelay;
    std::unique_ptr<juce::WebComboBoxRelay> playbackModeRelay;
    std::unique_ptr<juce::WebToggleButtonRelay> loopModeRelay;
    std::unique_ptr<juce::WebSliderRelay> crossfadeTimeRelay;

    // ------------------------------------------------------------------------
    // 2. WEBVIEW SECOND
    // ------------------------------------------------------------------------
    std::unique_ptr<juce::WebBrowserComponent> webView;

    // ------------------------------------------------------------------------
    // 3. PARAMETER ATTACHMENTS LAST (53 parameters)
    // ------------------------------------------------------------------------

    // Global parameters (2)
    std::unique_ptr<juce::WebSliderParameterAttachment> speedAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> volumeAttachment;

    // Region 1 parameters (8)
    std::unique_ptr<juce::WebSliderParameterAttachment> region1StartAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region1EndAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region1PitchAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region1FilterCutoffAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region1FilterResonanceAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region1AttackAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region1DecayAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region1PanAttachment;

    // Region 2 parameters (8)
    std::unique_ptr<juce::WebSliderParameterAttachment> region2StartAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region2EndAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region2PitchAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region2FilterCutoffAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region2FilterResonanceAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region2AttackAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region2DecayAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region2PanAttachment;

    // Region 3 parameters (8)
    std::unique_ptr<juce::WebSliderParameterAttachment> region3StartAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region3EndAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region3PitchAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region3FilterCutoffAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region3FilterResonanceAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region3AttackAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region3DecayAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region3PanAttachment;

    // Region 4 parameters (8)
    std::unique_ptr<juce::WebSliderParameterAttachment> region4StartAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region4EndAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region4PitchAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region4FilterCutoffAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region4FilterResonanceAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region4AttackAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region4DecayAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region4PanAttachment;

    // Region 5 parameters (8)
    std::unique_ptr<juce::WebSliderParameterAttachment> region5StartAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region5EndAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region5PitchAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region5FilterCutoffAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region5FilterResonanceAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region5AttackAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region5DecayAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> region5PanAttachment;

    // Sequencer parameters (11)
    std::unique_ptr<juce::WebComboBoxParameterAttachment> step1RegionAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> step2RegionAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> step3RegionAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> step4RegionAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> step5RegionAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> step6RegionAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> step7RegionAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> step8RegionAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> playbackModeAttachment;
    std::unique_ptr<juce::WebToggleButtonParameterAttachment> loopModeAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> crossfadeTimeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MuSamAudioProcessorEditor)
};
