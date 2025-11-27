#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <cmath>

class SektorAudioProcessor : public juce::AudioProcessor
{
public:
    // Multi-Region Constants
    static constexpr int MaxRegions = 5;

    // Region data structure for DSP
    struct RegionData {
        float start = 0.0f;
        float end = 1.0f;
        bool active = false;
    };

    SektorAudioProcessor();
    ~SektorAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Sektor"; }
    bool acceptsMidi() const override { return true; }  // Instrument requires MIDI
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

    juce::AudioProcessorValueTreeState parameters;

    // Sample buffer management (thread-safe)
    void setSampleBuffer(std::unique_ptr<juce::AudioBuffer<float>> newBuffer);

    // Playhead visualization data
    struct PlayheadPosition {
        float normalizedPosition;  // 0.0-1.0 position in sample
        int regionIndex;            // Which region this voice is playing
        bool isActive;
    };
    std::vector<PlayheadPosition> getPlayheadPositions() const;

private:
    // Parameter layout creation
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Sample buffer pointer (atomic for thread safety)
    std::atomic<juce::AudioBuffer<float>*> currentSampleBuffer { nullptr };

    // Active grain structure for overlap-add synthesis
    struct ActiveGrain
    {
        float readPosition;     // Current fractional read position in grain
        int samplesRemaining;   // Samples until grain finishes
        float grainStartPhase;  // Sample position in source where grain started
        bool isActive;

        ActiveGrain() : readPosition(0.0f), samplesRemaining(0), grainStartPhase(0.0f), isActive(false) {}
    };

    // Voice class for overlapping grain playback
    class Voice
    {
    public:
        enum State { IDLE, PLAYING, RELEASING };

        Voice();

        void prepare(double sampleRate, int maxGrainSize);
        void setSourceBuffer(const juce::AudioBuffer<float>* newBuffer);
        void startNote(int midiNote, float velocity);
        void stopNote();
        void retrigger(int midiNote, float velocity);
        void triggerQuickRelease();
        void processBlock(juce::AudioBuffer<float>& output, int numSamples,
                         float grainSizeMs, float density, float pitchShiftSemitones, float spacing,
                         const std::vector<RegionData>& regions);

        bool isPlaying() const { return state == PLAYING; }
        bool isActive() const { return state != IDLE; }
        int getNoteNumber() const { return midiNoteNumber; }
        int getAge() const { return voiceAge; }
        float getGrainPhase() const { return grainPhase; }
        float getAbsoluteGrainPosition() const { return absoluteGrainPosition; }
        int getCurrentRegionIndex() const { return currentRegionIndex; }

    private:
        void generateTestSample(double sampleRate);
        void generateHannWindow(int grainSize);
        void generateGrain(int grainSamples, float spacing, const std::vector<RegionData>& regions);
        const RegionData& getRandomActiveRegion(const std::vector<RegionData>& regions);
        float readFractionalSample(float position);
        void processEnvelope();

        juce::Random rng;  // Random generator for region selection

        const juce::AudioBuffer<float>* sourceBuffer = nullptr;  // Pointer to shared sample buffer
        std::vector<float> hannWindow;          // Pre-calculated Hann window

        static constexpr int MAX_ACTIVE_GRAINS = 8;  // CPU protection
        std::vector<ActiveGrain> activeGrains;

        float grainPhase;          // Current relative position within region for grain extraction
        float absoluteGrainPosition; // Absolute sample position for playhead visualization
        int samplesUntilNextGrain; // Sample counter until next grain trigger
        State state;
        int midiNoteNumber;
        float noteVelocity;
        float envelopeLevel;       // 0.0-1.0 envelope amplitude
        float releaseRate;         // Envelope release rate (per sample)
        int voiceAge;              // Age in samples (for voice stealing)
        int currentRegionIndex;    // Which region this voice is currently playing from

        double currentSampleRate;
        int maxGrainSamples;
    };

    // Voice Manager for polyphonic voice allocation
    class VoiceManager
    {
    public:
        static constexpr int MAX_VOICES = 16;

        VoiceManager();

        void prepare(double sampleRate, int maxGrainSize);
        void setSharedBuffer(const juce::AudioBuffer<float>* newBuffer);
        void handleNoteOn(int noteNumber, float velocity, bool monoMode);
        void handleNoteOff(int noteNumber, bool monoMode);
        void handleAllNotesOff();
        void processBlock(juce::AudioBuffer<float>& output, int numSamples,
                         float grainSizeMs, float density, float pitchShiftSemitones, float spacing,
                         const std::vector<RegionData>& regions);

        const std::vector<Voice>& getVoices() const { return voices; }

    private:
        Voice* allocateVoice(int noteNumber, bool monoMode);
        Voice* findVoiceForNote(int noteNumber);

        std::vector<Voice> voices;
    };

    // DSP Components
    VoiceManager voiceManager;  // Phase 2.3: Full polyphonic voice management

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SektorAudioProcessor)
};
