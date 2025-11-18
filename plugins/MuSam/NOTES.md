# MuSam Notes

## Status
- **Current Status:** 📦 Installed
- **Version:** 1.0.0
- **Type:** Sampler/Effect
- **Complexity Score:** 4.2 (Complex)
- **Implementation Strategy:** Phase-based (7 phases: 4 DSP + 3 GUI)

## Lifecycle Timeline
- **2025-11-17:** Reinstalled to system folders (VST3 + AU)

- **2025-11-16:** Plugin ideated - creative brief created
- **2025-11-16 (Stage 0):** Research & Planning complete - Architecture and plan documented (Complexity 4.2)
- **2025-11-16 (Stage 1):** Foundation + Shell complete - Build system created, all 53 parameters implemented with APVTS
- **2025-11-16 (Stage 2):** Audio Engine Working - All DSP components implemented (4 phases: Core Playback, Per-Region Processing, Pitch Shifting, Sequencer)
- **2025-11-16 (Stage 3):** UI Integrated - WebView UI complete with all 53 parameters bound (3 phases: Layout, Parameter Binding, Advanced UI Elements)
- **2025-11-16 (v1.0.0):** Validation complete - Semantic validation passed, plugin ready for installation
- **2025-11-16 (v1.0.0):** Installation complete - VST3 and AU installed to system folders (5.5MB each)

## Known Issues

None

## Additional Notes

**Description:**
Multi-region sample sequencer with crossfading playback and color-coded interface. Load a sample, define 5 regions with independent processing (pitch, filter, envelope, pan), sequence them through an 8-step visual grid with smooth crossfade transitions.

**Inspiration:**
- Spectres (Slate+Ash) - multi-sample approach
- Apple Liquid Glass aesthetic - visual design language

**Key Features:**
- 5 color-coded regions per sample
- Per-region parameters: Start/End, Pitch (-24 to +24 st), Lowpass Filter, AD Envelope, Pan
- Global parameters: Speed (0.5x-2x playback rate), Volume
- 8-step visual sequencer (regions can repeat)
- Crossfade playback mode with adjustable crossfade time
- Global loop mode
- Drag-and-drop sample loading

**Use Cases:**
- Glitch effects (rapid micro-region sequencing)
- Ambient textures (crossfading between tonal regions)
- Drum deconstruction (isolate and rearrange drum loop elements)
- Creative resampling (transform any audio into new material)

**UI Concept:**
- Central waveform display with 5 markable regions
- Color-coded interface (each region has distinct color that "illuminates" UI)
- Liquid Glass aesthetic (translucent panels, smooth gradients, depth)
- Single-click accessibility (no nested menus)
- 8-step grid sequencer below waveform

**Technical Considerations:**
- High-quality pitch shifting needed (granular or phase vocoder)
- Equal-power or linear crossfading between regions
- Classic lowpass filter with resonance
- Variable-rate playback engine (Speed affects tempo + pitch)

**Formats:** VST3, AU, Standalone

**Installation Locations:**
- VST3: `~/Library/Audio/Plug-Ins/VST3/MuSam.vst3` (5.5 MB)
- AU: `~/Library/Audio/Plug-Ins/Components/MuSam.component` (5.5 MB)
