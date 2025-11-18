# Sektor - Parameter Specification

**Generated from:** v1 UI mockup (2025-11-18)
**UI Layout:** 900×600 window, minimalistic dark interface with orange accents
**Plugin Type:** Instrument (Granular Sampler) - IS_SYNTH TRUE

---

## Parameters (7 total)

### 1. GRAIN_SIZE (AudioParameterFloat)

- **Range:** 10.0 - 500.0 ms
- **Default:** 100.0 ms
- **Unit:** milliseconds
- **Resolution:** 0.1 ms (1 decimal place)
- **Skew:** Linear
- **UI Control:** Horizontal slider
- **UI Position:** x=30, y=480, width=180, height=30
- **Purpose:** Grain duration for granular synthesis
- **Mapping:** Direct - value in milliseconds converted to sample count: `grainSamples = (value / 1000.0) * sampleRate`
- **APVTS ID:** "GRAIN_SIZE"
- **String ID:** "grain_size"

---

### 2. DENSITY (AudioParameterFloat)

- **Range:** 1.0 - 200.0 grains/sec
- **Default:** 50.0 grains/sec
- **Unit:** grains per second
- **Resolution:** 0.1 grains/sec (1 decimal place)
- **Skew:** Linear
- **UI Control:** Horizontal slider
- **UI Position:** x=240, y=480, width=180, height=30
- **Purpose:** Number of grains triggered per second
- **Mapping:** Direct - value is grain trigger rate: `grainInterval = sampleRate / value`
- **APVTS ID:** "DENSITY"
- **String ID:** "density"

---

### 3. PITCH_SHIFT (AudioParameterFloat)

- **Range:** -12.0 to +12.0 semitones
- **Default:** 0.0 semitones
- **Unit:** semitones
- **Resolution:** 0.1 semitones (1 decimal place)
- **Skew:** Linear
- **UI Control:** Horizontal slider
- **UI Position:** x=450, y=480, width=180, height=30
- **Purpose:** Transpose grain playback pitch
- **Mapping:** Logarithmic - semitones to playback rate: `rate = pow(2.0, value / 12.0)`
  - -12 semitones → 0.5x rate (one octave down)
  - 0 semitones → 1.0x rate (no shift)
  - +12 semitones → 2.0x rate (one octave up)
- **APVTS ID:** "PITCH_SHIFT"
- **String ID:** "pitch_shift"

---

### 4. SPACING (AudioParameterFloat)

- **Range:** 0.1 - 2.0 (multiplier)
- **Default:** 1.0
- **Unit:** None (multiplier)
- **Resolution:** 0.01 (2 decimal places)
- **Skew:** Linear
- **UI Control:** Horizontal slider
- **UI Position:** x=660, y=480, width=180, height=30
- **Purpose:** Grain position advancement multiplier (controls overlap/texture)
- **Mapping:** Direct - multiplier for grain position increment
  - 0.1 = Dense overlap (chorus-like texture)
  - 1.0 = Normal overlap (default grain spacing)
  - 2.0 = Sparse grains (granular texture)
- **APVTS ID:** "SPACING"
- **String ID:** "spacing"

---

### 5. REGION_START (AudioParameterFloat)

- **Range:** 0.0 - 1.0 (normalized)
- **Default:** 0.0
- **Unit:** None (normalized position)
- **Resolution:** 0.001 (3 decimal places)
- **Skew:** Linear
- **UI Control:** Custom - Draggable region marker on waveform canvas
- **UI Position:** x=50, y=100, width=10, height=260 (waveform canvas position)
- **Description:** Start position of playback region (normalized 0.0-1.0)
- **Purpose:** Define left boundary of region-based playback
- **Mapping:** Normalized to sample position: `startFrame = value * totalSampleFrames`
- **APVTS ID:** "REGION_START"
- **String ID:** "region_start"
- **Interaction:** Click-drag left marker on waveform to update

---

### 6. REGION_END (AudioParameterFloat)

- **Range:** 0.0 - 1.0 (normalized)
- **Default:** 1.0
- **Unit:** None (normalized position)
- **Resolution:** 0.001 (3 decimal places)
- **Skew:** Linear
- **UI Control:** Custom - Draggable region marker on waveform canvas
- **UI Position:** x=840, y=100, width=10, height=260 (waveform canvas position)
- **Description:** End position of playback region (normalized 0.0-1.0)
- **Purpose:** Define right boundary of region-based playback
- **Mapping:** Normalized to sample position: `endFrame = value * totalSampleFrames`
- **APVTS ID:** "REGION_END"
- **String ID:** "region_end"
- **Interaction:** Click-drag right marker on waveform to update

---

### 7. POLYPHONY_MODE (AudioParameterBool)

- **Type:** Boolean
- **Values:** false (Mono) / true (Poly)
- **Default:** true (Polyphony enabled)
- **UI Control:** Toggle switch
- **UI Position:** x=30, y=540, width=120, height=30
- **Toggle Labels:** ["Mono", "Poly"]
- **Purpose:** Switch between monophonic and polyphonic voice modes
- **Behavior:**
  - false (Mono): Single voice playback, last-note priority, legato playback
  - true (Poly): Up to 16 simultaneous voices
- **APVTS ID:** "POLYPHONY_MODE"
- **String ID:** "polyphony_mode"

---

## Parameter Summary Table

| ID | Name | Type | Range | Default | Unit | Resolution |
|---|---|---|---|---|---|---|
| GRAIN_SIZE | Grain Size | Float | 10-500 | 100 | ms | 0.1 |
| DENSITY | Density | Float | 1-200 | 50 | grains/sec | 0.1 |
| PITCH_SHIFT | Pitch Shift | Float | -12 to +12 | 0 | semitones | 0.1 |
| SPACING | Spacing | Float | 0.1-2.0 | 1.0 | (multiplier) | 0.01 |
| REGION_START | Region Start | Float | 0.0-1.0 | 0.0 | (normalized) | 0.001 |
| REGION_END | Region End | Float | 0.0-1.0 | 1.0 | (normalized) | 0.001 |
| POLYPHONY_MODE | Polyphony Mode | Bool | false/true | true | (toggle) | N/A |

---

## Implementation Notes

### AudioParameter Types (JUCE 8)

```cpp
// Float parameters with ranges and skew
parameters.createAndReleaseParameter(
    std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("GRAIN_SIZE", 1),
        "Grain Size",
        juce::NormalisableRange<float>(10.0f, 500.0f),
        100.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter
    )
);

// Boolean parameter
parameters.createAndReleaseParameter(
    std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("POLYPHONY_MODE", 1),
        "Polyphony Mode",
        true
    )
);
```

### Normalization Strategy

- **Float parameters:** All ranges normalized to 0.0-1.0 internally by JUCE
  - Audio thread reads via `getRawParameterValue()->load()` (returns 0.0-1.0)
  - Must denormalize in DSP: `actualValue = param->load() * (max - min) + min`
- **Boolean parameter:** Already 0.0/1.0, direct conversion
- **Region parameters:** Already 0.0-1.0 normalized, direct use

### APVTS Configuration

```cpp
juce::AudioProcessorValueTreeState parameters {
    *this,
    nullptr,
    juce::Identifier("SektorState"),
    {
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("GRAIN_SIZE", 1), ...),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("DENSITY", 1), ...),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("PITCH_SHIFT", 1), ...),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("SPACING", 1), ...),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("REGION_START", 1), ...),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("REGION_END", 1), ...),
        std::make_unique<juce::AudioParameterBool>(juce::ParameterID("POLYPHONY_MODE", 1), ...),
    }
};
```

### Parameter Access in processBlock()

```cpp
// Read normalized values (0.0-1.0)
float grainSizeNorm = parameters.getRawParameterValue("GRAIN_SIZE")->load();
float densityNorm = parameters.getRawParameterValue("DENSITY")->load();
float pitchShiftNorm = parameters.getRawParameterValue("PITCH_SHIFT")->load();
float spacingNorm = parameters.getRawParameterValue("SPACING")->load();
float regionStartNorm = parameters.getRawParameterValue("REGION_START")->load();
float regionEndNorm = parameters.getRawParameterValue("REGION_END")->load();
bool polyMode = (parameters.getRawParameterValue("POLYPHONY_MODE")->load() >= 0.5f);

// Denormalize to actual values
float grainSizeMs = grainSizeNorm * 490.0f + 10.0f;      // 10-500
float densityGrains = densityNorm * 199.0f + 1.0f;       // 1-200
float pitchShiftSemitones = pitchShiftNorm * 24.0f - 12.0f; // -12 to +12
float spacingMult = spacingNorm * 1.9f + 0.1f;           // 0.1-2.0
float regionStart = regionStartNorm;                      // Already 0.0-1.0
float regionEnd = regionEndNorm;                          // Already 0.0-1.0
```

---

## UI/DSP Binding

### WebView Parameter IDs (JavaScript)

JavaScript uses lowercase parameter IDs (auto-converted from C++ IDs):
- `getSliderState("grain_size")` → Updates GRAIN_SIZE parameter
- `getSliderState("density")` → Updates DENSITY parameter
- `getSliderState("pitch_shift")` → Updates PITCH_SHIFT parameter
- `getSliderState("spacing")` → Updates SPACING parameter
- `getSliderState("region_start")` → Updates REGION_START parameter (from waveform drag)
- `getSliderState("region_end")` → Updates REGION_END parameter (from waveform drag)
- `getToggleState("polyphony_mode")` → Updates POLYPHONY_MODE parameter

### WebView Attachments (C++)

In PluginEditor.cpp:
```cpp
// Sliders
gainAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
    *processorRef.parameters.getParameter("GRAIN_SIZE"), *gainRelay, nullptr);

// Toggle switch
modeAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
    *processorRef.parameters.getParameter("POLYPHONY_MODE"), *modeRelay, nullptr);

// Region markers (custom C++ implementation, not WebView bound)
// Updated via callback when user drags on waveform
```

---

## Preset System

Parameters automatically saved/restored via APVTS:
- All 7 parameters persisted as XML in ValueTree
- Custom sample file path also persisted (see architecture.md)
- Preset format: `.fxp` (VST2) or XML (DAW-native)

---

## Validation Checklist

- [x] 7 parameters defined with correct ranges/defaults
- [x] Parameter IDs match mockup (GRAIN_SIZE, DENSITY, etc.)
- [x] UI controls match parameter types (sliders, toggle, custom waveform)
- [x] Normalization strategy documented
- [x] WebView binding ready (lowercase IDs for JavaScript)
- [x] APVTS configuration template ready
- [x] processBlock() access patterns documented
- [x] No circular dependencies between parameters
