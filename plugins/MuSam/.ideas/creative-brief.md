# MuSam - Creative Brief

## Overview

**Type:** Sampler/Effect
**Core Concept:** Multi-region sample sequencer with crossfading playback and color-coded interface
**Status:** 💡 Ideated
**Created:** 2025-11-16

## Vision

MuSam is a creative multi-region sampler that transforms single audio samples into dynamic, evolving textures. Users can define up to 5 distinct regions within a loaded sample, each with independent processing parameters (pitch, filter, envelope, pan). These regions are then sequenced through an 8-step visual grid sequencer with smooth crossfade transitions, enabling everything from glitchy rhythmic patterns to flowing ambient soundscapes.

Inspired by Spectres (Slate+Ash), MuSam is designed as a true multitool - equally capable of deconstructing drum loops into new rhythmic variations, creating evolving pad textures from static recordings, or generating experimental glitch effects through creative region manipulation.

The interface centers around a waveform display where regions are visually marked and color-coded. Each region's color "illuminates" the interface like a light source, creating a dynamic visual feedback system that reflects which regions are currently active. The design follows Apple's Liquid Glass aesthetic - translucent, fluid, and immediately engaging.

## Parameters

### Global Parameters

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Speed | 0.5x - 2.0x | 1.0x | Playback rate (affects both tempo and pitch, tape-style) |
| Volume | -∞ to 0 dB | -6 dB | Master output volume |

### Per-Region Parameters (5 regions)

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Start | 0 - 100% | varies | Region start point in sample |
| End | 0 - 100% | varies | Region end point in sample |
| Pitch | -24 to +24 semitones | 0 | Pitch shift for this region |
| Filter Cutoff | 20 Hz - 20 kHz | 20 kHz | Lowpass filter cutoff frequency |
| Filter Resonance | 0 - 100% | 0% | Lowpass filter resonance/Q |
| Attack | 0 - 1000 ms | 10 ms | Envelope attack time |
| Decay | 0 - 5000 ms | 500 ms | Envelope decay time |
| Pan | -100% (L) to +100% (R) | 0% (Center) | Stereo positioning |

### Sequencer Parameters

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Sequence Pattern | 8 steps | 12345 | Which regions play in which step (visual grid) |
| Playback Mode | Sequential/Random/Custom | Sequential | How the sequence is traversed |
| Loop Mode | On/Off | On | Whether the sequence repeats (global) |
| Crossfade Time | 0 - 500 ms | 50 ms | Transition smoothness between regions |

## UI Concept

### Layout

**Central Element:** Large waveform display showing the loaded sample with 5 color-coded region markers that can be dragged to define start/end points.

**Region Controls:** Below the waveform, 5 color-coded control strips (one per region), each containing:
- Pitch slider
- Filter cutoff/resonance knobs
- Attack/Decay envelope controls
- Pan knob

**Sequencer:** 8-step grid below region controls where regions can be dragged/assigned to steps. Visual indication of current playback position.

**Global Controls:** Top bar with Speed and Volume controls, plus sample drag-drop zone.

### Visual Style

**Aesthetic:** Apple Liquid Glass - translucent, fluid, depth through layering and blur
- Frosted glass panels with subtle blur
- Smooth gradients and soft shadows
- Clean typography (SF Pro or similar)

**Color System:**
- Each region assigned a distinct color (e.g., cyan, magenta, yellow, green, orange)
- Active regions "illuminate" the interface - their colors glow and cast light on surrounding UI elements
- Inactive regions appear desaturated/dimmed
- Waveform display uses region colors to show which parts are mapped

**Key Elements:**
- **Waveform Display:** Central focus with region boundaries clearly visible
- **Color-coded regions:** Visual identity through consistent color theming
- **Step sequencer grid:** Visual drag-and-drop interface for pattern programming
- **Smooth animations:** Crossfade transitions reflected in UI (color blending, smooth step progression)

**Accessibility:** All controls accessible with a single click - no nested menus or hidden parameters. Everything visible at once.

## Use Cases

- **Glitch Effects:** Rapidly sequence through micro-regions of a sample with randomized playback for stuttering, granular effects
- **Ambient Textures:** Crossfade between 5 tonal regions of a pad recording to create evolving, breathing soundscapes
- **Drum Deconstruction:** Load a drum loop, isolate kick/snare/hat regions, rearrange into new rhythmic patterns via sequencer
- **Creative Resampling:** Transform any audio source (vocals, synths, field recordings) into new melodic/rhythmic material
- **Live Performance:** Program sequences in real-time, trigger variations with MIDI notes

## Inspirations

- **Spectres (Slate+Ash):** Multi-sample triggering and creative sound design approach
- **Apple Liquid Glass:** Visual design language - translucency, depth, fluid interactions
- **Hardware Samplers:** Classic MPC-style region/slice workflow updated for modern production

## Technical Notes

### DSP Considerations

- **Crossfading:** Smooth amplitude crossfades between regions (equal-power or linear based on Crossfade Time parameter)
- **Pitch Shifting:** High-quality time-domain pitch shifter needed (granular or phase vocoder approach)
- **Filter:** Classic lowpass (12 dB/oct or 24 dB/oct) with resonance control
- **Sample Playback:** Variable-rate playback engine (Speed parameter affects both tempo and pitch)
- **Envelope:** Simple AD envelope applied to each region's playback

### Playback Logic

- **Triggering:** Each MIDI note-on triggers the sequence from step 1
- **Sequencer:** 8 steps, each can reference any of the 5 regions (regions can repeat)
- **Crossfade Mode:** When transitioning between steps, crossfade between outgoing and incoming regions over Crossfade Time duration
- **Loop:** When Loop Mode enabled, sequence repeats continuously until note-off (or indefinitely depending on envelope behavior)

### Sample Loading

- Drag-and-drop single audio file (WAV/AIFF/MP3)
- Waveform analysis and display
- Default region distribution (5 equal slices as starting point)

## Next Steps

- [ ] Create UI mockup (`/dream MuSam` → option 3)
- [ ] Start implementation (`/implement MuSam`)
- [ ] Research pitch-shifting algorithms for quality vs. performance
