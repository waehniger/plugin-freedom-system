# Stage 3 (GUI) Integration Checklist - Sektor v1

**Plugin:** Sektor (MIDI Granular Sampler)
**Mockup Version:** v1
**Generated:** 2025-11-18
**Window Size:** 900×600 px (fixed, non-resizable)
**Parameters:** 7 total (4 sliders, 2 region markers, 1 toggle)

---

## Overview

This checklist guides integration of the Sektor v1 WebView UI during Stage 3 (GUI) implementation. The UI features:

- **Interactive waveform display** with region marking
- **Sample loading** (drag & drop + file browser)
- **7 granular parameters** with real-time control
- **CPU usage monitoring**
- **Dark grey (#2a2a2a) background** with orange (#ff8c00) accents

---

## 1. Copy UI Files

### 1.1 Copy Production HTML
- [ ] Copy `v1-ui.html` to `Source/ui/public/index.html`
- [ ] Verify file exists: `ls Source/ui/public/index.html`

### 1.2 Copy JUCE Frontend Library
- [ ] Copy JUCE frontend library to `Source/ui/public/js/juce/index.js`
- [ ] Source: `/Applications/JUCE/modules/juce_gui_basics/native/javabridge/index.js`
- [ ] Verify file exists: `ls Source/ui/public/js/juce/index.js`

### 1.3 Copy Native Interop Check (REQUIRED)
- [ ] Copy `check_native_interop.js` to `Source/ui/public/js/juce/`
- [ ] Source: Working plugin (GainKnob, TapeAge, etc.) or JUCE examples
- [ ] Verify file exists: `ls Source/ui/public/js/juce/check_native_interop.js`
- [ ] **Missing this file causes frozen UI** (no parameter binding)

### 1.4 Create Directory Structure
```bash
mkdir -p Source/ui/public/js/juce
```

---

## 2. Update PluginEditor Files

### 2.1 Replace PluginEditor.h
- [ ] Backup existing: `cp Source/PluginEditor.h Source/PluginEditor.h.bak`
- [ ] Copy template content from `v1-PluginEditor.h`
- [ ] Update class name: `PluginEditor` → `SektorAudioProcessorEditor`
- [ ] Verify member order: relays → webView → attachments
- [ ] Verify 7 relay declarations (grain_size, density, pitch_shift, spacing, region_start, region_end, polyphony_mode)
- [ ] Verify 7 attachment declarations (matching relay types)

### 2.2 Replace PluginEditor.cpp
- [ ] Backup existing: `cp Source/PluginEditor.cpp Source/PluginEditor.cpp.bak`
- [ ] Copy template content from `v1-PluginEditor.cpp`
- [ ] Update class name: `PluginEditor` → `SektorAudioProcessorEditor`
- [ ] Verify initialization order matches declaration order
- [ ] Verify all 7 relays created BEFORE WebView
- [ ] Verify WebView created with all 7 `.withOptionsFrom()` calls
- [ ] Verify all 7 attachments created AFTER WebView
- [ ] Verify window size: `setSize(900, 600)`
- [ ] Verify `setResizable(false, false)` (fixed window)

### 2.3 Critical Pattern Verification
- [ ] **Member order:** relays → webView → attachments (lines 75-115 in .h)
- [ ] **Third parameter:** All attachments have `nullptr` as third parameter (JUCE 8 requirement)
- [ ] **Explicit URL mapping:** getResource() uses explicit `if (url == ...)` (not loops)
- [ ] **MIME types:** `application/javascript` for .js files (not `text/javascript`)

---

## 3. Update CMakeLists.txt

### 3.1 Append WebView Configuration
- [ ] Add `include(v1-CMakeLists.txt)` at end of main CMakeLists.txt
- [ ] Verify `juce_add_binary_data(Sektor_UIResources ...)` block present
- [ ] Verify all 3 UI files listed in SOURCES:
  - `Source/ui/public/index.html`
  - `Source/ui/public/js/juce/index.js`
  - `Source/ui/public/js/juce/check_native_interop.js`

### 3.2 Add WebView Module (CRITICAL)
- [ ] Verify `target_link_libraries(Sektor PRIVATE juce::juce_gui_extra)`
- [ ] **Missing this causes:** "WebBrowserComponent not found" compile error

### 3.3 Add WebView Compile Definitions
- [ ] Verify `JUCE_WEB_BROWSER=1` in `target_compile_definitions()`
- [ ] Verify `JUCE_USE_CURL=0` in `target_compile_definitions()`

### 3.4 Add NEEDS_WEB_BROWSER Flag (CRITICAL)
- [ ] Locate `juce_add_plugin(Sektor ...)` in main CMakeLists.txt
- [ ] Add `NEEDS_WEB_BROWSER TRUE` to plugin configuration
- [ ] **Missing this causes:** VST3 builds but won't load in DAW

### 3.5 Verify IS_SYNTH Flag (Instrument Requirement)
- [ ] Verify `IS_SYNTH TRUE` in `juce_add_plugin()` (REQUIRED for instruments)
- [ ] Verify `NEEDS_MIDI_INPUT TRUE` in `juce_add_plugin()`
- [ ] **Missing this causes:** No audio output (MIDI not routed)

---

## 4. Update PluginProcessor

### 4.1 Verify Bus Configuration (Instrument Pattern)
- [ ] Open `Source/PluginProcessor.cpp`
- [ ] Verify constructor uses output-only bus:
```cpp
SektorAudioProcessor::SektorAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // ...
}
```
- [ ] **NO input bus** (instruments generate audio from MIDI)

### 4.2 Verify Parameter IDs Match
- [ ] Verify APVTS parameter IDs match relay names:
  - `grain_size` (Float, 10-500 ms, default 100)
  - `density` (Float, 1-200 grains/sec, default 50)
  - `pitch_shift` (Float, -12 to +12 semitones, default 0)
  - `spacing` (Float, 0.1-2.0, default 1.0)
  - `region_start` (Float, 0.0-1.0, default 0.0)
  - `region_end` (Float, 0.0-1.0, default 1.0)
  - `polyphony_mode` (Bool, default true)

---

## 5. Build and Test (Debug)

### 5.1 Clean Build
```bash
cd plugins/Sektor
rm -rf build
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
```

### 5.2 Verify Build Output
- [ ] Build succeeds without warnings
- [ ] Verify binary exists: `ls build/Sektor_artefacts/Debug/VST3/Sektor.vst3`
- [ ] Verify binary exists: `ls build/Sektor_artefacts/Debug/AU/Sektor.component`
- [ ] Verify binary exists: `ls build/Sektor_artefacts/Debug/Standalone/Sektor.app`

### 5.3 Test Standalone (Quick Validation)
```bash
open build/Sektor_artefacts/Debug/Standalone/Sektor.app
```
- [ ] Window opens at 900×600 px
- [ ] WebView loads (not blank screen)
- [ ] Waveform display visible (mock sample loads)
- [ ] All 4 sliders visible and labeled
- [ ] Polyphony toggle visible
- [ ] Sample drop zone visible
- [ ] Right-click → Inspect works (debug mode only)
- [ ] Console shows: "Sektor UI initialized (standalone mockup)"
- [ ] Console shows NO JavaScript errors

### 5.4 Verify JUCE Backend Connection
- [ ] Open browser console (Right-click → Inspect)
- [ ] Check: `window.__JUCE__` object exists
- [ ] Check: `window.__JUCE__.backend` exists
- [ ] Check: Console shows "JUCE backend connected"

---

## 6. Build and Test (Release)

### 6.1 Clean Release Build
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### 6.2 Install to System Folders (REQUIRED for DAW testing)
```bash
./scripts/build-and-install.sh Sektor
```
- [ ] Script completes without errors
- [ ] VST3 installed: `ls ~/Library/Audio/Plug-Ins/VST3/Sektor.vst3`
- [ ] AU installed: `ls ~/Library/Audio/Plug-Ins/Components/Sektor.component`
- [ ] Plugins code-signed (script does this automatically)
- [ ] Caches cleared (script does this automatically)

### 6.3 Test Plugin Reload (Member Order Validation)
- [ ] Restart DAW (REQUIRED after cache clear)
- [ ] Load Sektor in empty project
- [ ] Close plugin window
- [ ] Reopen plugin window
- [ ] Repeat 10 times (test member order robustness)
- [ ] **No crashes** (validates member order correctness)

---

## 7. Test Parameter Binding

### 7.1 UI → DSP (User Interaction)
- [ ] Move Grain Size slider → parameter updates (check in DAW automation view)
- [ ] Move Density slider → parameter updates
- [ ] Move Pitch Shift slider → parameter updates
- [ ] Move Spacing slider → parameter updates
- [ ] Click Polyphony toggle → parameter updates
- [ ] Drag region start marker → parameter updates
- [ ] Drag region end marker → parameter updates

### 7.2 DSP → UI (Automation)
- [ ] Automate Grain Size in DAW → slider moves in UI
- [ ] Automate Density in DAW → slider moves in UI
- [ ] Automate Pitch Shift in DAW → slider moves in UI
- [ ] Automate Polyphony toggle in DAW → button state changes in UI

### 7.3 Preset Recall
- [ ] Save preset with custom parameter values
- [ ] Change parameters to different values
- [ ] Recall preset → UI updates to match saved values
- [ ] Verify all 7 parameters restore correctly

### 7.4 Parameter Persistence
- [ ] Set custom parameter values
- [ ] Close plugin window
- [ ] Reopen plugin window
- [ ] Verify UI reflects last known values (not defaults)

---

## 8. WebView-Specific Validation

### 8.1 CSS Constraints (CRITICAL)
- [ ] Open `Source/ui/public/index.html`
- [ ] Search for `100vh`, `100vw`, `100dvh`, `100svh`
- [ ] **Result:** NONE found (viewport units break WebView first render)
- [ ] Verify `html, body { height: 100%; }` present (correct pattern)

### 8.2 Native Feel CSS
- [ ] Verify `user-select: none` present
- [ ] Verify `-webkit-user-select: none` present
- [ ] Verify context menu disabled in JavaScript
- [ ] Test: Right-click on UI → no context menu appears

### 8.3 Resource Provider Validation
- [ ] Open browser console in standalone
- [ ] Check Network tab (if available)
- [ ] Verify no 404 errors for resources
- [ ] Verify `/index.html` loads
- [ ] Verify `/js/juce/index.js` loads
- [ ] Verify `/js/juce/check_native_interop.js` loads

### 8.4 MIME Type Validation
- [ ] Open `Source/PluginEditor.cpp`
- [ ] Verify `getResource()` returns correct MIME types:
  - `index.html` → `"text/html"`
  - `index.js` → `"application/javascript"` (NOT `text/javascript`)
  - `check_native_interop.js` → `"application/javascript"`

---

## 9. Sample Loading (Future Implementation)

**Note:** Full sample loading requires DSP implementation (Stage 2). For now, verify UI handles mock sample:

- [ ] Drop zone displays "Drop Sample Here" message
- [ ] Drop zone shows hover state (orange border)
- [ ] Browse button visible and clickable
- [ ] Mock waveform displays on page load
- [ ] Sample length displays as "2.00 s" (mock sample)
- [ ] Region markers are draggable
- [ ] Region length updates when markers move

**Future Stage 2 integration:**
- Implement `AudioFormatManager` for file loading (WAV, AIFF, MP3)
- Implement `FileChooser` for browse button
- Send waveform data to WebView via `emitEventIfBrowserIsVisible()`
- Update region markers from DSP (sample position → normalized 0-1)

---

## 10. CPU Monitoring (Future Implementation)

**Note:** CPU monitoring requires DSP implementation. For now, verify UI displays mock data:

- [ ] CPU usage field visible in info row
- [ ] Mock CPU value updates every second
- [ ] Value displays as percentage (e.g., "8.2%")

**Future Stage 2 integration:**
- Implement `std::atomic<float> cpuUsage` in PluginProcessor
- Calculate CPU usage in `processBlock()`
- Send updates via timer in PluginEditor
- Update info display: `webView->emitEventIfBrowserIsVisible("cpuUpdate", ...)`

---

## Parameter Reference

| Parameter ID    | Type  | Range            | Default | UI Control      | Relay Type              | Attachment Type                    |
|-----------------|-------|------------------|---------|-----------------|-------------------------|------------------------------------|
| grain_size      | Float | 10.0 - 500.0 ms  | 100.0   | Slider          | WebSliderRelay          | WebSliderParameterAttachment       |
| density         | Float | 1.0 - 200.0 g/s  | 50.0    | Slider          | WebSliderRelay          | WebSliderParameterAttachment       |
| pitch_shift     | Float | -12.0 - +12.0 st | 0.0     | Slider          | WebSliderRelay          | WebSliderParameterAttachment       |
| spacing         | Float | 0.1 - 2.0        | 1.0     | Slider          | WebSliderRelay          | WebSliderParameterAttachment       |
| region_start    | Float | 0.0 - 1.0        | 0.0     | Drag handle     | WebSliderRelay          | WebSliderParameterAttachment       |
| region_end      | Float | 0.0 - 1.0        | 1.0     | Drag handle     | WebSliderRelay          | WebSliderParameterAttachment       |
| polyphony_mode  | Bool  | false/true       | true    | Toggle button   | WebToggleButtonRelay    | WebToggleButtonParameterAttachment |

---

## Common Issues

### Issue: Blank WebView (White Screen)
**Symptoms:** Plugin loads but UI is blank white screen
**Causes:**
1. Missing `NEEDS_WEB_BROWSER=TRUE` in CMakeLists.txt
2. Resource provider returning wrong MIME types
3. Viewport units (`100vh`) in CSS
4. Missing `juce::juce_gui_extra` module

**Resolution:**
1. Verify `NEEDS_WEB_BROWSER=TRUE` in `juce_add_plugin()`
2. Check MIME types in `getResource()` (use `application/javascript`)
3. Replace all viewport units with percentages
4. Add `juce::juce_gui_extra` to `target_link_libraries()`

### Issue: Frozen Knobs (No Parameter Updates)
**Symptoms:** UI displays correctly but sliders don't respond to drag
**Causes:**
1. Missing third parameter (`nullptr`) in attachment constructor (JUCE 8)
2. Missing `check_native_interop.js` file
3. Wrong parameter IDs (mismatch between relay and APVTS)

**Resolution:**
1. Verify all attachments have 3 parameters: `(parameter, relay, nullptr)`
2. Copy `check_native_interop.js` to `Source/ui/public/js/juce/`
3. Verify relay parameter IDs exactly match APVTS IDs (case-sensitive)

### Issue: Release Build Crashes on Plugin Reload
**Symptoms:** Debug builds work, release builds crash when closing/reopening plugin
**Cause:** Wrong member order (attachments before webView)

**Resolution:**
1. Open `Source/PluginEditor.h`
2. Verify order: relays → webView → attachments
3. NEVER put attachments before webView

### Issue: VST3 Missing in DAW (AU Works)
**Symptoms:** AU appears in DAW, VST3 doesn't (but VST3 binary exists)
**Cause:** Missing `NEEDS_WEB_BROWSER=TRUE` flag

**Resolution:**
1. Add `NEEDS_WEB_BROWSER TRUE` to `juce_add_plugin()` in CMakeLists.txt
2. Rebuild from clean state
3. Reinstall to system folders
4. Clear DAW caches (script does this)
5. Restart DAW

### Issue: No Audio Output (Plugin Loads)
**Symptoms:** Plugin appears in DAW instruments, loads UI, but produces no sound
**Cause:** Missing `IS_SYNTH=TRUE` flag (DAW treats as effect, doesn't route MIDI)

**Resolution:**
1. Add `IS_SYNTH TRUE` to `juce_add_plugin()` in CMakeLists.txt
2. Add `NEEDS_MIDI_INPUT TRUE` to `juce_add_plugin()`
3. Verify bus configuration (output-only, no input)
4. Rebuild and reinstall

---

## Integration Complete

Once all checklist items pass:

- [ ] All 7 parameters sync UI ↔ APVTS
- [ ] Automation works (DAW → UI updates)
- [ ] Preset recall works (UI reflects saved values)
- [ ] No crashes on plugin reload (10+ cycles)
- [ ] WebView renders correctly (no blank screen)
- [ ] No 404 errors in resource loading
- [ ] Fixed 900×600 px window size

**Next Step:** Proceed to DSP implementation (Stage 2) to add granular synthesis engine and sample loading.

---

**Generated:** 2025-11-18
**Version:** v1
**Plugin:** Sektor (MIDI Granular Sampler)
