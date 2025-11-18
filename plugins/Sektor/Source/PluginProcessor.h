#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <cmath>

class SektorAudioProcessor : public juce::AudioProcessor
{
public:
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

private:
    // Parameter layout creation
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Voice class for single grain playback
    class Voice
    {
    public:
        Voice();

        void prepare(double sampleRate, int maxGrainSize);
        void startNote(int midiNote, float velocity);
        void stopNote();
        void processBlock(juce::AudioBuffer<float>& output, int numSamples,
                         float grainSizeMs, float pitchShiftSemitones);

        bool isPlaying() const { return playing; }

    private:
        void generateTestSample(double sampleRate);
        void generateHannWindow(int grainSize);

        juce::AudioBuffer<float> sampleBuffer;  // Test sample (sine wave)
        std::vector<float> hannWindow;          // Pre-calculated Hann window

        float grainPhase;      // Current read position in grain (0 to grainSamples-1)
        float grainTrigger;    // Sample counter until next grain trigger
        bool playing;
        int midiNoteNumber;
        float noteVelocity;

        double currentSampleRate;
        int maxGrainSamples;
    };

    // DSP Components
    Voice monoVoice;  // Single monophonic voice for Phase 2.1

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SektorAudioProcessor)
};
