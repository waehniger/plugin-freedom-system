# Changelog

All notable changes to Sektor will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.1.0] - 2025-11-27

### Added
- **Playhead visualization**: Real-time playback position indicators on waveform display
  - Shows grain extraction positions for all active voices
  - Color-coded by region (matches region tab colors)
  - Smooth 30 FPS animation
  - Vertical lines with circle markers at top
  - Supports polyphonic visualization (up to 16 playheads simultaneously)

### Technical Details
- Added `PlayheadPosition` struct to PluginProcessor with normalized position, region index, and active state
- Implemented `getPlayheadPositions()` method to collect playhead data from all active voices
- Added `currentRegionIndex` tracking to Voice class to identify which region each voice is playing from
- Added Timer callback to PluginEditor running at 30 Hz for smooth playhead updates
- JavaScript receives playhead array via `window.updatePlayheads()` and renders on canvas
- Playheads drawn with region-specific colors (orange, blue, green, purple, cyan)

### Implementation Notes
- Playhead positions are sampled from audio thread via atomic operations (thread-safe)
- No impact on audio processing performance (read-only access to voice state)
- Playheads update independently of waveform rendering
- Animation runs on message thread via JUCE Timer (30 Hz = 33ms interval)

## [1.0.0] - 2025-11-18

### Initial Release
- Multi-region granular sampler (5 independent regions)
- MIDI-triggered granular synthesis
- Real-time waveform visualization with interactive region selection
- 7 parameters: Grain Size, Density, Pitch Shift, Spacing, Region Start/End (×5), Polyphony Mode
- 16-voice polyphonic engine with voice stealing
- Drag-and-drop sample loading
- File browser integration
- WebView-based UI (900×600 px)
- Supports WAV, AIFF, FLAC, MP3 formats
