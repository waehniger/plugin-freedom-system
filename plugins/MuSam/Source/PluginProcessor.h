#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <atomic>

class MuSamAudioProcessor : public juce::AudioProcessor
{
public:
    MuSamAudioProcessor();
    ~MuSamAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "MuSam"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Public access to parameters for editor
    juce::AudioProcessorValueTreeState parameters;

    // Sample loading interface (called from UI thread)
    void loadSampleFromFile(const juce::File& file);

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // ============================================================================
    // Phase 4.1: Core Sample Playback
    // ============================================================================

    // Sample loading
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioBuffer<float>> sampleBuffer;  // Current sample (stereo or mono)
    std::atomic<juce::AudioBuffer<float>*> activeSampleBuffer{ nullptr };  // Atomic pointer for thread-safe access
    double fileSampleRate = 44100.0;
    int fileNumChannels = 2;
    std::atomic<bool> sampleReady{ false };
    
    // Forward declaration for sample loading thread
    class SampleLoaderThread;
    std::unique_ptr<SampleLoaderThread> loaderThread;
    
    // Friend class for sample loading thread
    friend class SampleLoaderThread;

    // Region playback state (5 regions)
    struct RegionPlaybackState
    {
        float samplePosition = 0.0f;  // Current playback position in samples
        bool isActive = false;          // Whether this region is currently playing
        int startSample = 0;            // Region start in samples
        int endSample = 0;              // Region end in samples
        
        // Phase 4.2: Per-Region Processing
        juce::dsp::IIR::Filter<float> filter;
        float envelopeAmplitude = 0.0f;  // Current envelope amplitude (0.0 to 1.0)
        float envelopeTime = 0.0f;        // Current envelope time in samples
        enum EnvelopePhase { Attack, Decay, Idle };
        EnvelopePhase envelopePhase = Idle;
        
        // Phase 4.3: Pitch Shifting (Granular Synthesis)
        juce::AudioBuffer<float> grainBuffer;  // Circular buffer for grains
        float grainPosition = 0.0f;            // Current position in grain buffer
        float grainWritePosition = 0.0f;       // Write position for overlap-add
        float pitchRatio = 1.0f;               // Current pitch ratio
    };
    RegionPlaybackState regionStates[5];
    
    // Phase 4.2: DSP processors
    juce::dsp::Panner<float> panner;  // Shared panner (reused per region)
    
    // Phase 4.4: Sequencer Engine
    struct SequencerState
    {
        int currentStep = 0;           // Current step (0-7)
        float stepTime = 0.0f;         // Time in current step (in samples)
        float stepDuration = 0.0f;     // Duration of current step (in samples)
        bool isPlaying = false;        // Whether sequencer is active
        int previousRegion = -1;       // Previous active region for crossfade
        float crossfadePosition = 0.0f; // Crossfade position (0.0 to 1.0)
    };
    SequencerState sequencerState;

    // Playback state
    double currentSampleRate = 44100.0;
    bool isPlaying = false;

    // Helper methods
    void updateRegionBoundaries();
    void processRegionPlayback(int regionIndex, juce::AudioBuffer<float>& buffer, int startSample, int numSamples);
    float linearInterpolateSample(const juce::AudioBuffer<float>& buffer, int channel, float position);
    
    // Phase 4.2: Per-Region Processing helpers
    void updateFilterCoefficients(int regionIndex);
    void updateEnvelope(int regionIndex, int numSamples);
    void applyPan(int regionIndex, juce::AudioBuffer<float>& buffer, int startSample, int numSamples);
    
    // Phase 4.3: Pitch Shifting helpers
    void applyPitchShift(int regionIndex, juce::AudioBuffer<float>& buffer, int startSample, int numSamples);
    float calculatePitchRatio(int regionIndex);
    float hannWindow(float position);  // Position 0.0 to 1.0
    
    // Phase 4.4: Sequencer helpers
    void updateSequencer(int numSamples);
    int getActiveRegionForStep(int step);
    void advanceSequencerStep();
    void applyCrossfade(juce::AudioBuffer<float>& buffer, int startSample, int numSamples, int outgoingRegion, int incomingRegion);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MuSamAudioProcessor)
};

