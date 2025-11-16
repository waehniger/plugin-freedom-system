#include "PluginEditor.h"

//==============================================================================
// Constructor
//==============================================================================

MuSamAudioProcessorEditor::MuSamAudioProcessorEditor(MuSamAudioProcessor& p)
    : AudioProcessorEditor(p), audioProcessor(p)
{
    // ========================================================================
    // STEP 1: CREATE RELAYS (before WebView!)
    // ========================================================================

    // Global parameters (2)
    speedRelay = std::make_unique<juce::WebSliderRelay>("speed");
    volumeRelay = std::make_unique<juce::WebSliderRelay>("volume");

    // Region 1 parameters (8)
    region1StartRelay = std::make_unique<juce::WebSliderRelay>("region_1_start");
    region1EndRelay = std::make_unique<juce::WebSliderRelay>("region_1_end");
    region1PitchRelay = std::make_unique<juce::WebSliderRelay>("region_1_pitch");
    region1FilterCutoffRelay = std::make_unique<juce::WebSliderRelay>("region_1_filter_cutoff");
    region1FilterResonanceRelay = std::make_unique<juce::WebSliderRelay>("region_1_filter_resonance");
    region1AttackRelay = std::make_unique<juce::WebSliderRelay>("region_1_attack");
    region1DecayRelay = std::make_unique<juce::WebSliderRelay>("region_1_decay");
    region1PanRelay = std::make_unique<juce::WebSliderRelay>("region_1_pan");

    // Region 2 parameters (8)
    region2StartRelay = std::make_unique<juce::WebSliderRelay>("region_2_start");
    region2EndRelay = std::make_unique<juce::WebSliderRelay>("region_2_end");
    region2PitchRelay = std::make_unique<juce::WebSliderRelay>("region_2_pitch");
    region2FilterCutoffRelay = std::make_unique<juce::WebSliderRelay>("region_2_filter_cutoff");
    region2FilterResonanceRelay = std::make_unique<juce::WebSliderRelay>("region_2_filter_resonance");
    region2AttackRelay = std::make_unique<juce::WebSliderRelay>("region_2_attack");
    region2DecayRelay = std::make_unique<juce::WebSliderRelay>("region_2_decay");
    region2PanRelay = std::make_unique<juce::WebSliderRelay>("region_2_pan");

    // Region 3 parameters (8)
    region3StartRelay = std::make_unique<juce::WebSliderRelay>("region_3_start");
    region3EndRelay = std::make_unique<juce::WebSliderRelay>("region_3_end");
    region3PitchRelay = std::make_unique<juce::WebSliderRelay>("region_3_pitch");
    region3FilterCutoffRelay = std::make_unique<juce::WebSliderRelay>("region_3_filter_cutoff");
    region3FilterResonanceRelay = std::make_unique<juce::WebSliderRelay>("region_3_filter_resonance");
    region3AttackRelay = std::make_unique<juce::WebSliderRelay>("region_3_attack");
    region3DecayRelay = std::make_unique<juce::WebSliderRelay>("region_3_decay");
    region3PanRelay = std::make_unique<juce::WebSliderRelay>("region_3_pan");

    // Region 4 parameters (8)
    region4StartRelay = std::make_unique<juce::WebSliderRelay>("region_4_start");
    region4EndRelay = std::make_unique<juce::WebSliderRelay>("region_4_end");
    region4PitchRelay = std::make_unique<juce::WebSliderRelay>("region_4_pitch");
    region4FilterCutoffRelay = std::make_unique<juce::WebSliderRelay>("region_4_filter_cutoff");
    region4FilterResonanceRelay = std::make_unique<juce::WebSliderRelay>("region_4_filter_resonance");
    region4AttackRelay = std::make_unique<juce::WebSliderRelay>("region_4_attack");
    region4DecayRelay = std::make_unique<juce::WebSliderRelay>("region_4_decay");
    region4PanRelay = std::make_unique<juce::WebSliderRelay>("region_4_pan");

    // Region 5 parameters (8)
    region5StartRelay = std::make_unique<juce::WebSliderRelay>("region_5_start");
    region5EndRelay = std::make_unique<juce::WebSliderRelay>("region_5_end");
    region5PitchRelay = std::make_unique<juce::WebSliderRelay>("region_5_pitch");
    region5FilterCutoffRelay = std::make_unique<juce::WebSliderRelay>("region_5_filter_cutoff");
    region5FilterResonanceRelay = std::make_unique<juce::WebSliderRelay>("region_5_filter_resonance");
    region5AttackRelay = std::make_unique<juce::WebSliderRelay>("region_5_attack");
    region5DecayRelay = std::make_unique<juce::WebSliderRelay>("region_5_decay");
    region5PanRelay = std::make_unique<juce::WebSliderRelay>("region_5_pan");

    // Sequencer parameters (11)
    step1RegionRelay = std::make_unique<juce::WebComboBoxRelay>("step_1_region");
    step2RegionRelay = std::make_unique<juce::WebComboBoxRelay>("step_2_region");
    step3RegionRelay = std::make_unique<juce::WebComboBoxRelay>("step_3_region");
    step4RegionRelay = std::make_unique<juce::WebComboBoxRelay>("step_4_region");
    step5RegionRelay = std::make_unique<juce::WebComboBoxRelay>("step_5_region");
    step6RegionRelay = std::make_unique<juce::WebComboBoxRelay>("step_6_region");
    step7RegionRelay = std::make_unique<juce::WebComboBoxRelay>("step_7_region");
    step8RegionRelay = std::make_unique<juce::WebComboBoxRelay>("step_8_region");
    playbackModeRelay = std::make_unique<juce::WebComboBoxRelay>("playback_mode");
    loopModeRelay = std::make_unique<juce::WebToggleButtonRelay>("loop_mode");
    crossfadeTimeRelay = std::make_unique<juce::WebSliderRelay>("crossfade_time");

    // ========================================================================
    // STEP 2: CREATE WEBVIEW (with relay options)
    // ========================================================================

    webView = std::make_unique<juce::WebBrowserComponent>(
        juce::WebBrowserComponent::Options{}
            .withNativeIntegrationEnabled()
            .withResourceProvider([this](const auto& url) { return getResource(url); })
            .withKeepPageLoadedWhenBrowserIsHidden()

            // Global parameters
            .withOptionsFrom(*speedRelay)
            .withOptionsFrom(*volumeRelay)

            // Region 1 parameters
            .withOptionsFrom(*region1StartRelay)
            .withOptionsFrom(*region1EndRelay)
            .withOptionsFrom(*region1PitchRelay)
            .withOptionsFrom(*region1FilterCutoffRelay)
            .withOptionsFrom(*region1FilterResonanceRelay)
            .withOptionsFrom(*region1AttackRelay)
            .withOptionsFrom(*region1DecayRelay)
            .withOptionsFrom(*region1PanRelay)

            // Region 2 parameters
            .withOptionsFrom(*region2StartRelay)
            .withOptionsFrom(*region2EndRelay)
            .withOptionsFrom(*region2PitchRelay)
            .withOptionsFrom(*region2FilterCutoffRelay)
            .withOptionsFrom(*region2FilterResonanceRelay)
            .withOptionsFrom(*region2AttackRelay)
            .withOptionsFrom(*region2DecayRelay)
            .withOptionsFrom(*region2PanRelay)

            // Region 3 parameters
            .withOptionsFrom(*region3StartRelay)
            .withOptionsFrom(*region3EndRelay)
            .withOptionsFrom(*region3PitchRelay)
            .withOptionsFrom(*region3FilterCutoffRelay)
            .withOptionsFrom(*region3FilterResonanceRelay)
            .withOptionsFrom(*region3AttackRelay)
            .withOptionsFrom(*region3DecayRelay)
            .withOptionsFrom(*region3PanRelay)

            // Region 4 parameters
            .withOptionsFrom(*region4StartRelay)
            .withOptionsFrom(*region4EndRelay)
            .withOptionsFrom(*region4PitchRelay)
            .withOptionsFrom(*region4FilterCutoffRelay)
            .withOptionsFrom(*region4FilterResonanceRelay)
            .withOptionsFrom(*region4AttackRelay)
            .withOptionsFrom(*region4DecayRelay)
            .withOptionsFrom(*region4PanRelay)

            // Region 5 parameters
            .withOptionsFrom(*region5StartRelay)
            .withOptionsFrom(*region5EndRelay)
            .withOptionsFrom(*region5PitchRelay)
            .withOptionsFrom(*region5FilterCutoffRelay)
            .withOptionsFrom(*region5FilterResonanceRelay)
            .withOptionsFrom(*region5AttackRelay)
            .withOptionsFrom(*region5DecayRelay)
            .withOptionsFrom(*region5PanRelay)

            // Sequencer parameters
            .withOptionsFrom(*step1RegionRelay)
            .withOptionsFrom(*step2RegionRelay)
            .withOptionsFrom(*step3RegionRelay)
            .withOptionsFrom(*step4RegionRelay)
            .withOptionsFrom(*step5RegionRelay)
            .withOptionsFrom(*step6RegionRelay)
            .withOptionsFrom(*step7RegionRelay)
            .withOptionsFrom(*step8RegionRelay)
            .withOptionsFrom(*playbackModeRelay)
            .withOptionsFrom(*loopModeRelay)
            .withOptionsFrom(*crossfadeTimeRelay)
    );

    // ========================================================================
    // STEP 3: CREATE PARAMETER ATTACHMENTS (after WebView!)
    // ========================================================================

    // Global parameters (2)
    speedAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("speed"), *speedRelay, nullptr);
    volumeAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("volume"), *volumeRelay, nullptr);

    // Region 1 parameters (8)
    region1StartAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_1_start"), *region1StartRelay, nullptr);
    region1EndAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_1_end"), *region1EndRelay, nullptr);
    region1PitchAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_1_pitch"), *region1PitchRelay, nullptr);
    region1FilterCutoffAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_1_filter_cutoff"), *region1FilterCutoffRelay, nullptr);
    region1FilterResonanceAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_1_filter_resonance"), *region1FilterResonanceRelay, nullptr);
    region1AttackAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_1_attack"), *region1AttackRelay, nullptr);
    region1DecayAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_1_decay"), *region1DecayRelay, nullptr);
    region1PanAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_1_pan"), *region1PanRelay, nullptr);

    // Region 2 parameters (8)
    region2StartAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_2_start"), *region2StartRelay, nullptr);
    region2EndAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_2_end"), *region2EndRelay, nullptr);
    region2PitchAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_2_pitch"), *region2PitchRelay, nullptr);
    region2FilterCutoffAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_2_filter_cutoff"), *region2FilterCutoffRelay, nullptr);
    region2FilterResonanceAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_2_filter_resonance"), *region2FilterResonanceRelay, nullptr);
    region2AttackAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_2_attack"), *region2AttackRelay, nullptr);
    region2DecayAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_2_decay"), *region2DecayRelay, nullptr);
    region2PanAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_2_pan"), *region2PanRelay, nullptr);

    // Region 3 parameters (8)
    region3StartAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_3_start"), *region3StartRelay, nullptr);
    region3EndAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_3_end"), *region3EndRelay, nullptr);
    region3PitchAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_3_pitch"), *region3PitchRelay, nullptr);
    region3FilterCutoffAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_3_filter_cutoff"), *region3FilterCutoffRelay, nullptr);
    region3FilterResonanceAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_3_filter_resonance"), *region3FilterResonanceRelay, nullptr);
    region3AttackAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_3_attack"), *region3AttackRelay, nullptr);
    region3DecayAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_3_decay"), *region3DecayRelay, nullptr);
    region3PanAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_3_pan"), *region3PanRelay, nullptr);

    // Region 4 parameters (8)
    region4StartAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_4_start"), *region4StartRelay, nullptr);
    region4EndAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_4_end"), *region4EndRelay, nullptr);
    region4PitchAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_4_pitch"), *region4PitchRelay, nullptr);
    region4FilterCutoffAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_4_filter_cutoff"), *region4FilterCutoffRelay, nullptr);
    region4FilterResonanceAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_4_filter_resonance"), *region4FilterResonanceRelay, nullptr);
    region4AttackAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_4_attack"), *region4AttackRelay, nullptr);
    region4DecayAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_4_decay"), *region4DecayRelay, nullptr);
    region4PanAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_4_pan"), *region4PanRelay, nullptr);

    // Region 5 parameters (8)
    region5StartAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_5_start"), *region5StartRelay, nullptr);
    region5EndAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_5_end"), *region5EndRelay, nullptr);
    region5PitchAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_5_pitch"), *region5PitchRelay, nullptr);
    region5FilterCutoffAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_5_filter_cutoff"), *region5FilterCutoffRelay, nullptr);
    region5FilterResonanceAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_5_filter_resonance"), *region5FilterResonanceRelay, nullptr);
    region5AttackAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_5_attack"), *region5AttackRelay, nullptr);
    region5DecayAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_5_decay"), *region5DecayRelay, nullptr);
    region5PanAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_5_pan"), *region5PanRelay, nullptr);

    // Sequencer parameters (11)
    step1RegionAttachment = std::make_unique<juce::WebComboBoxParameterAttachment>(
        *audioProcessor.apvts.getParameter("step_1_region"), *step1RegionRelay, nullptr);
    step2RegionAttachment = std::make_unique<juce::WebComboBoxParameterAttachment>(
        *audioProcessor.apvts.getParameter("step_2_region"), *step2RegionRelay, nullptr);
    step3RegionAttachment = std::make_unique<juce::WebComboBoxParameterAttachment>(
        *audioProcessor.apvts.getParameter("step_3_region"), *step3RegionRelay, nullptr);
    step4RegionAttachment = std::make_unique<juce::WebComboBoxParameterAttachment>(
        *audioProcessor.apvts.getParameter("step_4_region"), *step4RegionRelay, nullptr);
    step5RegionAttachment = std::make_unique<juce::WebComboBoxParameterAttachment>(
        *audioProcessor.apvts.getParameter("step_5_region"), *step5RegionRelay, nullptr);
    step6RegionAttachment = std::make_unique<juce::WebComboBoxParameterAttachment>(
        *audioProcessor.apvts.getParameter("step_6_region"), *step6RegionRelay, nullptr);
    step7RegionAttachment = std::make_unique<juce::WebComboBoxParameterAttachment>(
        *audioProcessor.apvts.getParameter("step_7_region"), *step7RegionRelay, nullptr);
    step8RegionAttachment = std::make_unique<juce::WebComboBoxParameterAttachment>(
        *audioProcessor.apvts.getParameter("step_8_region"), *step8RegionRelay, nullptr);
    playbackModeAttachment = std::make_unique<juce::WebComboBoxParameterAttachment>(
        *audioProcessor.apvts.getParameter("playback_mode"), *playbackModeRelay, nullptr);
    loopModeAttachment = std::make_unique<juce::WebToggleButtonParameterAttachment>(
        *audioProcessor.apvts.getParameter("loop_mode"), *loopModeRelay, nullptr);
    crossfadeTimeAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("crossfade_time"), *crossfadeTimeRelay, nullptr);

    // ========================================================================
    // WEBVIEW SETUP
    // ========================================================================

    webView->goToURL(juce::WebBrowserComponent::getResourceProviderRoot());
    addAndMakeVisible(*webView);

    // Window size (from YAML: 900x600)
    setSize(900, 600);
}

//==============================================================================
// Destructor
//==============================================================================

MuSamAudioProcessorEditor::~MuSamAudioProcessorEditor()
{
    // Automatic destruction in reverse order:
    // 1. Attachments → 2. WebView → 3. Relays
}

//==============================================================================
// AudioProcessorEditor Overrides
//==============================================================================

void MuSamAudioProcessorEditor::paint(juce::Graphics& g)
{
    // WebView fills the entire editor
}

void MuSamAudioProcessorEditor::resized()
{
    webView->setBounds(getLocalBounds());
}

//==============================================================================
// Resource Provider
//==============================================================================

std::optional<juce::WebBrowserComponent::Resource> MuSamAudioProcessorEditor::getResource(
    const juce::String& url
)
{
    auto makeVector = [](const char* data, int size) {
        return std::vector<std::byte>(
            reinterpret_cast<const std::byte*>(data),
            reinterpret_cast<const std::byte*>(data) + size
        );
    };

    // Root URL (index.html)
    if (url == "/" || url == "/index.html") {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::index_html, BinaryData::index_htmlSize),
            juce::String("text/html")
        };
    }

    // JUCE frontend library
    if (url == "/js/juce/index.js") {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::index_js, BinaryData::index_jsSize),
            juce::String("application/javascript")
        };
    }

    // Check native interop (required for WebView)
    if (url == "/js/juce/check_native_interop.js") {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::check_native_interop_js,
                      BinaryData::check_native_interop_jsSize),
            juce::String("application/javascript")
        };
    }

    // 404 - Resource not found
    return std::nullopt;
}
