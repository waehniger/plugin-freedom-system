# Stage 3 (GUI) Integration Checklist - MuSam v1

**Plugin:** MuSam
**Mockup Version:** v1
**Generated:** 2025-11-16
**Parameters:** 53 total (2 global + 40 region + 11 sequencer)
**Window Size:** 900x600 pixels

## Overview

This checklist guides gui-agent through integrating the finalized v1 mockup into MuSam during Stage 3 (GUI) implementation.

**CRITICAL:** Member order in PluginEditor.h MUST be: relays → webView → attachments (prevents release build crashes).

---

## Phase 1: Copy UI Files

### 1.1 Create Directory Structure

```bash
mkdir -p plugins/MuSam/Source/ui/public/js/juce
```

### 1.2 Copy HTML

```bash
cp plugins/MuSam/.ideas/mockups/v1-ui.html \
   plugins/MuSam/Source/ui/public/index.html
```

**Verify:**
- File size > 0 bytes
- No viewport units (100vh, 100vw)
- Native CSS present (user-select: none)
- No debug/test elements (removed parameter monitor)

### 1.3 Copy JUCE Frontend Library

**Source:** Copy from working plugin (e.g., GainKnob, TapeAge) or JUCE examples

```bash
cp path/to/reference/plugin/Source/ui/public/js/juce/index.js \
   plugins/MuSam/Source/ui/public/js/juce/index.js

cp path/to/reference/plugin/Source/ui/public/js/juce/check_native_interop.js \
   plugins/MuSam/Source/ui/public/js/juce/check_native_interop.js
```

**Verify:**
- `index.js` exports `getSliderState`, `getToggleState`, `getComboBoxState`
- `check_native_interop.js` exists (required for WebView initialization)
- Both files use ES6 module syntax

---

## Phase 2: Update PluginEditor Files

### 2.1 Replace PluginEditor.h

```bash
# Read template
cat plugins/MuSam/.ideas/mockups/v1-PluginEditor.h

# Manually integrate into:
plugins/MuSam/Source/PluginEditor.h
```

**Critical verifications:**

1. **Class name correct:**
   - Template uses `MuSamAudioProcessorEditor`
   - Must match processor's `createEditor()` return type

2. **Member order (CRITICAL):**
   ```cpp
   private:
       // 1. Relays (53)
       std::unique_ptr<juce::WebSliderRelay> speedRelay;
       // ... all 53 relays

       // 2. WebView (1)
       std::unique_ptr<juce::WebBrowserComponent> webView;

       // 3. Attachments (53)
       std::unique_ptr<juce::WebSliderParameterAttachment> speedAttachment;
       // ... all 53 attachments
   ```

3. **Count verification:**
   - Relay declarations: 53
   - Attachment declarations: 53
   - Counts MUST match

4. **Relay type mapping:**
   - Float parameters → `WebSliderRelay`
   - Bool parameters → `WebToggleButtonRelay`
   - Choice parameters → `WebComboBoxRelay`

### 2.2 Replace PluginEditor.cpp

```bash
# Read template
cat plugins/MuSam/.ideas/mockups/v1-PluginEditor.cpp

# Manually integrate into:
plugins/MuSam/Source/PluginEditor.cpp
```

**Critical verifications:**

1. **Initialization order matches declaration:**
   - Relays created FIRST
   - WebView created SECOND (with `.withOptionsFrom()` for all 53 relays)
   - Attachments created LAST

2. **Parameter ID matching:**
   - Relay IDs match YAML exactly (snake_case)
   - APVTS retrieval uses same IDs
   - Example: `"region_1_filter_cutoff"` (NOT `"region1FilterCutoff"`)

3. **Window size:**
   - `setSize(900, 600);` (from YAML dimensions)

4. **Resource provider:**
   - Serves `/` and `/index.html` → `BinaryData::index_html`
   - Serves `/js/juce/index.js` → `BinaryData::index_js`
   - Serves `/js/juce/check_native_interop.js` → `BinaryData::check_native_interop_js`
   - MIME type: `application/javascript` (NOT `text/javascript`)

---

## Phase 3: Update CMakeLists.txt

### 3.1 Append WebView Configuration

```bash
# Append v1-CMakeLists.txt content to:
plugins/MuSam/CMakeLists.txt
```

**Sections to add:**

1. **Binary data:**
   ```cmake
   juce_add_binary_data(MuSam_UIResources
       SOURCES
           Source/ui/public/index.html
           Source/ui/public/js/juce/index.js
           Source/ui/public/js/juce/check_native_interop.js
   )
   ```

2. **Link libraries:**
   ```cmake
   target_link_libraries(MuSam
       PRIVATE
           MuSam_UIResources
           juce::juce_gui_extra  # Required for WebView
   )
   ```

3. **Compile definitions:**
   ```cmake
   target_compile_definitions(MuSam
       PUBLIC
           JUCE_WEB_BROWSER=1
           JUCE_USE_CURL=0
   )
   ```

### 3.2 Verify juce_add_plugin() Configuration

**Ensure these flags are set:**

```cmake
juce_add_plugin(MuSam
    COMPANY_NAME "YourCompany"
    PLUGIN_MANUFACTURER_CODE Manu
    PLUGIN_CODE MuSa
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "MuSam"
    NEEDS_WEB_BROWSER TRUE    # CRITICAL for VST3
    IS_SYNTH TRUE             # MuSam is instrument
    NEEDS_MIDI_INPUT TRUE     # Accepts MIDI
)
```

---

## Phase 4: Build and Test (Debug)

### 4.1 Clean Build

```bash
rm -rf plugins/MuSam/build
mkdir plugins/MuSam/build
cd plugins/MuSam/build
cmake ..
```

**Expected output:**
- No errors about missing headers
- `juce_generate_juce_header(MuSam)` executed successfully
- Binary data registered (3 files)

### 4.2 Build Targets

```bash
cmake --build . --target MuSam_VST3 MuSam_AU MuSam_Standalone
```

**Expected output:**
- No compiler errors
- No warnings about member order
- Binaries created in `build/MuSam_artefacts/`

### 4.3 Test Standalone (Debug)

```bash
open build/MuSam_artefacts/Debug/Standalone/MuSam.app
```

**Verify:**
1. Window opens (900x600)
2. WebView loads (not blank screen)
3. Apple Liquid Glass styling visible
4. Right-click → Inspect works (check for errors in console)
5. Console shows: `"MuSam UI initialized - 53 parameters"`
6. `window.__JUCE__` object exists in console

**Debug JavaScript errors:**
```javascript
// In browser console (via Inspect):
window.__JUCE__.backend  // Should exist
Object.keys(window.__JUCE__.backend)  // Should list parameter methods
```

---

## Phase 5: Build and Test (Release)

### 5.1 Release Build

```bash
cmake --build . --config Release --target MuSam_VST3 MuSam_AU MuSam_Standalone
```

**Expected output:**
- Same as debug build
- No additional warnings

### 5.2 Test Release Stability

**CRITICAL TEST:** Member order verification (release builds expose this bug)

```bash
# Open and close Standalone 10 times rapidly
for i in {1..10}; do
    open build/MuSam_artefacts/Release/Standalone/MuSam.app
    sleep 2
    killall MuSam
done
```

**Expected:**
- No crashes on close
- No "evaluateJavascript on destroyed WebView" errors

**If crashes occur:**
- Member order is WRONG in PluginEditor.h
- Return to Phase 2.1, verify relays → webView → attachments

---

## Phase 6: Install and Test in DAW

### 6.1 Install to System Folders

```bash
cd /Users/willybeyer/Documents/Code/plugin-freedom-system
./scripts/build-and-install.sh MuSam
```

**Script performs:**
1. Builds fresh binaries (Release)
2. Removes old versions from system
3. Installs new versions to:
   - `~/Library/Audio/Plug-Ins/VST3/MuSam.vst3`
   - `~/Library/Audio/Plug-Ins/Components/MuSam.component`
4. Signs binaries (prevents "sealed resource" errors)
5. Clears DAW caches (Ableton DB, AU cache)
6. Verifies installation

### 6.2 Restart DAW

**CRITICAL:** DAW must restart to rescan plugins after cache clear.

```bash
killall "Ableton Live 12 Suite" "Logic Pro" 2>/dev/null
```

### 6.3 Test in DAW (VST3)

**Load plugin:**
1. Open Ableton/Logic/Reaper
2. Create instrument track
3. Load "MuSam" from VST3 instruments

**Verify:**
- Plugin appears in browser (not grayed out)
- UI loads correctly (Apple Liquid Glass styling)
- Window size 900x600
- No blank screen

**If plugin doesn't appear:**
- Check `NEEDS_WEB_BROWSER TRUE` in CMakeLists.txt
- Rebuild and reinstall
- Check scanner logs:
  ```bash
  tail -50 ~/Library/Preferences/Ableton/Live*/PluginScanner.txt | grep -i musam
  ```

### 6.4 Test in DAW (AU)

**Repeat tests from 6.3 with AU format.**

**AU-specific checks:**
- Component validation passes
- No "code signature invalid" errors
- Check AU cache:
  ```bash
  ls -la ~/Library/Caches/AudioUnitCache/ | grep -i musam
  ```

---

## Phase 7: Test Parameter Binding

### 7.1 Manual Parameter Testing

**For EACH of the 53 parameters:**

1. **UI → DAW:**
   - Move control in plugin UI
   - Verify DAW automation lane shows change
   - Verify value persists after closing/reopening

2. **DAW → UI:**
   - Draw automation in DAW
   - Play back
   - Verify UI control moves with automation

3. **Preset Recall:**
   - Set parameters to non-default values
   - Save preset
   - Load different preset
   - Load original preset
   - Verify all 53 parameters recall correctly

### 7.2 Parameter Groups to Test

**Global (2):**
- speed, volume

**Region 1-5 (8 each = 40):**
- Start/end markers (draggable in waveform)
- Pitch slider
- Filter cutoff/resonance knobs
- Attack/decay sliders
- Pan knob

**Sequencer (11):**
- 8 step region assignments (dropdowns)
- Playback mode (dropdown)
- Loop toggle
- Crossfade slider

### 7.3 Automation Recording

**Test automation:**
1. Enable write mode in DAW
2. Move multiple parameters while playing
3. Playback automation
4. Verify smooth parameter changes (no jumps/glitches)

---

## Phase 8: WebView-Specific Validation

### 8.1 CSS Validation

```bash
grep -r "100vh\|100vw\|100dvh\|100svh" plugins/MuSam/Source/ui/public/
```

**Expected output:** None (no viewport units)

**If found:** Replace with `100%` and `html, body { height: 100%; }`

### 8.2 Native Feel CSS

**Verify in index.html:**
```css
body {
    user-select: none;
    -webkit-user-select: none;
    -moz-user-select: none;
    cursor: default;
}
```

### 8.3 Resource Provider Validation

**Test in browser console (Inspect mode):**

```javascript
// All resources should return 200 OK
fetch('/').then(r => console.log('index.html:', r.status));
fetch('/js/juce/index.js').then(r => console.log('index.js:', r.status));
fetch('/js/juce/check_native_interop.js').then(r => console.log('check_native_interop.js:', r.status));
```

**Expected output:** All 200

**If 404:** Resource provider missing URL mapping in PluginEditor.cpp

### 8.4 MIME Type Validation

**Check network tab (Inspect mode):**
- `index.html` → `text/html`
- `index.js` → `application/javascript` (NOT `text/javascript`)
- `check_native_interop.js` → `application/javascript`

**Wrong MIME type causes:** Script not executed, blank screen

---

## Phase 9: Performance Testing

### 9.1 UI Responsiveness

**Test:**
1. Rapidly drag sliders/knobs (10 seconds continuous)
2. Monitor CPU usage in DAW

**Expected:**
- Smooth animation (60fps)
- CPU usage < 5% during interaction
- No lag or stuttering

### 9.2 Parameter Update Rate

**Test:**
1. Enable automation for 10+ parameters
2. Play back complex automation curves
3. Monitor UI updates

**Expected:**
- UI tracks automation smoothly
- No visual glitches or frozen controls
- Frame rate remains stable

---

## Phase 10: Final Checklist

### Implementation Complete

- [ ] All 5 UI files copied to Source/ui/public/
- [ ] PluginEditor.h member order verified (relays → webView → attachments)
- [ ] PluginEditor.cpp initialization order matches declaration
- [ ] CMakeLists.txt WebView configuration added
- [ ] Debug build succeeds without warnings
- [ ] Release build succeeds without crashes
- [ ] Standalone loads WebView (not blank)
- [ ] VST3 appears in DAW browser
- [ ] AU passes validation
- [ ] All 53 parameters sync UI ↔ DAW
- [ ] Automation recording/playback works
- [ ] Preset recall updates UI correctly
- [ ] No viewport units in CSS
- [ ] Native feel CSS present (user-select: none)
- [ ] Resource provider returns correct MIME types
- [ ] No JavaScript errors in console
- [ ] Performance acceptable (< 5% CPU during interaction)

### Known Issues (if any)

Document any deviations or issues here:

- None (expected for successful integration)

---

## Troubleshooting

### Issue: Blank WebView

**Symptoms:**
- Window opens but shows black/white screen
- No UI elements visible

**Checks:**
1. Resource provider returns files correctly
2. `window.__JUCE__` exists in console
3. JavaScript console shows no errors
4. MIME types correct (application/javascript)

**Solution:**
- Add console.log() to getResource()
- Verify BinaryData symbols match file paths
- Check CMake registered all 3 files

### Issue: Parameters Don't Update

**Symptoms:**
- UI displays but controls frozen
- Dragging controls does nothing
- Automation doesn't move UI

**Checks:**
1. All 53 relays created in constructor
2. All 53 attachments created with correct parameter IDs
3. `.withOptionsFrom()` called for all 53 relays
4. Parameter IDs match YAML exactly (snake_case)

**Solution:**
- Count relay/attachment declarations (must be 53 each)
- Verify parameter ID spelling
- Add `nullptr` as 3rd parameter to attachments (JUCE 8)

### Issue: Release Build Crashes

**Symptoms:**
- Debug works fine
- Release crashes on plugin close
- Error mentions "evaluateJavascript"

**Root cause:** Wrong member order

**Solution:**
- Verify PluginEditor.h order: relays → webView → attachments
- Members destroyed in REVERSE order
- Attachments MUST be declared AFTER webView

### Issue: VST3 Not in DAW

**Symptoms:**
- AU works
- VST3 missing from browser
- Binary exists in build artifacts

**Checks:**
1. `NEEDS_WEB_BROWSER TRUE` in juce_add_plugin()
2. Reinstalled after rebuild
3. DAW restarted

**Solution:**
- Add NEEDS_WEB_BROWSER flag
- Rebuild + reinstall + restart DAW

---

## Success Criteria

**Stage 3 (GUI) complete when:**

1. Plugin loads in all formats (VST3, AU, Standalone)
2. UI displays correctly (Apple Liquid Glass aesthetic)
3. All 53 parameters bind UI ↔ DAW
4. Automation recording/playback works
5. Preset recall updates UI
6. No crashes in release builds
7. Performance acceptable
8. No JavaScript errors

**Next:** Proceed to validation-agent (automatic runtime testing)
