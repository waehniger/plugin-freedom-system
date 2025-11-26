#pragma once
#include "PluginProcessor.h"
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_formats/juce_audio_formats.h>

class SektorAudioProcessorEditor : public juce::AudioProcessorEditor,
                                    public juce::FileDragAndDropTarget
{
public:
    explicit SektorAudioProcessorEditor(SektorAudioProcessor&);
    ~SektorAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    // FileDragAndDropTarget interface
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

private:
    SektorAudioProcessor& processorRef;

    // ⚠️ MEMBER DECLARATION ORDER IS CRITICAL ⚠️
    // Members are destroyed in REVERSE order of declaration
    // Declare dependencies AFTER what they depend on

    // 1️⃣ RELAYS FIRST (no dependencies)
    std::unique_ptr<juce::WebSliderRelay> grainSizeRelay;
    std::unique_ptr<juce::WebSliderRelay> densityRelay;
    std::unique_ptr<juce::WebSliderRelay> pitchShiftRelay;
    std::unique_ptr<juce::WebSliderRelay> spacingRelay;
    
    // Multi-Region relays (5 regions × 3 parameters each)
    std::vector<std::unique_ptr<juce::WebSliderRelay>> regionStartRelays;
    std::vector<std::unique_ptr<juce::WebSliderRelay>> regionEndRelays;
    std::vector<std::unique_ptr<juce::WebToggleButtonRelay>> regionActiveRelays;
    
    std::unique_ptr<juce::WebToggleButtonRelay> polyphonyModeRelay;  // Pattern #19: Use getToggleState for bool

    // 2️⃣ WEBVIEW SECOND (depends on relays via withOptionsFrom)
    std::unique_ptr<juce::WebBrowserComponent> webView;

    // 3️⃣ ATTACHMENTS LAST (depend on both relays and webView)
    std::unique_ptr<juce::WebSliderParameterAttachment> grainSizeAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> densityAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> pitchShiftAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> spacingAttachment;
    
    // Multi-Region attachments (5 regions × 3 parameters each)
    std::vector<std::unique_ptr<juce::WebSliderParameterAttachment>> regionStartAttachments;
    std::vector<std::unique_ptr<juce::WebSliderParameterAttachment>> regionEndAttachments;
    std::vector<std::unique_ptr<juce::WebToggleButtonParameterAttachment>> regionActiveAttachments;
    
    std::unique_ptr<juce::WebToggleButtonParameterAttachment> polyphonyModeAttachment;

    // Helper for resource serving
    std::optional<juce::WebBrowserComponent::Resource> getResource(const juce::String& url);

    // Sample loading
    void loadSampleAsync(const juce::File& file);
    void loadAudioFromBase64(const juce::String& base64Data, const juce::String& filename);
    void updateUIStatus(const juce::String& message);
    void openFileBrowser();

    // Waveform visualization
    void sendWaveformDataToJS(const juce::AudioBuffer<float>& buffer);

    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SektorAudioProcessorEditor)
};
