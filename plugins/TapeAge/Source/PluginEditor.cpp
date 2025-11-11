#include "PluginEditor.h"
#include "BinaryData.h"

//==============================================================================
// Constructor - CRITICAL: Initialize in correct order
//==============================================================================

TapeAgeAudioProcessorEditor::TapeAgeAudioProcessorEditor(TapeAgeAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    // ========================================================================
    // INITIALIZATION SEQUENCE (CRITICAL ORDER)
    // ========================================================================
    // 1. Create relays FIRST (before WebView construction)
    // 2. Create WebView with relay options
    // 3. Create parameter attachments LAST (after WebView construction)
    // ========================================================================

    // ------------------------------------------------------------------------
    // STEP 1: CREATE RELAYS (before WebView!)
    // ------------------------------------------------------------------------
    driveRelay = std::make_unique<juce::WebSliderRelay>("drive");
    ageRelay = std::make_unique<juce::WebSliderRelay>("age");
    mixRelay = std::make_unique<juce::WebSliderRelay>("mix");

    // ------------------------------------------------------------------------
    // STEP 2: CREATE WEBVIEW (with relay options)
    // ------------------------------------------------------------------------
    webView = std::make_unique<juce::WebBrowserComponent>(
        juce::WebBrowserComponent::Options{}
            // Enable JUCE frontend library
            .withNativeIntegrationEnabled()

            // Resource provider for embedded files
            .withResourceProvider([this](const auto& url) {
                return getResource(url);
            })

            // FL Studio fix (prevents blank screen on focus loss)
            .withKeepPageLoadedWhenBrowserIsHidden()

            // Register relays with WebView
            .withOptionsFrom(*driveRelay)
            .withOptionsFrom(*ageRelay)
            .withOptionsFrom(*mixRelay)
    );

    // ------------------------------------------------------------------------
    // STEP 3: CREATE PARAMETER ATTACHMENTS (after WebView!)
    // ------------------------------------------------------------------------
    driveAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("drive"),
        *driveRelay,
        nullptr  // No undo manager
    );

    ageAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("age"),
        *ageRelay,
        nullptr
    );

    mixAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("mix"),
        *mixRelay,
        nullptr
    );

    // ------------------------------------------------------------------------
    // WEBVIEW SETUP
    // ------------------------------------------------------------------------
    webView->goToURL(juce::WebBrowserComponent::getResourceProviderRoot());
    addAndMakeVisible(*webView);

    // ------------------------------------------------------------------------
    // TIMER SETUP (for VU meter updates)
    // ------------------------------------------------------------------------
    // 60ms = ~16 FPS, sufficient for meter updates
    startTimerHz(16);

    // ------------------------------------------------------------------------
    // WINDOW SIZE
    // ------------------------------------------------------------------------
    setSize(500, 450);
    setResizable(false, false);
}

//==============================================================================
// Destructor
//==============================================================================

TapeAgeAudioProcessorEditor::~TapeAgeAudioProcessorEditor()
{
    // Members are automatically destroyed in reverse order of declaration:
    // 1. Attachments destroyed first (stop calling evaluateJavascript)
    // 2. WebView destroyed second (safe, attachments are gone)
    // 3. Relays destroyed last (safe, nothing using them)
}

//==============================================================================
// AudioProcessorEditor Overrides
//==============================================================================

void TapeAgeAudioProcessorEditor::paint(juce::Graphics& g)
{
    // WebView fills the entire editor, no custom painting needed
    juce::ignoreUnused(g);
}

void TapeAgeAudioProcessorEditor::resized()
{
    // Make WebView fill the entire editor bounds
    webView->setBounds(getLocalBounds());
}

//==============================================================================
// Timer Callback (VU meter updates)
//==============================================================================

void TapeAgeAudioProcessorEditor::timerCallback()
{
    // Example: Update VU meter from audio thread data
    // In production, PluginProcessor would store current level in std::atomic
    //
    // float dbLevel = processorRef.getCurrentOutputLevel();  // Atomic read
    //
    // webView->emitEventIfBrowserIsVisible("meterUpdate", {
    //     { "level", dbLevel }
    // });
}

//==============================================================================
// Resource Provider (JUCE 8 Required Pattern)
//==============================================================================

std::optional<juce::WebBrowserComponent::Resource> TapeAgeAudioProcessorEditor::getResource(
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

    // 404 - Resource not found
    return std::nullopt;
}
