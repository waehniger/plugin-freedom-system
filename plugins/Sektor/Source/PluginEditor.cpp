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
    polyphonyModeRelay = std::make_unique<juce::WebToggleButtonRelay>("POLYPHONY_MODE");

    // 2. Create WebView with relay options
    webView = std::make_unique<juce::WebBrowserComponent>(
        juce::WebBrowserComponent::Options{}
            .withNativeIntegrationEnabled()
            .withResourceProvider([this](const auto& url) { return getResource(url); })
            .withOptionsFrom(*grainSizeRelay)
            .withOptionsFrom(*densityRelay)
            .withOptionsFrom(*pitchShiftRelay)
            .withOptionsFrom(*spacingRelay)
            .withOptionsFrom(*polyphonyModeRelay)
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

    polyphonyModeAttachment = std::make_unique<juce::WebToggleButtonParameterAttachment>(
        *processorRef.parameters.getParameter("POLYPHONY_MODE"), *polyphonyModeRelay, nullptr);

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

// FileDragAndDropTarget implementation
bool SektorAudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (const auto& file : files) {
        juce::File f(file);
        if (f.hasFileExtension("wav;aif;aiff;mp3;flac"))
            return true;
    }
    return false;
}

void SektorAudioProcessorEditor::filesDropped(const juce::StringArray& files, int x, int y)
{
    juce::ignoreUnused(x, y);

    if (!files.isEmpty()) {
        juce::File droppedFile(files[0]);
        loadSampleAsync(droppedFile);
    }
}

// Sample loading implementation
void SektorAudioProcessorEditor::loadSampleAsync(const juce::File& file)
{
    // Show loading indicator
    updateUIStatus("Loading sample...");

    // Launch background thread for file I/O (Pattern #5: Threading - UI ↔ Audio Thread)
    std::thread([this, file]() {
        juce::AudioFormatManager formatManager;
        formatManager.registerBasicFormats();

        auto reader = std::unique_ptr<juce::AudioFormatReader>(formatManager.createReaderFor(file));
        if (reader == nullptr) {
            juce::MessageManager::callAsync([this]() {
                updateUIStatus("Error: Invalid audio file");
            });
            return;
        }

        // Load sample into temporary buffer
        auto tempBuffer = std::make_unique<juce::AudioBuffer<float>>(
            static_cast<int>(reader->numChannels),
            static_cast<int>(reader->lengthInSamples)
        );

        reader->read(tempBuffer.get(), 0, static_cast<int>(reader->lengthInSamples), 0, true, true);

        // Atomic swap in processor (thread-safe)
        processorRef.setSampleBuffer(std::move(tempBuffer));

        // Update UI on message thread
        juce::MessageManager::callAsync([this, filename = file.getFileName()]() {
            updateUIStatus("Sample loaded: " + filename);
        });

    }).detach();
}

void SektorAudioProcessorEditor::updateUIStatus(const juce::String& message)
{
    // Send status update to JavaScript via evaluateJavascript
    if (webView) {
        juce::String escapedMessage = message.replace("'", "\\'");
        juce::String jsCode = "if (window.updateStatus) { window.updateStatus('" + escapedMessage + "'); }";
        webView->evaluateJavascript(jsCode);
    }
}
