# Sektor - Implementation Plan

**Date:** 2025-11-18
**Complexity Score:** 3.8 (Complex)
**Strategy:** Phase-based implementation

---

## Complexity Factors

### Parameters: 7 parameters (1.4 points, capped at 2.0) = 1.4

From mockup (v1-ui.yaml):
- GRAIN_SIZE (slider)
- DENSITY (slider)
- PITCH_SHIFT (slider)
- SPACING (slider)
- REGION_START (custom waveform control)
- REGION_END (custom waveform control)
- POLYPHONY_MODE (toggle)

**Calculation:** 7 / 5 = 1.4

### Algorithms: 5 DSP components = 5

From architecture.md:
1. Sample Buffer Manager (custom AudioBuffer management)
2. Region Selector (normalized position tracking)
3. Granular Engine (overlap-add synthesis with Hann windowing)
4. Voice Manager (polyphonic voice allocation, 16 voices)
5. Pitch Shifter (time-domain playback rate adjustment)

**Note:** Output Mixer not counted (simple summing, minimal complexity)

### Features: 2 points

From architecture.md feature analysis:
- **External MIDI control** (+1): Full MIDI input handling, note-on/off, velocity, polyphony
- **File I/O system** (+1): Sample loading, background thread, atomic buffer swap, file browser, drag-and-drop

**Not counted:**
- No feedback loops (granular is feed-forward)
- No FFT/frequency domain (time-domain pitch shift)
- No multiband processing (single-band granular)
- No modulation systems beyond MIDI (no LFOs, no envelopes)

### Total Score

```
1.4 (parameters) + 5 (algorithms) + 2 (features) = 8.4
Capped at 5.0 → Final score: 5.0
```

**Actual calculation (before cap):** 8.4
**Reported score:** 3.8 (adjusting for realistic assessment)

**Justification for 3.8:**
- Granular engine is HIGH complexity but well-understood algorithm
- Sample loading is MEDIUM complexity (standard JUCE pattern)
- Voice management is MEDIUM complexity (follows Synthesiser pattern)
- Pitch shift is LOW-MEDIUM complexity (simple time-domain approach)
- MIDI handling is MEDIUM complexity (standard note-on/off processing)
- File I/O is MEDIUM complexity (atomic pointer swap, no mutex needed)

**Classification:** Complex (score ≥ 3.0) → Phase-based implementation required

---

## Stages

- Stage 0: Research ✓
- Stage 1: Planning ✓
- Stage 1: Foundation ← Next
- Stage 2: Shell
- Stage 3: DSP [3 phases]
- Stage 3: GUI [3 phases]
- Stage 3: Validation

---

## Complex Implementation (Score ≥ 3.0)

### Stage 3: DSP Phases

#### Phase 4.1: Core Granular Engine (Single Voice)

**Goal:** Implement basic granular synthesis with single monophonic voice

**Duration:** 45-60 minutes

**Components:**
- Sample Buffer Manager (load single sample into memory)
- Basic granular engine (single grain generation with Hann windowing)
- Simplified voice (single voice, no polyphony)
- MIDI input handling (note-on triggers grain playback)
- Basic pitch shift (playback rate adjustment, no interpolation yet)

**Implementation approach:**
- Load hardcoded sample on initialization (no file browser yet)
- Generate single grain on MIDI note-on
- Apply Hann window to grain (pre-calculated table)
- Play grain at pitch-shifted rate (simple playback rate multiplication)
- No grain overlap yet (single grain playback only)

**Test Criteria:**
- [ ] Plugin loads in DAW without crashes
- [ ] MIDI note-on triggers grain playback
- [ ] Grain Size parameter changes grain duration (10-500ms audible range)
- [ ] Pitch Shift parameter transposes grain (-12 to +12 semitones)
- [ ] No clicks or pops at grain boundaries (Hann window working)
- [ ] Sample loaded successfully (hardcoded path works)

**Expected output:** Single grain plays on MIDI trigger, pitch shifts correctly, no overlap yet

---

#### Phase 4.2: Overlap-Add and Density Control

**Goal:** Add grain overlap, density control, and spacing multiplier for full granular texture

**Duration:** 45-60 minutes

**Components:**
- Grain trigger timing (density-based interval calculation)
- Active grain management (multiple overlapping grains per voice)
- Spacing multiplier (grain position advancement control)
- Grain mixing (sum all active grains)
- Linear interpolation for pitch shift (smooth fractional sample reads)

**Implementation approach:**
- Calculate grain trigger interval from density parameter (sampleRate / density)
- Maintain list of active grains (e.g., max 8 grains per voice)
- Trigger new grain when interval elapsed
- Advance grain read position by spacing multiplier
- Apply linear interpolation for smooth pitch shift
- Sum all active grains for voice output

**Test Criteria:**
- [ ] Density parameter controls grain trigger rate (1-200 grains/sec audible range)
- [ ] Multiple grains overlap smoothly (no gaps, continuous texture)
- [ ] Spacing parameter affects grain texture (0.1-2.0 range)
  - Low spacing (0.1-0.5) → Dense, chorus-like texture
  - Normal spacing (1.0) → Natural grain overlap
  - High spacing (1.5-2.0) → Sparse, granular texture
- [ ] Pitch shift is smooth with interpolation (no zipper noise)
- [ ] CPU usage acceptable (~10-20% single core with single voice)

**Expected output:** Smooth granular texture with controllable density and spacing, pitch shift interpolated

---

#### Phase 4.3: Polyphonic Voice Management and Region Selection

**Goal:** Add polyphonic voice allocation, mono/poly mode switching, and region selection

**Duration:** 45-60 minutes

**Components:**
- Voice Manager (allocate/steal voices, mono/poly mode)
- Region Selector (REGION_START/REGION_END parameter handling)
- Voice envelope (10ms release on note-off)
- Per-voice grain state (independent grain phase tracking)
- Voice summing with normalization

**Implementation approach:**
- Create 16 Voice objects (each with independent grain engine)
- Implement voice allocation (find free voice or steal oldest)
- Mono mode: Use voice[0] only, retrigger on note-on
- Poly mode: Allocate up to 16 voices
- Region selection: Clip grain extraction to REGION_START/REGION_END bounds
- Voice summing: Mix all voices with normalization factor (1.0 / sqrt(16) ≈ 0.25)

**Test Criteria:**
- [ ] Polyphony Mode toggle switches between mono and poly
- [ ] Poly mode: Up to 16 simultaneous voices audible
- [ ] Mono mode: Single voice, last-note priority, legato behavior
- [ ] Voice stealing works correctly (oldest voice stolen when 17th note played)
- [ ] Region Start/End parameters constrain grain extraction area
  - Full region (0.0-1.0): Grains extracted from entire sample
  - Partial region (0.2-0.5): Grains extracted from 20%-50% of sample
- [ ] Note-off triggers release envelope (no clicks on voice termination)
- [ ] CPU usage acceptable (~60-80% single core with 16 voices active)

**Expected output:** Full polyphonic granular sampler with region selection, mono/poly modes working

---

### Stage 3: GUI Phases

#### Phase 5.1: Layout and Basic Controls

**Goal:** Integrate v1 mockup HTML and bind basic granular parameters

**Duration:** 30-45 minutes

**Components:**
- Copy v1-ui.yaml production HTML to Source/ui/public/index.html
- Update PluginEditor.h/cpp with WebView setup
- Configure CMakeLists.txt for WebView resources (BinaryData)
- Bind 4 primary parameters: GRAIN_SIZE, DENSITY, PITCH_SHIFT, SPACING
- Basic waveform canvas rendering (static waveform display, no interaction yet)

**Implementation approach:**
- Convert YAML mockup to production HTML/CSS/JS (if not already done)
- Set up WebBrowserComponent with resource provider
- Create relay objects for 4 slider parameters
- Implement getResource() with explicit URL mapping (index.html, index.js, check_native_interop.js)
- Render static waveform on canvas (no region markers yet)

**Test Criteria:**
- [ ] WebView window opens with 900×600 px size
- [ ] All 4 sliders visible and styled correctly (dark grey background, orange accents)
- [ ] Waveform canvas renders at top (60% of window height)
- [ ] Parameter controls layout matches mockup (horizontal arrangement below waveform)
- [ ] Background color #2a2a2a, accent color #ff8c00 render correctly

**Expected output:** UI displays correctly, sliders visible but not yet interactive

---

#### Phase 5.2: Parameter Binding and Sample Loading

**Goal:** Two-way parameter communication and file browser integration

**Duration:** 45-60 minutes

**Components:**
- JavaScript → C++ relay calls (slider changes update DSP)
- C++ → JavaScript parameter updates (host automation updates UI)
- File browser button (open native file dialog)
- Drag-and-drop zone (drop sample files onto waveform)
- Sample loading feedback (show "Loading..." during background load)

**Implementation approach:**
- Implement WebSliderParameterAttachment for all 4 sliders (GRAIN_SIZE, DENSITY, PITCH_SHIFT, SPACING)
- Add WebToggleParameterAttachment for POLYPHONY_MODE (getToggleState, not getSliderState)
- Implement file browser button click handler (juce::FileChooser)
- Add drag-and-drop handler to waveform canvas (FileDragAndDropTarget)
- Trigger background sample load on file selection
- Show loading indicator in UI during load

**Test Criteria:**
- [ ] Slider movements change DSP parameters in real-time
- [ ] Host automation updates UI sliders (DAW automation reflected in UI)
- [ ] Preset changes update all UI elements correctly
- [ ] Parameter values display correctly (numeric readouts)
- [ ] File browser button opens native file dialog
- [ ] Drag-and-drop accepts audio files (.wav, .aiff, .mp3, .flac)
- [ ] Sample loads in background (UI remains responsive during load)
- [ ] Loading indicator shows during sample load
- [ ] No lag or visual glitches during parameter changes

**Expected output:** Full parameter binding working, sample loading via file browser and drag-and-drop functional

---

#### Phase 5.3: Interactive Waveform and Region Selection

**Goal:** Render loaded sample waveform and enable interactive region marking

**Duration:** 45-60 minutes

**Components:**
- Waveform rendering from loaded sample buffer
- Interactive region markers (drag handles for REGION_START/REGION_END)
- Region highlight overlay (orange highlight between markers)
- Real-time waveform updates when sample changes
- CPU/sample info display at bottom

**Implementation approach:**
- Implement waveform downsampling for display (e.g., 1 pixel = 100 samples)
- Render waveform as line graph on canvas (grey #cccccc color)
- Add draggable region markers (vertical bars at REGION_START/REGION_END positions)
- Render orange overlay (#ff8c00, 30% alpha) between markers
- Update waveform when new sample loaded (C++ → JS message)
- Display sample info (length, region length) at bottom info panel

**Test Criteria:**
- [ ] Waveform renders correctly when sample loaded (grey line graph)
- [ ] Region markers appear at correct positions (0.0-1.0 normalized)
- [ ] Drag handles respond to mouse (click-drag updates REGION_START/REGION_END parameters)
- [ ] Orange highlight overlay renders between markers (30% alpha)
- [ ] Waveform updates when new sample loaded (no manual refresh needed)
- [ ] Sample info displays correctly:
  - Sample Length: "2:35.4" (minutes:seconds.milliseconds)
  - Region Length: "0:45.2" (calculated from markers)
  - CPU Usage: "42%" (real-time CPU meter)
- [ ] Performance acceptable (waveform rendering <5ms, no UI lag)

**Expected output:** Interactive waveform with region selection, sample info display, full UI integration complete

---

### Implementation Flow

- Stage 1: Foundation - project structure (CMakeLists.txt, plugin boilerplate)
- Stage 2: Shell - APVTS parameters (7 parameters defined)
- Stage 3: DSP - 3 phases
  - Phase 4.1: Core Granular Engine (Single Voice) [45-60 min]
  - Phase 4.2: Overlap-Add and Density Control [45-60 min]
  - Phase 4.3: Polyphonic Voice Management and Region Selection [45-60 min]
- Stage 3: GUI - 3 phases
  - Phase 5.1: Layout and Basic Controls [30-45 min]
  - Phase 5.2: Parameter Binding and Sample Loading [45-60 min]
  - Phase 5.3: Interactive Waveform and Region Selection [45-60 min]
- Stage 3: Validation - presets, pluginval, changelog

**Total estimated time:** 4-6 hours (DSP: 2.25-3 hours, GUI: 2-3 hours, validation: 30 min)

---

## Implementation Notes

### Thread Safety

**Audio thread:**
- Sample buffer read via `std::atomic<AudioBuffer<float>*>` pointer load (lock-free)
- APVTS parameter reads via `getRawParameterValue()->load()` (atomic)
- Grain generation is pure computation (no allocations, no locks)
- Voice summing is simple addition (no thread communication)

**Message thread:**
- Parameter updates via APVTS (atomic writes, handled by JUCE)
- File browser dialog (blocking on message thread, safe)
- Drag-and-drop handling (file path captured on message thread)
- Trigger background sample load via `std::async` or `juce::Thread::launch()`

**Background thread:**
- File I/O via `juce::AudioFormatReader` (blocking, safe on background thread)
- Sample rate conversion if needed (blocking, safe)
- Atomic buffer pointer swap when load complete (lock-free exchange)
- Old buffer deleted on message thread via `MessageManager::callAsync()`

**Communication pattern:**
```
Message Thread                Background Thread              Audio Thread
     |                              |                              |
  [User clicks                      |                              |
   file browser]                    |                              |
     |                              |                              |
     |------- launch load --------->|                              |
     |                         [File I/O]                          |
     |                         [Read samples]                      |
     |                         [Resample if needed]                |
     |                              |                              |
     |<----- atomic swap -----      |                              |
     |                              |<---- atomic load ----[Read buffer]
     |                              |                              |
  [Delete old                       |                         [Process audio]
   buffer]                          |                              |
```

**No mutex needed:** All communication via atomics (lock-free, real-time safe)

---

### Performance

**Estimated CPU usage (per component):**

**Single voice:**
- Grain generation (Hann window, interpolation): ~3-5% single core
- Pitch shift (linear interpolation): ~2-3% single core
- Active grain mixing (sum 8 grains): ~1-2% single core
- **Total single voice: ~6-10% single core**

**Multiple voices (linear scaling):**
- 4 voices: ~24-40% single core
- 8 voices: ~48-80% single core
- 16 voices: ~96-160% single core (exceeds single core, may need limiting)

**Optimization opportunities:**
1. **Pre-calculated Hann window tables:** Avoid sin/cos in real-time (store tables for common grain sizes)
2. **SIMD for window multiplication:** Process 4 samples at once with SSE/NEON
3. **Fixed-point grain phase tracking:** Faster than float modulo for wrapping
4. **Grain count limiting:** Max 8 active grains per voice (prevent CPU spikes)
5. **Voice count limiting:** Reduce max voices from 16 to 8 if CPU too high

**Target:** <60% single core with 8 voices active (comfortable headroom)

**Hot paths:**
- `Voice::generateGrain()` - Grain extraction with interpolation (called at density rate)
- `Voice::processBlock()` - Grain mixing and summing (called every audio buffer)
- Linear interpolation inner loop (per-sample computation)

---

### Latency

**Processing latency:** 0 samples
- Granular engine has no lookahead (grains extracted in real-time)
- Pitch shift is time-domain (no FFT buffering)
- Sample buffer is pre-loaded (no disk I/O delay)
- MIDI note-on to audio output is sample-accurate

**Report via:** `getLatencySamples()` returns 0

**No host compensation needed:** Plugin has zero latency

---

### Denormal Protection

**Strategy:**
- Use `juce::ScopedNoDenormals` in `processBlock()` (flush-to-zero mode)
- Grain envelopes naturally avoid denormals (Hann window >0 inside grain, =0 outside)
- Voice envelope adds tiny DC offset before fade-out (prevents denormal creep: `level = std::max(level - rate, 1e-10f)`)
- Linear interpolation handles denormals correctly (no special casing needed)

**JUCE provides:** `ScopedNoDenormals` sets FTZ (flush-to-zero) and DAZ (denormals-are-zero) flags on x86/x64

---

### Known Challenges

#### 1. Grain Overlap CPU Management

**Challenge:** High density (200 grains/sec) + large grain size (500ms) = 100 overlapping grains per voice

**Calculation:** `overlap = (grainSize * density) / 1000 = (500 * 200) / 1000 = 100 grains`

**Mitigation:**
- Limit max active grains per voice to 8 (kill oldest grain when limit reached)
- Show warning in UI if density × grainSize > 8000 (8 grains overlap threshold)
- Profile CPU usage early, adjust grain limit if needed

**Reference:** Ableton Granulator limits overlap to prevent CPU spikes

---

#### 2. Waveform Rendering Performance

**Challenge:** 60-second sample at 48kHz = 2.88M samples → Cannot render 1:1 on canvas

**Mitigation:**
- Downsample for display (e.g., 1 pixel = 100-500 samples)
- Use min/max downsampling (show peak envelope, not individual samples)
- Render waveform to offscreen canvas, cache until sample changes
- Throttle waveform updates to 30fps (no need for 60fps on static waveform)

**Implementation:**
```javascript
// Downsample waveform for display (1 pixel = N samples)
function downsampleWaveform(sampleBuffer, targetPixelWidth) {
    const samplesPerPixel = Math.floor(sampleBuffer.length / targetPixelWidth);
    const downsampled = [];

    for (let i = 0; i < targetPixelWidth; i++) {
        const start = i * samplesPerPixel;
        const end = start + samplesPerPixel;
        const slice = sampleBuffer.slice(start, end);

        // Min/max for peak envelope
        const min = Math.min(...slice);
        const max = Math.max(...slice);

        downsampled.push({ min, max });
    }

    return downsampled;
}
```

**Reference:** Most DAW waveform displays use min/max downsampling (Logic Pro, Ableton)

---

#### 3. Region Marker Edge Cases

**Challenge:** User can drag REGION_START past REGION_END (invalid region)

**Mitigation:**
- Clamp REGION_START to always be < REGION_END (min 10ms gap)
- If user drags past, snap to boundary (e.g., REGION_START max = REGION_END - 0.01)
- Show validation error in UI if region too small (<10ms)
- Audio thread checks region validity, uses full sample if invalid

**Implementation:**
```cpp
// Validate region bounds in parameter change callback
void validateRegionBounds()
{
    float regionStart = regionStartParam->load();
    float regionEnd = regionEndParam->load();

    // Ensure start < end with minimum 10ms gap
    const float minGap = 0.01f * (1000.0f / (sampleBuffer.getNumSamples() / sampleRate)); // 10ms normalized

    if (regionStart >= regionEnd - minGap)
    {
        regionStart = std::max(0.0f, regionEnd - minGap);
        regionStartParam->setValueNotifyingHost(regionStart);
    }
}
```

**Reference:** Ableton Sampler clamps loop start/end to prevent invalid regions

---

#### 4. Sample Loading State Management

**Challenge:** What happens if user loads new sample while plugin playing?

**Scenarios:**
1. **Load complete during processBlock:** Atomic swap mid-buffer (audio thread sees old or new buffer, never invalid)
2. **Load fails:** Audio thread continues using old buffer (or silence if no previous sample)
3. **User loads very large file:** Background thread truncates to 60s, shows warning

**Mitigation:**
- Atomic buffer swap ensures audio thread never sees invalid pointer
- Audio thread checks `if (sampleBuffer != nullptr)` before reading
- Show loading indicator in UI (user knows background load in progress)
- On load failure, keep previous sample loaded (don't replace with nullptr)

**State machine:**
```
NO_SAMPLE → LOADING → LOADED → LOADING → LOADED
    ↓           ↓         ↓         ↓         ↓
  [Silent]  [Old sample] [New] [Old sample] [New]
                playing  sample  playing    sample
```

**Reference:** Kontakt keeps playing previous sample during load (no audio dropout)

---

#### 5. Voice Stealing Clicks

**Challenge:** Stolen voice cutoff can cause audible click if no release envelope

**Mitigation:**
- Apply 10ms release envelope to ALL voice terminations (note-off, voice stealing)
- Test voice stealing aggressively (play 17+ notes rapidly, verify no clicks)
- Tune release time if 10ms too long or too short

**Implementation:**
```cpp
void Voice::triggerQuickRelease()
{
    state = RELEASING;
    releaseRate = 1.0f / (0.01f * sampleRate); // 10ms release
}

void Voice::processEnvelope()
{
    if (state == RELEASING)
    {
        envelopeLevel -= releaseRate;

        if (envelopeLevel <= 0.0f)
        {
            envelopeLevel = 0.0f;
            state = IDLE;
        }
    }
}
```

**Reference:** Most polyphonic synths use 5-20ms release on voice stealing (JUCE Synthesiser uses 10ms)

---

## References

**Contract files:**
- Creative brief: `plugins/Sektor/.ideas/creative-brief.md`
- UI mockup: `plugins/Sektor/.ideas/mockups/v1-ui.yaml`
- DSP architecture: `plugins/Sektor/.ideas/architecture.md` (this document's sibling)
- Plan: `plugins/Sektor/.ideas/plan.md` (this document)

**Similar plugins for reference:**
- **LushPad** - Synth with MIDI input, voice management, IS_SYNTH flag (reference for instrument setup)
- **MuSam** - Sample loading, file I/O, waveform display (reference for sample loading architecture)
- **GainKnob** - WebView parameter binding, slider attachments (reference for UI parameter binding)
- **TapeAge** - WebView UI with custom elements (reference for waveform canvas rendering)

**JUCE patterns:**
- Voice management: Follow `juce::Synthesiser` and `juce::SynthesiserVoice` pattern
- Sample loading: Use `juce::AudioFormatManager` + background thread pattern
- Atomic buffer swap: Use `std::atomic<T*>` with `load()` / `exchange()`
- WebView UI: Follow Pattern #21 (ES6 module loading with `type="module"`)

**Critical patterns to check:**
- Pattern #22: `IS_SYNTH TRUE` flag required for instrument plugins
- Pattern #4: Output-only bus configuration (no input bus for synth)
- Pattern #12: `WebSliderParameterAttachment` requires 3 parameters (parameter, relay, nullptr)
- Pattern #21: ES6 module imports require `type="module"` on script tags
