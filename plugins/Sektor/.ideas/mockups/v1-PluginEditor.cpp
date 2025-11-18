#include "PluginEditor.h"

//==============================================================================
// Constructor - CRITICAL: Initialize in correct order
//==============================================================================

SektorAudioProcessorEditor::SektorAudioProcessorEditor(SektorAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // ========================================================================
    // INITIALIZATION SEQUENCE (CRITICAL ORDER)
    // ========================================================================
    //
    // 1. Create relays FIRST (before WebView construction)
    // 2. Create WebView with relay options
    // 3. Create parameter attachments LAST (after WebView construction)
    //
    // This matches the member declaration order and ensures safe destruction.
    // ========================================================================

    // ------------------------------------------------------------------------
    // STEP 1: CREATE RELAYS (before WebView!)
    // ------------------------------------------------------------------------
    //
    // 7 parameters total:
    // - grain_size, density, pitch_shift, spacing (continuous sliders)
    // - region_start, region_end (region markers)
    // - polyphony_mode (boolean toggle)
    //
    grainSizeRelay = std::make_unique<juce::WebSliderRelay>("grain_size");
    densityRelay = std::make_unique<juce::WebSliderRelay>("density");
    pitchShiftRelay = std::make_unique<juce::WebSliderRelay>("pitch_shift");
    spacingRelay = std::make_unique<juce::WebSliderRelay>("spacing");
    regionStartRelay = std::make_unique<juce::WebSliderRelay>("region_start");
    regionEndRelay = std::make_unique<juce::WebSliderRelay>("region_end");
    polyphonyModeRelay = std::make_unique<juce::WebToggleButtonRelay>("polyphony_mode");

    // ------------------------------------------------------------------------
    // STEP 2: CREATE WEBVIEW (with relay options)
    // ------------------------------------------------------------------------
    //
    // WebView creation with all necessary options:
    // - withNativeIntegrationEnabled() - REQUIRED for JUCE parameter binding
    // - withResourceProvider() - REQUIRED for JUCE 8 (serves embedded files)
    // - withOptionsFrom(*relay) - REQUIRED for each parameter relay
    // - withKeepPageLoadedWhenBrowserIsHidden() - OPTIONAL (FL Studio fix)
    //
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

            // REQUIRED: Register all 7 parameter relays with WebView
            .withOptionsFrom(*grainSizeRelay)
            .withOptionsFrom(*densityRelay)
            .withOptionsFrom(*pitchShiftRelay)
            .withOptionsFrom(*spacingRelay)
            .withOptionsFrom(*regionStartRelay)
            .withOptionsFrom(*regionEndRelay)
            .withOptionsFrom(*polyphonyModeRelay)
    );

    // ------------------------------------------------------------------------
    // STEP 3: CREATE PARAMETER ATTACHMENTS (after WebView!)
    // ------------------------------------------------------------------------
    //
    // Attachments synchronize APVTS parameters with relay state.
    // Constructor: (parameter, relay, undoManager)
    //
    // CRITICAL: Third parameter (nullptr) is REQUIRED in JUCE 8.
    // Missing it causes silent failure - UI displays but doesn't respond.
    //
    grainSizeAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("grain_size"),
        *grainSizeRelay,
        nullptr  // REQUIRED in JUCE 8
    );

    densityAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("density"),
        *densityRelay,
        nullptr
    );

    pitchShiftAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("pitch_shift"),
        *pitchShiftRelay,
        nullptr
    );

    spacingAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("spacing"),
        *spacingRelay,
        nullptr
    );

    regionStartAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_start"),
        *regionStartRelay,
        nullptr
    );

    regionEndAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("region_end"),
        *regionEndRelay,
        nullptr
    );

    polyphonyModeAttachment = std::make_unique<juce::WebToggleButtonParameterAttachment>(
        *audioProcessor.apvts.getParameter("polyphony_mode"),
        *polyphonyModeRelay,
        nullptr
    );

    // ------------------------------------------------------------------------
    // WEBVIEW SETUP
    // ------------------------------------------------------------------------

    // Navigate to root (loads index.html via resource provider)
    webView->goToURL(juce::WebBrowserComponent::getResourceProviderRoot());

    // Make WebView visible
    addAndMakeVisible(*webView);

    // ------------------------------------------------------------------------
    // TIMER SETUP (for real-time updates)
    // ------------------------------------------------------------------------
    //
    // 60ms = ~16 FPS (sufficient for CPU monitoring)
    //
    startTimer(60);

    // ------------------------------------------------------------------------
    // WINDOW SIZE (from v1-ui.yaml)
    // ------------------------------------------------------------------------
    //
    // Fixed 900x600 px window (not resizable)
    //
    setSize(900, 600);
    setResizable(false, false);
}

//==============================================================================
// Destructor
//==============================================================================

SektorAudioProcessorEditor::~SektorAudioProcessorEditor()
{
    // Members are automatically destroyed in reverse order of declaration:
    // 1. Attachments destroyed first (stop calling evaluateJavascript)
    // 2. WebView destroyed second (safe, attachments are gone)
    // 3. Relays destroyed last (safe, nothing using them)
    //
    // No manual cleanup needed if member order is correct!

    stopTimer();
}

//==============================================================================
// AudioProcessorEditor Overrides
//==============================================================================

void SektorAudioProcessorEditor::paint(juce::Graphics& g)
{
    // WebView fills the entire editor, so no custom painting needed
}

void SektorAudioProcessorEditor::resized()
{
    // Make WebView fill the entire editor bounds
    webView->setBounds(getLocalBounds());
}

//==============================================================================
// Timer Callback (Real-time Updates)
//==============================================================================

void SektorAudioProcessorEditor::timerCallback()
{
    // Example: Update CPU usage from audio thread data
    // CRITICAL: Use std::atomic in PluginProcessor for thread-safe communication
    //
    // float cpuUsage = audioProcessor.getCPUUsage();  // Atomic read
    //
    // webView->emitEventIfBrowserIsVisible("cpuUpdate", {
    //     { "usage", cpuUsage }
    // });

    // Example: Update waveform when sample changes
    // if (audioProcessor.hasSampleChanged()) {
    //     // Send waveform data to JavaScript
    //     const auto& waveformData = audioProcessor.getWaveformData();
    //     webView->emitEventIfBrowserIsVisible("waveformUpdate", {
    //         { "data", waveformData },
    //         { "sampleRate", audioProcessor.getSampleRate() }
    //     });
    // }
}

//==============================================================================
// Resource Provider (JUCE 8 Required Pattern)
//==============================================================================

std::optional<juce::WebBrowserComponent::Resource> SektorAudioProcessorEditor::getResource(
    const juce::String& url
)
{
    // ========================================================================
    // RESOURCE PROVIDER IMPLEMENTATION
    // ========================================================================
    //
    // Maps URLs to embedded binary data (from juce_add_binary_data).
    //
    // CRITICAL: Use EXPLICIT URL mapping (not generic loops).
    // Generic loops break when BinaryData flattens paths.
    //
    // File path → BinaryData symbol:
    // - Source/ui/public/index.html       → BinaryData::index_html
    // - Source/ui/public/js/juce/index.js → BinaryData::index_js
    //
    // Pattern: juce_add_binary_data converts slashes and dots to underscores.
    // ========================================================================

    // Helper to convert raw data to std::vector<std::byte>
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

    // JUCE frontend library (REQUIRED for parameter binding)
    if (url == "/js/juce/index.js") {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::index_js, BinaryData::index_jsSize),
            juce::String("application/javascript")  // Correct MIME type
        };
    }

    // JUCE native interop check (REQUIRED for WebView initialization)
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
