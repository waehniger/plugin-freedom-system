#include "PluginEditor.h"
#include "BinaryData.h"

//==============================================================================
// Constructor - CRITICAL: Initialize in correct order
//==============================================================================

MuSamAudioProcessorEditor::MuSamAudioProcessorEditor(MuSamAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    // ========================================================================
    // INITIALIZATION SEQUENCE (CRITICAL ORDER)
    // ========================================================================
    //
    // 1. Create relays FIRST (before WebView construction)
    // 2. Create WebView with relay options
    // 3. Create parameter attachments LAST (after WebView construction)
    // ========================================================================

    // ------------------------------------------------------------------------
    // STEP 1: CREATE RELAYS (before WebView!)
    // Phase 5.2: All 53 parameters
    // ------------------------------------------------------------------------
    
    // Global parameters (2)
    speedRelay = std::make_unique<juce::WebSliderRelay>("speed");
    volumeRelay = std::make_unique<juce::WebSliderRelay>("volume");
    
    // Region parameters (5 regions × 8 parameters = 40)
    // Region 1
    region_1_startRelay = std::make_unique<juce::WebSliderRelay>("region_1_start");
    region_1_endRelay = std::make_unique<juce::WebSliderRelay>("region_1_end");
    region_1_pitchRelay = std::make_unique<juce::WebSliderRelay>("region_1_pitch");
    region_1_filter_cutoffRelay = std::make_unique<juce::WebSliderRelay>("region_1_filter_cutoff");
    region_1_filter_resonanceRelay = std::make_unique<juce::WebSliderRelay>("region_1_filter_resonance");
    region_1_attackRelay = std::make_unique<juce::WebSliderRelay>("region_1_attack");
    region_1_decayRelay = std::make_unique<juce::WebSliderRelay>("region_1_decay");
    region_1_panRelay = std::make_unique<juce::WebSliderRelay>("region_1_pan");
    
    // Region 2
    region_2_startRelay = std::make_unique<juce::WebSliderRelay>("region_2_start");
    region_2_endRelay = std::make_unique<juce::WebSliderRelay>("region_2_end");
    region_2_pitchRelay = std::make_unique<juce::WebSliderRelay>("region_2_pitch");
    region_2_filter_cutoffRelay = std::make_unique<juce::WebSliderRelay>("region_2_filter_cutoff");
    region_2_filter_resonanceRelay = std::make_unique<juce::WebSliderRelay>("region_2_filter_resonance");
    region_2_attackRelay = std::make_unique<juce::WebSliderRelay>("region_2_attack");
    region_2_decayRelay = std::make_unique<juce::WebSliderRelay>("region_2_decay");
    region_2_panRelay = std::make_unique<juce::WebSliderRelay>("region_2_pan");
    
    // Region 3
    region_3_startRelay = std::make_unique<juce::WebSliderRelay>("region_3_start");
    region_3_endRelay = std::make_unique<juce::WebSliderRelay>("region_3_end");
    region_3_pitchRelay = std::make_unique<juce::WebSliderRelay>("region_3_pitch");
    region_3_filter_cutoffRelay = std::make_unique<juce::WebSliderRelay>("region_3_filter_cutoff");
    region_3_filter_resonanceRelay = std::make_unique<juce::WebSliderRelay>("region_3_filter_resonance");
    region_3_attackRelay = std::make_unique<juce::WebSliderRelay>("region_3_attack");
    region_3_decayRelay = std::make_unique<juce::WebSliderRelay>("region_3_decay");
    region_3_panRelay = std::make_unique<juce::WebSliderRelay>("region_3_pan");
    
    // Region 4
    region_4_startRelay = std::make_unique<juce::WebSliderRelay>("region_4_start");
    region_4_endRelay = std::make_unique<juce::WebSliderRelay>("region_4_end");
    region_4_pitchRelay = std::make_unique<juce::WebSliderRelay>("region_4_pitch");
    region_4_filter_cutoffRelay = std::make_unique<juce::WebSliderRelay>("region_4_filter_cutoff");
    region_4_filter_resonanceRelay = std::make_unique<juce::WebSliderRelay>("region_4_filter_resonance");
    region_4_attackRelay = std::make_unique<juce::WebSliderRelay>("region_4_attack");
    region_4_decayRelay = std::make_unique<juce::WebSliderRelay>("region_4_decay");
    region_4_panRelay = std::make_unique<juce::WebSliderRelay>("region_4_pan");
    
    // Region 5
    region_5_startRelay = std::make_unique<juce::WebSliderRelay>("region_5_start");
    region_5_endRelay = std::make_unique<juce::WebSliderRelay>("region_5_end");
    region_5_pitchRelay = std::make_unique<juce::WebSliderRelay>("region_5_pitch");
    region_5_filter_cutoffRelay = std::make_unique<juce::WebSliderRelay>("region_5_filter_cutoff");
    region_5_filter_resonanceRelay = std::make_unique<juce::WebSliderRelay>("region_5_filter_resonance");
    region_5_attackRelay = std::make_unique<juce::WebSliderRelay>("region_5_attack");
    region_5_decayRelay = std::make_unique<juce::WebSliderRelay>("region_5_decay");
    region_5_panRelay = std::make_unique<juce::WebSliderRelay>("region_5_pan");
    
    // Sequencer parameters (11)
    step_1_regionRelay = std::make_unique<juce::WebComboBoxRelay>("step_1_region");
    step_2_regionRelay = std::make_unique<juce::WebComboBoxRelay>("step_2_region");
    step_3_regionRelay = std::make_unique<juce::WebComboBoxRelay>("step_3_region");
    step_4_regionRelay = std::make_unique<juce::WebComboBoxRelay>("step_4_region");
    step_5_regionRelay = std::make_unique<juce::WebComboBoxRelay>("step_5_region");
    step_6_regionRelay = std::make_unique<juce::WebComboBoxRelay>("step_6_region");
    step_7_regionRelay = std::make_unique<juce::WebComboBoxRelay>("step_7_region");
    step_8_regionRelay = std::make_unique<juce::WebComboBoxRelay>("step_8_region");
    playback_modeRelay = std::make_unique<juce::WebComboBoxRelay>("playback_mode");
    loop_modeRelay = std::make_unique<juce::WebToggleButtonRelay>("loop_mode");
    crossfade_timeRelay = std::make_unique<juce::WebSliderRelay>("crossfade_time");

    // ------------------------------------------------------------------------
    // STEP 2: CREATE WEBVIEW (with relay options)
    // ------------------------------------------------------------------------
    webView = std::make_unique<juce::WebBrowserComponent>(
        juce::WebBrowserComponent::Options{}
            // REQUIRED: Enable JUCE frontend library
            .withNativeIntegrationEnabled()

            // REQUIRED: Resource provider for embedded files
            .withResourceProvider([this](const auto& url) {
                return getResource(url);
            })

            // OPTIONAL: FL Studio fix (prevents blank screen on focus loss)
            .withKeepPageLoadedWhenBrowserIsHidden()

            // REQUIRED: Register all relays with WebView
            // Global parameters (2)
            .withOptionsFrom(*speedRelay)
            .withOptionsFrom(*volumeRelay)
            
            // Region 1 (8)
            .withOptionsFrom(*region_1_startRelay)
            .withOptionsFrom(*region_1_endRelay)
            .withOptionsFrom(*region_1_pitchRelay)
            .withOptionsFrom(*region_1_filter_cutoffRelay)
            .withOptionsFrom(*region_1_filter_resonanceRelay)
            .withOptionsFrom(*region_1_attackRelay)
            .withOptionsFrom(*region_1_decayRelay)
            .withOptionsFrom(*region_1_panRelay)
            
            // Region 2 (8)
            .withOptionsFrom(*region_2_startRelay)
            .withOptionsFrom(*region_2_endRelay)
            .withOptionsFrom(*region_2_pitchRelay)
            .withOptionsFrom(*region_2_filter_cutoffRelay)
            .withOptionsFrom(*region_2_filter_resonanceRelay)
            .withOptionsFrom(*region_2_attackRelay)
            .withOptionsFrom(*region_2_decayRelay)
            .withOptionsFrom(*region_2_panRelay)
            
            // Region 3 (8)
            .withOptionsFrom(*region_3_startRelay)
            .withOptionsFrom(*region_3_endRelay)
            .withOptionsFrom(*region_3_pitchRelay)
            .withOptionsFrom(*region_3_filter_cutoffRelay)
            .withOptionsFrom(*region_3_filter_resonanceRelay)
            .withOptionsFrom(*region_3_attackRelay)
            .withOptionsFrom(*region_3_decayRelay)
            .withOptionsFrom(*region_3_panRelay)
            
            // Region 4 (8)
            .withOptionsFrom(*region_4_startRelay)
            .withOptionsFrom(*region_4_endRelay)
            .withOptionsFrom(*region_4_pitchRelay)
            .withOptionsFrom(*region_4_filter_cutoffRelay)
            .withOptionsFrom(*region_4_filter_resonanceRelay)
            .withOptionsFrom(*region_4_attackRelay)
            .withOptionsFrom(*region_4_decayRelay)
            .withOptionsFrom(*region_4_panRelay)
            
            // Region 5 (8)
            .withOptionsFrom(*region_5_startRelay)
            .withOptionsFrom(*region_5_endRelay)
            .withOptionsFrom(*region_5_pitchRelay)
            .withOptionsFrom(*region_5_filter_cutoffRelay)
            .withOptionsFrom(*region_5_filter_resonanceRelay)
            .withOptionsFrom(*region_5_attackRelay)
            .withOptionsFrom(*region_5_decayRelay)
            .withOptionsFrom(*region_5_panRelay)
            
            // Sequencer parameters (11)
            .withOptionsFrom(*step_1_regionRelay)
            .withOptionsFrom(*step_2_regionRelay)
            .withOptionsFrom(*step_3_regionRelay)
            .withOptionsFrom(*step_4_regionRelay)
            .withOptionsFrom(*step_5_regionRelay)
            .withOptionsFrom(*step_6_regionRelay)
            .withOptionsFrom(*step_7_regionRelay)
            .withOptionsFrom(*step_8_regionRelay)
            .withOptionsFrom(*playback_modeRelay)
            .withOptionsFrom(*loop_modeRelay)
            .withOptionsFrom(*crossfade_timeRelay)
    );

    // ------------------------------------------------------------------------
    // STEP 3: CREATE PARAMETER ATTACHMENTS (after WebView!)
    // Phase 5.2: All 53 parameters
    // ------------------------------------------------------------------------
    
    // Global parameters (2)
    speedAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("speed"),
        *speedRelay,
        nullptr  // No undo manager
    );

    volumeAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("volume"),
        *volumeRelay,
        nullptr
    );
    
    // Region parameters (40)
    // Region 1
    region_1_startAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_1_start"), *region_1_startRelay, nullptr);
    region_1_endAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_1_end"), *region_1_endRelay, nullptr);
    region_1_pitchAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_1_pitch"), *region_1_pitchRelay, nullptr);
    region_1_filter_cutoffAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_1_filter_cutoff"), *region_1_filter_cutoffRelay, nullptr);
    region_1_filter_resonanceAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_1_filter_resonance"), *region_1_filter_resonanceRelay, nullptr);
    region_1_attackAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_1_attack"), *region_1_attackRelay, nullptr);
    region_1_decayAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_1_decay"), *region_1_decayRelay, nullptr);
    region_1_panAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_1_pan"), *region_1_panRelay, nullptr);
    
    // Region 2
    region_2_startAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_2_start"), *region_2_startRelay, nullptr);
    region_2_endAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_2_end"), *region_2_endRelay, nullptr);
    region_2_pitchAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_2_pitch"), *region_2_pitchRelay, nullptr);
    region_2_filter_cutoffAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_2_filter_cutoff"), *region_2_filter_cutoffRelay, nullptr);
    region_2_filter_resonanceAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_2_filter_resonance"), *region_2_filter_resonanceRelay, nullptr);
    region_2_attackAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_2_attack"), *region_2_attackRelay, nullptr);
    region_2_decayAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_2_decay"), *region_2_decayRelay, nullptr);
    region_2_panAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_2_pan"), *region_2_panRelay, nullptr);
    
    // Region 3
    region_3_startAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_3_start"), *region_3_startRelay, nullptr);
    region_3_endAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_3_end"), *region_3_endRelay, nullptr);
    region_3_pitchAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_3_pitch"), *region_3_pitchRelay, nullptr);
    region_3_filter_cutoffAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_3_filter_cutoff"), *region_3_filter_cutoffRelay, nullptr);
    region_3_filter_resonanceAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_3_filter_resonance"), *region_3_filter_resonanceRelay, nullptr);
    region_3_attackAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_3_attack"), *region_3_attackRelay, nullptr);
    region_3_decayAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_3_decay"), *region_3_decayRelay, nullptr);
    region_3_panAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_3_pan"), *region_3_panRelay, nullptr);
    
    // Region 4
    region_4_startAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_4_start"), *region_4_startRelay, nullptr);
    region_4_endAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_4_end"), *region_4_endRelay, nullptr);
    region_4_pitchAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_4_pitch"), *region_4_pitchRelay, nullptr);
    region_4_filter_cutoffAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_4_filter_cutoff"), *region_4_filter_cutoffRelay, nullptr);
    region_4_filter_resonanceAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_4_filter_resonance"), *region_4_filter_resonanceRelay, nullptr);
    region_4_attackAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_4_attack"), *region_4_attackRelay, nullptr);
    region_4_decayAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_4_decay"), *region_4_decayRelay, nullptr);
    region_4_panAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_4_pan"), *region_4_panRelay, nullptr);
    
    // Region 5
    region_5_startAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_5_start"), *region_5_startRelay, nullptr);
    region_5_endAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_5_end"), *region_5_endRelay, nullptr);
    region_5_pitchAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_5_pitch"), *region_5_pitchRelay, nullptr);
    region_5_filter_cutoffAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_5_filter_cutoff"), *region_5_filter_cutoffRelay, nullptr);
    region_5_filter_resonanceAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_5_filter_resonance"), *region_5_filter_resonanceRelay, nullptr);
    region_5_attackAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_5_attack"), *region_5_attackRelay, nullptr);
    region_5_decayAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_5_decay"), *region_5_decayRelay, nullptr);
    region_5_panAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("region_5_pan"), *region_5_panRelay, nullptr);
    
    // Sequencer parameters (11)
    step_1_regionAttachment = std::make_unique<juce::WebComboBoxParameterAttachment>(
        *processorRef.parameters.getParameter("step_1_region"), *step_1_regionRelay, nullptr);
    step_2_regionAttachment = std::make_unique<juce::WebComboBoxParameterAttachment>(
        *processorRef.parameters.getParameter("step_2_region"), *step_2_regionRelay, nullptr);
    step_3_regionAttachment = std::make_unique<juce::WebComboBoxParameterAttachment>(
        *processorRef.parameters.getParameter("step_3_region"), *step_3_regionRelay, nullptr);
    step_4_regionAttachment = std::make_unique<juce::WebComboBoxParameterAttachment>(
        *processorRef.parameters.getParameter("step_4_region"), *step_4_regionRelay, nullptr);
    step_5_regionAttachment = std::make_unique<juce::WebComboBoxParameterAttachment>(
        *processorRef.parameters.getParameter("step_5_region"), *step_5_regionRelay, nullptr);
    step_6_regionAttachment = std::make_unique<juce::WebComboBoxParameterAttachment>(
        *processorRef.parameters.getParameter("step_6_region"), *step_6_regionRelay, nullptr);
    step_7_regionAttachment = std::make_unique<juce::WebComboBoxParameterAttachment>(
        *processorRef.parameters.getParameter("step_7_region"), *step_7_regionRelay, nullptr);
    step_8_regionAttachment = std::make_unique<juce::WebComboBoxParameterAttachment>(
        *processorRef.parameters.getParameter("step_8_region"), *step_8_regionRelay, nullptr);
    playback_modeAttachment = std::make_unique<juce::WebComboBoxParameterAttachment>(
        *processorRef.parameters.getParameter("playback_mode"), *playback_modeRelay, nullptr);
    loop_modeAttachment = std::make_unique<juce::WebToggleButtonParameterAttachment>(
        *processorRef.parameters.getParameter("loop_mode"), *loop_modeRelay, nullptr);
    crossfade_timeAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("crossfade_time"), *crossfade_timeRelay, nullptr);

    // ------------------------------------------------------------------------
    // WEBVIEW SETUP
    // ------------------------------------------------------------------------

    // Navigate to root (loads index.html via resource provider)
    webView->goToURL(juce::WebBrowserComponent::getResourceProviderRoot());

    // Make WebView visible
    addAndMakeVisible(*webView);

    // ------------------------------------------------------------------------
    // WINDOW SIZE
    // ------------------------------------------------------------------------
    // Set plugin window size to match HTML mockup (900x600)
    setSize(900, 600);
    setResizable(false, false);
}

//==============================================================================
// Destructor
//==============================================================================

MuSamAudioProcessorEditor::~MuSamAudioProcessorEditor()
{
    // Members automatically destroyed in reverse order:
    // 1. All attachments (stop calling evaluateJavascript)
    // 2. webView (safe, attachments are gone)
    // 3. All relays (safe, nothing using them)
}

//==============================================================================
// AudioProcessorEditor Overrides
//==============================================================================

void MuSamAudioProcessorEditor::paint(juce::Graphics& g)
{
    // WebView fills the entire editor, no custom painting needed
    juce::ignoreUnused(g);
}

void MuSamAudioProcessorEditor::resized()
{
    // Make WebView fill the entire editor bounds
    webView->setBounds(getLocalBounds());
}

//==============================================================================
// Resource Provider (JUCE 8 Required Pattern)
//==============================================================================

std::optional<juce::WebBrowserComponent::Resource> MuSamAudioProcessorEditor::getResource(
    const juce::String& url
)
{
    // Helper lambda to convert raw binary data to vector<byte>
    auto makeVector = [](const char* data, int size) {
        return std::vector<std::byte>(
            reinterpret_cast<const std::byte*>(data),
            reinterpret_cast<const std::byte*>(data) + size
        );
    };

    // Handle root URL (redirect to index.html)
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
            juce::String("text/javascript")
        };
    }

    if (url == "/js/juce/check_native_interop.js") {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::check_native_interop_js, BinaryData::check_native_interop_jsSize),
            juce::String("text/javascript")
        };
    }

    // 404 for unknown resources
    return std::nullopt;
}
