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

    // Multi-Region relays (5 regions × 3 parameters each)
    for (int i = 0; i < SektorAudioProcessor::MaxRegions; ++i)
    {
        juce::String idSuffix = "_" + juce::String(i);
        regionStartRelays.push_back(std::make_unique<juce::WebSliderRelay>("REGION_START" + idSuffix));
        regionEndRelays.push_back(std::make_unique<juce::WebSliderRelay>("REGION_END" + idSuffix));
        regionActiveRelays.push_back(std::make_unique<juce::WebToggleButtonRelay>("REGION_ACTIVE" + idSuffix));
    }

    // 2. Create WebView with relay options and native functions
    auto webViewOptions = juce::WebBrowserComponent::Options{}
        .withNativeIntegrationEnabled()
        .withResourceProvider([this](const auto& url) { return getResource(url); })
        .withOptionsFrom(*grainSizeRelay)
        .withOptionsFrom(*densityRelay)
        .withOptionsFrom(*pitchShiftRelay)
        .withOptionsFrom(*spacingRelay)
        .withOptionsFrom(*polyphonyModeRelay);

    // Add all region relay options
    for (int i = 0; i < SektorAudioProcessor::MaxRegions; ++i)
    {
        webViewOptions = webViewOptions
            .withOptionsFrom(*regionStartRelays[i])
            .withOptionsFrom(*regionEndRelays[i])
            .withOptionsFrom(*regionActiveRelays[i]);
    }

    // Add native functions
    webView = std::make_unique<juce::WebBrowserComponent>(
        webViewOptions
            // Native function for browse button
            .withNativeFunction("nativeOpenBrowser", [this](const juce::Array<juce::var>& args, auto completion) {
                juce::ignoreUnused(args);
                openFileBrowser();
                completion(true);
            })

            // Native function for drag & drop with Base64 data
            .withNativeFunction("nativeFileDrop", [this](const juce::Array<juce::var>& args, auto completion) {
                if (args.size() >= 2) {
                    juce::String filename = args[0].toString();
                    juce::String base64Data = args[1].toString();

                    // Remove data URL header if present (e.g., "data:audio/wav;base64,")
                    int commaPos = base64Data.indexOf(",");
                    if (commaPos != -1)
                        base64Data = base64Data.substring(commaPos + 1);

                    loadAudioFromBase64(base64Data, filename);
                }
                completion(true);
            })
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

    // Multi-Region attachments (5 regions × 3 parameters each)
    for (int i = 0; i < SektorAudioProcessor::MaxRegions; ++i)
    {
        juce::String idSuffix = "_" + juce::String(i);

        regionStartAttachments.push_back(std::make_unique<juce::WebSliderParameterAttachment>(
            *processorRef.parameters.getParameter("REGION_START" + idSuffix), *regionStartRelays[i], nullptr));

        regionEndAttachments.push_back(std::make_unique<juce::WebSliderParameterAttachment>(
            *processorRef.parameters.getParameter("REGION_END" + idSuffix), *regionEndRelays[i], nullptr));

        regionActiveAttachments.push_back(std::make_unique<juce::WebToggleButtonParameterAttachment>(
            *processorRef.parameters.getParameter("REGION_ACTIVE" + idSuffix), *regionActiveRelays[i], nullptr));
    }

    // Add WebView to editor
    addAndMakeVisible(*webView);

    // Drag-and-drop is enabled by FileDragAndDropTarget interface
    std::cout << "[SEKTOR INIT] Drag-and-drop interface registered (FileDragAndDropTarget)" << std::endl;

    // Set editor size (match mockup: 900×600)
    setSize(900, 600);

    // Navigate to UI
    webView->goToURL(juce::WebBrowserComponent::getResourceProviderRoot());
    std::cout << "[SEKTOR INIT] Editor initialized successfully with " << SektorAudioProcessor::MaxRegions << " regions" << std::endl;
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
    std::cout << "[SEKTOR DRAG] isInterestedInFileDrag called with " << files.size() << " files" << std::endl;

    for (const auto& file : files) {
        std::cout << "[SEKTOR DRAG] Checking file: " << file.toStdString() << std::endl;
        juce::File f(file);
        if (f.hasFileExtension("wav;aif;aiff;mp3;flac")) {
            std::cout << "[SEKTOR DRAG] ✓ File is audio format, interested!" << std::endl;
            return true;
        }
    }
    std::cout << "[SEKTOR DRAG] ✗ No audio files found, not interested" << std::endl;
    return false;
}

void SektorAudioProcessorEditor::filesDropped(const juce::StringArray& files, int x, int y)
{
    juce::ignoreUnused(x, y);

    std::cout << "[SEKTOR DROP] Files dropped count: " << files.size() << std::endl;

    if (!files.isEmpty()) {
        juce::File droppedFile(files[0]);
        std::cout << "[SEKTOR DROP] File path: " << droppedFile.getFullPathName().toStdString() << std::endl;
        std::cout << "[SEKTOR DROP] File exists: " << (droppedFile.exists() ? "YES" : "NO") << std::endl;
        loadSampleAsync(droppedFile);
    } else {
        std::cout << "[SEKTOR DROP] ERROR: No files in array!" << std::endl;
    }
}

// Sample loading implementation
void SektorAudioProcessorEditor::loadSampleAsync(const juce::File& file)
{
    // Debug log: file selection
    std::cout << "[SEKTOR SAMPLE] Starting load of: " << file.getFullPathName().toStdString() << std::endl;

    // Show loading indicator
    updateUIStatus("Loading sample...");

    // Launch background thread for file I/O (Pattern #5: Threading - UI ↔ Audio Thread)
    std::thread([this, file]() {
        juce::AudioFormatManager formatManager;
        formatManager.registerBasicFormats();

        std::cout << "[SEKTOR SAMPLE] Creating reader for: " << file.getFileName().toStdString() << std::endl;

        auto reader = std::unique_ptr<juce::AudioFormatReader>(formatManager.createReaderFor(file));
        if (reader == nullptr) {
            std::cout << "[SEKTOR SAMPLE] ERROR: Failed to create reader - invalid format" << std::endl;
            juce::MessageManager::callAsync([this]() {
                updateUIStatus("Error: Invalid audio file");
            });
            return;
        }

        std::cout << "[SEKTOR SAMPLE] Reader created successfully" << std::endl;
        std::cout << "[SEKTOR SAMPLE]   Channels: " << reader->numChannels << std::endl;
        std::cout << "[SEKTOR SAMPLE]   Length: " << reader->lengthInSamples << " samples" << std::endl;
        std::cout << "[SEKTOR SAMPLE]   Sample rate: " << reader->sampleRate << " Hz" << std::endl;

        // Load sample into temporary buffer
        auto tempBuffer = std::make_unique<juce::AudioBuffer<float>>(
            static_cast<int>(reader->numChannels),
            static_cast<int>(reader->lengthInSamples)
        );

        std::cout << "[SEKTOR SAMPLE] Reading audio data..." << std::endl;
        reader->read(tempBuffer.get(), 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
        std::cout << "[SEKTOR SAMPLE] Audio data loaded successfully" << std::endl;

        // Send waveform data to UI before moving buffer
        sendWaveformDataToJS(*tempBuffer);

        // Atomic swap in processor (thread-safe)
        processorRef.setSampleBuffer(std::move(tempBuffer));
        std::cout << "[SEKTOR SAMPLE] Sample buffer swapped in processor" << std::endl;

        // Update UI on message thread
        juce::MessageManager::callAsync([this, filename = file.getFileName()]() {
            std::cout << "[SEKTOR SAMPLE] Updating UI with filename: " << filename.toStdString() << std::endl;
            updateUIStatus("Sample loaded: " + filename);
        });

    }).detach();
}

void SektorAudioProcessorEditor::loadAudioFromBase64(const juce::String& base64Data, const juce::String& filename)
{
    updateUIStatus("Processing dropped file...");

    // Launch background thread (avoid blocking UI)
    std::thread([this, base64Data, filename]() {
        // 1. Decode Base64 to binary
        juce::MemoryOutputStream outputStream;
        if (juce::Base64::convertFromBase64(outputStream, base64Data))
        {
            auto dataBlock = outputStream.getMemoryBlock();

            // 2. Create MemoryInputStream for AudioFormatReader
            auto inputStream = std::make_unique<juce::MemoryInputStream>(dataBlock, false);

            juce::AudioFormatManager formatManager;
            formatManager.registerBasicFormats();

            // 3. Create reader from memory stream
            std::unique_ptr<juce::AudioFormatReader> reader(
                formatManager.createReaderFor(std::move(inputStream))
            );

            if (reader) {
                // 4. Load into audio buffer (same as existing loadSampleAsync)
                auto tempBuffer = std::make_unique<juce::AudioBuffer<float>>(
                    static_cast<int>(reader->numChannels),
                    static_cast<int>(reader->lengthInSamples)
                );

                reader->read(tempBuffer.get(), 0, static_cast<int>(reader->lengthInSamples), 0, true, true);

                // Send waveform data to UI before moving buffer
                sendWaveformDataToJS(*tempBuffer);

                processorRef.setSampleBuffer(std::move(tempBuffer));

                // 5. Update UI on message thread
                juce::MessageManager::callAsync([this, filename]() {
                    updateUIStatus("Sample loaded: " + filename);
                });
            } else {
                juce::MessageManager::callAsync([this]() {
                    updateUIStatus("Error: Format not recognized");
                });
            }
        } else {
            juce::MessageManager::callAsync([this]() {
                updateUIStatus("Error: Failed to decode audio data");
            });
        }
    }).detach();
}

void SektorAudioProcessorEditor::updateUIStatus(const juce::String& message)
{
    std::cout << "[SEKTOR UI] Updating UI status: " << message.toStdString() << std::endl;

    // Send status update to JavaScript via evaluateJavascript
    if (webView) {
        std::cout << "[SEKTOR UI] WebView exists, executing JavaScript..." << std::endl;
        juce::String escapedMessage = message.replace("'", "\\'");
        juce::String jsCode = "if (window.updateStatus) { window.updateStatus('" + escapedMessage + "'); } else { console.error('updateStatus function not found'); }";
        std::cout << "[SEKTOR UI] JS Code: " << jsCode.toStdString() << std::endl;
        webView->evaluateJavascript(jsCode);
    } else {
        std::cout << "[SEKTOR UI] ERROR: WebView is null!" << std::endl;
    }
}

void SektorAudioProcessorEditor::openFileBrowser()
{
    // IMPORTANT: FileChooser must run on message thread
    // Native functions may be called from other threads, so force message thread
    juce::MessageManager::callAsync([this]() {
        std::cout << "[SEKTOR BROWSE] Opening file browser dialog..." << std::endl;

        fileChooser = std::make_unique<juce::FileChooser>(
            "Load audio sample",
            juce::File::getSpecialLocation(juce::File::userMusicDirectory),
            "*.wav;*.aif;*.aiff;*.mp3;*.flac"
        );

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& chooser) {
                auto selectedFile = chooser.getResult();
                if (selectedFile.exists()) {
                    std::cout << "[SEKTOR BROWSE] User selected: " << selectedFile.getFullPathName().toStdString() << std::endl;
                    loadSampleAsync(selectedFile);
                } else {
                    std::cout << "[SEKTOR BROWSE] User cancelled file selection" << std::endl;
                }
            }
        );
    });
}

void SektorAudioProcessorEditor::sendWaveformDataToJS(const juce::AudioBuffer<float>& buffer)
{
    // Configuration
    const int numPoints = 1000; // Number of points for display
    const int totalSamples = buffer.getNumSamples();

    // Prevent division by zero with empty buffers
    if (totalSamples <= 0) return;

    juce::StringArray peaks;
    const int samplesPerChunk = std::ceil((float)totalSamples / (float)numPoints);

    // Downsampling (calculate only the peak per chunk)
    for (int i = 0; i < numPoints; ++i)
    {
        int startSample = i * samplesPerChunk;
        if (startSample >= totalSamples) break;

        // Get magnitude (volume) for this region
        // We use channel 0 (left) or mono
        float magnitude = buffer.getMagnitude(0, startSample, std::min(samplesPerChunk, totalSamples - startSample));

        // Limit to 0..1 and format
        peaks.add(juce::String(magnitude, 3));
    }

    // Build JSON array string: "[0.1, 0.5, 0.2, ...]"
    juce::String jsonStr = "[" + peaks.joinIntoString(",") + "]";

    // Send to JS (must happen on message thread)
    juce::MessageManager::callAsync([this, jsonStr]() {
        if (webView) {
            // Call the JS function 'updateWaveform'
            webView->evaluateJavascript("if (window.updateWaveform) window.updateWaveform(" + jsonStr + ");");
        }
    });
}

