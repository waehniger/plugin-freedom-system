# Sektor Notes

## Status
- **Current Status:** 🚧 Stage 0 (Research & Planning)
- **Version:** 1.0.0 (planned)
- **Type:** Instrument (Granular Sampler)
- **Complexity Score:** 3.8 (Complex - phased implementation)

## Lifecycle Timeline

- **2025-11-18 (Ideation):** Creative brief created - minimalistic granular sampler concept
- **2025-11-18 (Stage 0):** Research & Planning complete - Architecture and plan documented (Complexity 3.8)

## Known Issues

None (not yet implemented)

## Additional Notes

### Description

Sektor is a minimalistic granular sampler that allows users to load audio samples, mark regions interactively via waveform display, and play those regions via MIDI with full granular control. The plugin focuses on creative texture manipulation through grain size, density, pitch shifting, and spacing parameters.

### Parameters (7 total)

**Granular Controls:**
- **Grain Size:** 10-500 ms (default 100 ms) - Duration of each grain
- **Density:** 1-200 grains/sec (default 50) - Rate of grain generation
- **Pitch Shift:** -12 to +12 semitones (default 0) - Transposition via playback rate
- **Spacing:** 0.1-2.0 (default 1.0) - Grain position advancement multiplier

**Region Selection:**
- **Region Start:** 0.0-1.0 normalized (default 0.0) - Start position in sample
- **Region End:** 0.0-1.0 normalized (default 1.0) - End position in sample

**Voice Control:**
- **Polyphony Mode:** Mono/Poly toggle (default Poly) - Voice allocation mode

### DSP Architecture

**Core Components:**
- Granular engine with overlap-add synthesis
- Hann window for grain envelope (prevents clicks)
- Time-domain pitch shifting (playback rate adjustment, no FFT)
- 16-voice polyphonic voice management
- Atomic buffer swap for thread-safe sample loading
- Background thread file I/O (non-blocking)

**Technical Approach:**
- Time-domain granular synthesis (simpler than FFT-based)
- Linear interpolation for pitch shift (adequate quality for granular)
- Lock-free atomic pointer for sample buffer access
- Voice stealing with 10ms release envelope

**Estimated CPU:**
- Single voice: ~5-10% single core
- 8 voices: ~40-60% single core
- 16 voices: ~80-100% single core (max polyphony)

### GUI Design

**Layout:**
- Top section (60%): Interactive waveform display with region markers
- Bottom section (40%): 4 sliders (Grain Size, Density, Pitch Shift, Spacing)
- Sample loading: Drag-and-drop zone + file browser button
- Info panel: Sample length, region length, CPU usage

**Visual Style:**
- Dark grey background (#2a2a2a)
- Orange accent (#ff8c00) for highlights and region overlay
- Minimalist, function-first design
- 900×600 px fixed window size

**Interaction:**
- Click-drag on waveform to set region markers
- Drag handles for precise region adjustment
- Drag-and-drop audio files onto waveform
- File browser button for native file selection

### Implementation Strategy

**Phased approach (3.8 complexity score):**

**DSP Phases:**
- Phase 4.1: Core granular engine (single voice) - 45-60 min
- Phase 4.2: Overlap-add and density control - 45-60 min
- Phase 4.3: Polyphony and region selection - 45-60 min

**GUI Phases:**
- Phase 5.1: Layout and basic controls - 30-45 min
- Phase 5.2: Parameter binding and sample loading - 45-60 min
- Phase 5.3: Interactive waveform with region markers - 45-60 min

**Total estimated time:** 4-6 hours

### File I/O

**Supported formats:** WAV, AIFF, FLAC, MP3 (via macOS system codecs)

**Loading strategy:**
- Background thread loading (non-blocking)
- Atomic buffer pointer swap (lock-free)
- Sample rate conversion if needed (target = host rate)
- Maximum sample length: 60 seconds at 192kHz (~23MB stereo)

**Thread safety:**
- Audio thread: Lock-free atomic pointer load
- Background thread: File I/O and buffer creation
- Message thread: UI updates and old buffer deletion

### MIDI Handling

**Input:** Omni mode (all MIDI channels)

**Note mapping:**
- All MIDI notes (0-127): Trigger granular playback
- Velocity: Scales output gain (0-127 → 0.0-1.0)
- Pitch shift parameter: Transposes all notes equally

**Polyphony:**
- Mono mode: Single voice, last-note priority
- Poly mode: Up to 16 simultaneous voices
- Voice stealing: Oldest voice with 10ms release

### Critical Patterns Applied

**From juce8-critical-patterns.md:**
- Pattern #22: IS_SYNTH TRUE (instrument plugin)
- Pattern #4: Output-only bus (no input for synth)
- Pattern #12: WebSliderParameterAttachment 3-param constructor
- Pattern #21: ES6 module loading with type="module"

### Professional References

**Researched plugins:**
- Ableton Granulator II (overlap-add synthesis, spray parameter)
- Native Instruments Reaktor (time-domain pitch shift)
- Mutable Instruments Clouds (4-voice polyphony, Hann windowing)
- Output Portal (low-latency granular, 1-500ms grain size)

### Contract Files

- **Creative brief:** plugins/Sektor/.ideas/creative-brief.md
- **UI mockup:** plugins/Sektor/.ideas/mockups/v1-ui.yaml
- **DSP architecture:** plugins/Sektor/.ideas/architecture.md
- **Implementation plan:** plugins/Sektor/.ideas/plan.md

### Next Actions

1. Run `/implement Sektor` to start Stage 1 (Foundation + Shell)
2. Review architecture.md and plan.md before implementation
3. Expect 4-6 hour implementation time (phased approach)
