#include "PluginEditor.h"
#include "BinaryData.h"

SektorAudioProcessorEditor::SektorAudioProcessorEditor(SektorAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    // 1. Create relays FIRST
    grainSizeRelay = std::make_unique<juce::WebSliderRelay>("GRAIN_SIZE");
    densityRelay = std::make_unique<juce::WebSliderRelay>("DENSITY");
    pitchShiftRelay = std::make_unique<juce::WebSliderRelay>("PITCH_SHIFT");
    spacingRelay = std::make_unique<juce::WebSliderRelay>("SPACING");

    // 2. Create WebView with relay options
    webView = std::make_unique<juce::WebBrowserComponent>(
        juce::WebBrowserComponent::Options{}
            .withNativeIntegrationEnabled()
            .withResourceProvider([this](const auto& url) { return getResource(url); })
            .withOptionsFrom(*grainSizeRelay)
            .withOptionsFrom(*densityRelay)
            .withOptionsFrom(*pitchShiftRelay)
            .withOptionsFrom(*spacingRelay)
    );

    // 3. Create attachments LAST (Pattern #12: 3 parameters required)
    grainSizeAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("GRAIN_SIZE"), *grainSizeRelay, nullptr);

    densityAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("DENSITY"), *densityRelay, nullptr);

    pitchShiftAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("PITCH_SHIFT"), *pitchShiftRelay, nullptr);

    spacingAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("SPACING"), *spacingRelay, nullptr);

    // Add WebView to editor
    addAndMakeVisible(*webView);

    // Set editor size (match mockup: 900×600)
    setSize(900, 600);

    // Navigate to UI
    webView->goToURL(juce::WebBrowserComponent::getResourceProviderRoot());
}

SektorAudioProcessorEditor::~SektorAudioProcessorEditor()
{
    // Destructor handles cleanup in reverse order (automatic with unique_ptr)
}

void SektorAudioProcessorEditor::paint(juce::Graphics& g)
{
    // WebView handles all painting
    juce::ignoreUnused(g);
}

void SektorAudioProcessorEditor::resized()
{
    // WebView fills entire editor
    if (webView)
        webView->setBounds(getLocalBounds());
}

// Pattern #8: Explicit URL Mapping (REQUIRED)
std::optional<juce::WebBrowserComponent::Resource>
SektorAudioProcessorEditor::getResource(const juce::String& url)
{
    auto makeVector = [](const char* data, int size) {
        return std::vector<std::byte>(
            reinterpret_cast<const std::byte*>(data),
            reinterpret_cast<const std::byte*>(data) + size
        );
    };

    // Explicit mapping - easy to debug and maintain
    if (url == "/" || url == "/index.html") {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::index_html, BinaryData::index_htmlSize),
            juce::String("text/html")
        };
    }

    if (url == "/js/juce/index.js") {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::index_js, BinaryData::index_jsSize),
            juce::String("text/javascript")
        };
    }

    if (url == "/js/juce/check_native_interop.js") {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::check_native_interop_js,
                      BinaryData::check_native_interop_jsSize),
            juce::String("text/javascript")
        };
    }

    // Resource not found
    juce::Logger::writeToLog("[Sektor] Resource not found: " + url);
    return std::nullopt;
}
