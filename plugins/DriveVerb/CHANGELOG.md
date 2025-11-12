# Changelog

All notable changes to DriveVerb will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [1.0.0] - 2025-11-12

### Added

- Initial release
- Algorithmic reverb with independent size (0-100%) and decay (0.5-10s) control
- Warm tape saturation (0-24dB) applied only to wet signal using tanh waveshaping
- DJ-style filter (-100% to +100%) with exponential frequency mapping
  - Center position (0%) bypasses filter completely
  - Negative values: Low-pass filter (200Hz to 20kHz)
  - Positive values: High-pass filter (20Hz to 10kHz)
- PRE/POST routing toggle for filter placement relative to drive stage
  - PRE mode: Reverb → Filter → Drive (focused saturation on filtered range)
  - POST mode: Reverb → Drive → Filter (broader harmonics then shaped)
- 6 parameters: Size, Decay, Dry/Wet, Drive, Filter, Filter Position
- 5 factory presets showcasing plugin capabilities

### Audio Processing

- **Reverb:** juce::dsp::Reverb with size-controlled room dimensions and independent decay
- **Drive:** tanh() waveshaping for even harmonic saturation (tape-like warmth)
- **Filter:** 2nd-order Butterworth IIR filters with Q=0.707 for smooth response
- **Dry/Wet:** juce::dsp::DryWetMixer with equal-power crossfade
- **Signal Flow:** Conditional branching supports PRE/POST routing modes
- All DSP processing real-time safe with atomic parameter reads

### User Interface

- WebView-based UI (1000x500px) with vintage hardware aesthetic
- 5 interactive rotary knobs with relative drag control
- Vertical toggle switch for PRE/POST routing
- Animated VU meter showing drive saturation level
  - Ballistic motion (fast attack 0.4, slow decay 0.15)
  - Color zones: green/orange/red based on level thresholds
- Real-time parameter value displays with proper units (%, s, dB)
- Scan lines and vignette for retro hardware feel

### Validation

- Compiles successfully (VST3, AU, Standalone)
- Build time: ~20s
- All 6 parameters functional and audible
- PRE/POST modes produce distinct tonal character
- VU meter responds to real-time audio signal
- No crashes on plugin reload or parameter automation
- Installed to system folders (~/Library/Audio/Plug-Ins/)

### Technical Details

- JUCE 8.x compatible
- Modern juce::dsp pipeline (ProcessSpec, AudioBlock, ProcessContext)
- ES6 module imports in WebView JavaScript
- WebSliderParameterAttachment with three-parameter constructor (nullptr for undo manager)
- Explicit URL mapping in WebView resource provider
- Member initialization order: Relays → WebView → Attachments
