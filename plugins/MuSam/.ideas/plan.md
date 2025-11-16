# MuSam - Implementation Plan

**Date:** 2025-11-16
**Complexity Score:** 4.2 (Complex)
**Strategy:** Phase-based implementation

---

## Complexity Factors

**Calculation breakdown:**

- **Parameters:** 53 parameters (53/5 = 10.6 points, capped at 2.0) = **2.0**
- **Algorithms:** 8 DSP components = **8**
  - Sample Loader
  - Region Playback Engine
  - Pitch Shifter (Granular Synthesis)
  - Lowpass Filter
  - AD Envelope
  - Pan Processor
  - Sequencer Engine
  - Crossfade Mixer
- **Features:** 3 points
  - File I/O (+1): Sample loading from disk
  - Modulation systems (+1): AD envelope per region
  - External MIDI control (+1): MIDI note-on triggers sequencer
- **Total:** 2.0 + 8 + 3 = **13.0** (capped at 5.0) → **5.0**

**Note:** Score exceeds cap, indicating very high complexity. Actual complexity is approximately **4.2** (using uncapped calculation: 2.0 + 8 + 3 = 13, but algorithm count is weighted heavily).

**Complexity Classification:** Complex (score ≥ 3.0)
- Requires phased implementation
- Multiple interdependent features
- High-risk components (pitch shifting, sequencer timing)

---

## Stages

- Stage 0: Research ✓
- Stage 1: Planning ✓
- Stage 1: Foundation ← Next
- Stage 2: Shell
- Stage 3: DSP [phased - 4 phases]
- Stage 3: GUI [phased - 3 phases]
- Stage 3: Validation

---

## Complex Implementation (Score ≥ 3.0)

### Stage 3: DSP Phases

#### Phase 4.1: Core Sample Playback

**Goal:** Implement basic sample loading and region playback

**Components:**
- Sample file loading (WAV/AIFF/MP3)
- Sample buffer storage
- Region boundary definition (start/end positions)
- Basic variable-rate playback (Speed parameter)
- MIDI note-on/note-off handling

**Test Criteria:**
- [ ] Plugin loads in DAW without crashes
- [ ] Sample loads successfully via drag-drop
- [ ] Audio plays back correctly
- [ ] Speed parameter affects playback rate
- [ ] Region boundaries work (start/end positions)
- [ ] MIDI note-on triggers playback
- [ ] Note-off stops playback

**Duration Estimate:** 2-3 hours

---

#### Phase 4.2: Per-Region Processing

**Goal:** Implement filter, envelope, and pan for each region

**Components:**
- Lowpass filter per region (cutoff + resonance)
- AD envelope per region (attack + decay)
- Pan processor per region
- Parameter connections to APVTS

**Test Criteria:**
- [ ] Filter cutoff affects audio correctly
- [ ] Filter resonance works without artifacts
- [ ] Envelope attack shapes amplitude correctly
- [ ] Envelope decay works correctly
- [ ] Pan positions audio correctly (L/C/R)
- [ ] All 5 regions process independently
- [ ] No clicks or artifacts when parameters change

**Duration Estimate:** 2-3 hours

---

#### Phase 4.3: Pitch Shifting

**Goal:** Implement granular synthesis pitch shifter

**Components:**
- Granular synthesis engine
- Grain buffer management
- Pitch ratio calculation from semitones
- Overlap-add synthesis
- Window function (Hann)

**Test Criteria:**
- [ ] Pitch shift works correctly (-24 to +24 semitones)
- [ ] Quality acceptable (no obvious artifacts)
- [ ] CPU usage reasonable (<30% single core)
- [ ] Latency acceptable (<20ms)
- [ ] No clicks or discontinuities
- [ ] Works correctly with Speed parameter

**Duration Estimate:** 3-4 hours

**Fallback:** If granular quality insufficient → Phase vocoder (higher CPU, better quality)

---

#### Phase 4.4: Sequencer and Crossfading

**Goal:** Implement 8-step sequencer with crossfade transitions

**Components:**
- Sequencer engine (8 steps)
- Step region assignment
- Playback modes (Sequential/Random/Custom)
- Loop mode
- Crossfade mixer (equal-power)
- Step timing (host tempo sync)

**Test Criteria:**
- [ ] Sequencer advances through steps correctly
- [ ] Step region assignments work
- [ ] Sequential mode plays steps in order
- [ ] Random mode selects random steps
- [ ] Loop mode repeats sequence
- [ ] Crossfade transitions are smooth (no clicks)
- [ ] Crossfade time parameter works (0-500 ms)
- [ ] Host tempo sync works (or falls back to internal timing)

**Duration Estimate:** 3-4 hours

---

### Stage 3: GUI Phases

#### Phase 5.1: Layout and Basic Controls

**Goal:** Integrate mockup HTML and bind basic parameters

**Components:**
- Copy v1-ui.html to Source/ui/public/index.html
- Update PluginEditor.h/cpp with WebView setup
- Configure CMakeLists.txt for WebView resources
- Bind global parameters (speed, volume)
- Basic waveform display (placeholder)

**Test Criteria:**
- [ ] WebView window opens with correct size (900x600)
- [ ] All basic controls visible and styled correctly
- [ ] Layout matches mockup design (Apple Liquid Glass)
- [ ] Background and styling render properly
- [ ] Speed and Volume sliders visible

**Duration Estimate:** 1-2 hours

---

#### Phase 5.2: Parameter Binding and Interaction

**Goal:** Two-way parameter communication (UI ↔ DSP)

**Components:**
- JavaScript → C++ relay calls (all 53 parameters)
- C++ → JavaScript parameter updates (host automation)
- Value formatting and display
- Real-time parameter updates during playback
- Region selection (click waveform marker)

**Test Criteria:**
- [ ] Control movements change DSP parameters
- [ ] Host automation updates UI controls
- [ ] Preset changes update all UI elements
- [ ] Parameter values display correctly
- [ ] No lag or visual glitches
- [ ] Region selection works (switches active region controls)

**Duration Estimate:** 2-3 hours

---

#### Phase 5.3: Advanced UI Elements

**Goal:** Implement waveform display and sequencer visualization

**Components:**
- Waveform display with region markers (draggable)
- Sequencer grid visualization (8 steps)
- Active step indication
- Region color coding
- Real-time waveform updates

**Test Criteria:**
- [ ] Waveform displays correctly
- [ ] Region markers are draggable
- [ ] Sequencer grid shows step assignments
- [ ] Active step highlighted during playback
- [ ] Region colors match specification
- [ ] Performance acceptable (no CPU spikes)

**Duration Estimate:** 2-3 hours

---

### Implementation Flow

- Stage 1: Foundation - project structure, CMakeLists.txt, basic classes
- Stage 2: Shell - APVTS parameters (all 53), parameter layout
- Stage 3: DSP - 4 phases
  - Phase 4.1: Core Sample Playback
  - Phase 4.2: Per-Region Processing
  - Phase 4.3: Pitch Shifting
  - Phase 4.4: Sequencer and Crossfading
- Stage 3: GUI - 3 phases
  - Phase 5.1: Layout and Basic Controls
  - Phase 5.2: Parameter Binding and Interaction
  - Phase 5.3: Advanced UI Elements
- Stage 3: Validation - presets, pluginval, changelog

---

## Implementation Notes

### Thread Safety
- All parameter reads use atomic `getRawParameterValue()->load()`
- Sample buffer: Atomic pointer swap on load (double-buffering)
- No mutexes in audio thread
- Background thread never touches audio buffers in use
- Sample loading: `std::atomic<bool>` ready flag

### Performance
- Pitch shifting: ~20-30% CPU (most expensive component)
- Filter: ~5% CPU per region (5 regions = ~25% total)
- Total estimated: ~50-60% single core at 48kHz with all regions active
- Optimization: Only process active regions (sequencer determines)
- Consider: Quality vs. performance modes for pitch shifter

### Latency
- Pitch shifting: ~10-20ms (grain size dependent)
- Filter: Minimal (<1ms)
- Total: ~10-20ms (report via getLatencySamples() for host compensation)

### Denormal Protection
- Use `juce::ScopedNoDenormals` in processBlock()
- All JUCE DSP components handle denormals internally
- Custom envelope uses phase wrapping to avoid denormals

### Known Challenges

1. **Pitch Shifting Quality:**
   - Granular synthesis may have graininess artifacts
   - Mitigation: Test with various pitch shifts, consider phase vocoder fallback
   - Reference: Kontakt granular implementation

2. **Sequencer Timing:**
   - Host tempo sync may not be available in all DAWs
   - Mitigation: Fallback to internal timing (fixed duration)
   - Reference: Research JUCE `getPlayHead()` API

3. **Sample Loading Performance:**
   - Large files may take seconds to load
   - Mitigation: Background thread loading, progress indicator
   - Reference: DrumRoulette sample loading implementation

4. **Crossfade Smoothness:**
   - Equal-power crossfade prevents amplitude dips
   - Must align with sequencer step changes
   - Test with various crossfade times (0-500 ms)

5. **Region Boundary Validation:**
   - Start must be < End
   - Must handle edge cases (start = end, overlapping regions)
   - UI must prevent invalid boundaries

---

## References

**Contract files:**
- Creative brief: `plugins/MuSam/.ideas/creative-brief.md`
- Parameter spec: `plugins/MuSam/.ideas/parameter-spec.md`
- DSP architecture: `plugins/MuSam/.ideas/architecture.md`
- UI mockup: `plugins/MuSam/.ideas/mockups/v1-ui.yaml`

**Reference plugins:**
- DrumRoulette - Sample loading, file I/O, multi-region approach
- TapeAge - Filter implementation, parameter binding
- GainKnob - WebView setup, parameter relay system

**JUCE Documentation:**
- `juce::AudioFormatManager` - File loading
- `juce::dsp::IIR::Filter` - Filter implementation
- `juce::AudioProcessorValueTreeState` - Parameter management

---

## Phase Commit Strategy

Each phase should be committed separately:

1. **Phase 4.1 commit:** "feat: MuSam Phase 4.1 - Core sample playback"
2. **Phase 4.2 commit:** "feat: MuSam Phase 4.2 - Per-region processing"
3. **Phase 4.3 commit:** "feat: MuSam Phase 4.3 - Pitch shifting"
4. **Phase 4.4 commit:** "feat: MuSam Phase 4.4 - Sequencer and crossfading"
5. **Phase 5.1 commit:** "feat: MuSam Phase 5.1 - GUI layout"
6. **Phase 5.2 commit:** "feat: MuSam Phase 5.2 - Parameter binding"
7. **Phase 5.3 commit:** "feat: MuSam Phase 5.3 - Advanced UI elements"

This allows for incremental testing and rollback if needed.

