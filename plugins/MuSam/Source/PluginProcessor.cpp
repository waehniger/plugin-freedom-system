#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout MuSamAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // ============================================================================
    // GLOBAL PARAMETERS (2)
    // ============================================================================

    // speed - Float parameter (0.5 to 2.0, default 1.0)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "speed", 1 },
        "Speed",
        juce::NormalisableRange<float>(0.5f, 2.0f, 0.01f, 1.0f),
        1.0f,
        "x"
    ));

    // volume - Float parameter (-60.0 to 0.0 dB, default -6.0)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "volume", 1 },
        "Volume",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f, 1.0f),
        -6.0f,
        "dB"
    ));

    // ============================================================================
    // REGION PARAMETERS (5 regions × 8 parameters = 40 parameters)
    // ============================================================================

    // Helper lambda to add region parameters
    auto addRegionParameters = [&layout](int regionNum, float startDefault, float endDefault) {
        juce::String regionPrefix = "region_" + juce::String(regionNum) + "_";

        // Start position (0.0 to 100.0%)
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { regionPrefix + "start", 1 },
            "Region " + juce::String(regionNum) + " Start",
            juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f, 1.0f),
            startDefault,
            "%"
        ));

        // End position (0.0 to 100.0%)
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { regionPrefix + "end", 1 },
            "Region " + juce::String(regionNum) + " End",
            juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f, 1.0f),
            endDefault,
            "%"
        ));

        // Pitch (-24.0 to +24.0 semitones)
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { regionPrefix + "pitch", 1 },
            "Region " + juce::String(regionNum) + " Pitch",
            juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f, 1.0f),
            0.0f,
            "st"
        ));

        // Filter cutoff (20.0 to 20000.0 Hz, logarithmic skew 0.3)
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { regionPrefix + "filter_cutoff", 1 },
            "Region " + juce::String(regionNum) + " Filter Cutoff",
            juce::NormalisableRange<float>(20.0f, 20000.0f, 0.1f, 0.3f),
            20000.0f,
            "Hz"
        ));

        // Filter resonance (0.0 to 100.0%)
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { regionPrefix + "filter_resonance", 1 },
            "Region " + juce::String(regionNum) + " Filter Resonance",
            juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f, 1.0f),
            0.0f,
            "%"
        ));

        // Attack (0.0 to 1000.0 ms)
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { regionPrefix + "attack", 1 },
            "Region " + juce::String(regionNum) + " Attack",
            juce::NormalisableRange<float>(0.0f, 1000.0f, 0.1f, 1.0f),
            10.0f,
            "ms"
        ));

        // Decay (0.0 to 5000.0 ms)
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { regionPrefix + "decay", 1 },
            "Region " + juce::String(regionNum) + " Decay",
            juce::NormalisableRange<float>(0.0f, 5000.0f, 0.1f, 1.0f),
            500.0f,
            "ms"
        ));

        // Pan (-100.0 to +100.0%)
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { regionPrefix + "pan", 1 },
            "Region " + juce::String(regionNum) + " Pan",
            juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f, 1.0f),
            0.0f,
            "%"
        ));
    };

    // Add parameters for all 5 regions
    addRegionParameters(1, 0.0f, 20.0f);   // Region 1: 0-20%
    addRegionParameters(2, 20.0f, 40.0f);  // Region 2: 20-40%
    addRegionParameters(3, 40.0f, 60.0f);  // Region 3: 40-60%
    addRegionParameters(4, 60.0f, 80.0f);  // Region 4: 60-80%
    addRegionParameters(5, 80.0f, 100.0f); // Region 5: 80-100%

    // ============================================================================
    // SEQUENCER PARAMETERS (11)
    // ============================================================================

    // Step region assignments (8 steps)
    juce::StringArray stepChoices = { "None", "Region 1", "Region 2", "Region 3", "Region 4", "Region 5" };
    
    for (int step = 1; step <= 8; ++step) {
        int defaultIndex = (step <= 5) ? step : 0; // Steps 1-5 default to their region, 6-8 default to None
        
        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID { "step_" + juce::String(step) + "_region", 1 },
            "Step " + juce::String(step) + " Region",
            stepChoices,
            defaultIndex
        ));
    }

    // Playback mode (Sequential/Random/Custom)
    juce::StringArray playbackModeChoices = { "Sequential", "Random", "Custom" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "playback_mode", 1 },
        "Playback Mode",
        playbackModeChoices,
        0 // Default: Sequential
    ));

    // Loop mode (Bool)
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "loop_mode", 1 },
        "Loop Mode",
        true // Default: On
    ));

    // Crossfade time (0.0 to 500.0 ms)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "crossfade_time", 1 },
        "Crossfade Time",
        juce::NormalisableRange<float>(0.0f, 500.0f, 0.1f, 1.0f),
        50.0f,
        "ms"
    ));

    return layout;
}

MuSamAudioProcessor::MuSamAudioProcessor()
    : AudioProcessor(BusesProperties()
                        .withInput("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , parameters(*this, nullptr, "Parameters", createParameterLayout())
{
}

MuSamAudioProcessor::~MuSamAudioProcessor()
{
}

void MuSamAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // DSP initialization will be added in Stage 2
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void MuSamAudioProcessor::releaseResources()
{
    // Cleanup will be added in Stage 2
}

void MuSamAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    // Clear output buffer (DSP processing will be added in Stage 2)
    buffer.clear();
    
    // MIDI processing will be added in Stage 2
    juce::ignoreUnused(midiMessages);
}

juce::AudioProcessorEditor* MuSamAudioProcessor::createEditor()
{
    return new MuSamAudioProcessorEditor(*this);
}

void MuSamAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MuSamAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    
    if (xmlState != nullptr && xmlState->hasTagName(parameters.state.getType()))
    {
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
}

