---
name: plugin-lifecycle
description: Install/remove plugins from macOS system folders
allowed-tools:
  - Bash
  - Read
  - Edit # For PLUGINS.md updates
preconditions:
  - Plugin must have successful Release build
---

# plugin-lifecycle Skill

**Purpose:** Install plugins to macOS system folders with proper permissions, cache clearing, and verification.

## Overview

This skill handles the complete lifecycle of plugin installation and removal on macOS:

- **Installation**: Copy Release binaries to system folders (`~/Library/Audio/Plug-Ins/`)
- **Permissions**: Set correct permissions (755) for DAW access
- **Cache clearing**: Clear DAW caches (Ableton, Logic Pro, AudioComponentRegistrar)
- **Verification**: Confirm successful installation with timestamps and sizes
- **State tracking**: Update PLUGINS.md with installation status and locations
- **Uninstallation**: Clean removal with cache clearing

**Why this matters:**

DAWs scan system folders for plugins. Installing to the correct locations with proper permissions ensures your plugin appears in the DAW's plugin list.

**System folders (macOS):**

- **VST3**: `~/Library/Audio/Plug-Ins/VST3/`
- **AU (Audio Unit)**: `~/Library/Audio/Plug-Ins/Components/`
- **AAX** (Pro Tools): `~/Library/Application Support/Avid/Audio/Plug-Ins/` (future support)

## Step 1: Build Verification

**Before installation, verify Release binaries exist.**

Check for build artifacts:

```bash
PLUGIN_NAME="[PluginName]"
PRODUCT_NAME="[ProductName]"  # May differ from plugin name (can contain spaces)

# VST3 location (JUCE's default artefacts directory)
VST3_PATH="build/plugins/$PLUGIN_NAME/${PLUGIN_NAME}_artefacts/Release/VST3/${PRODUCT_NAME}.vst3"

# AU location
AU_PATH="build/plugins/$PLUGIN_NAME/${PLUGIN_NAME}_artefacts/Release/AU/${PRODUCT_NAME}.component"

# Check existence
test -d "$VST3_PATH"
test -d "$AU_PATH"
```

**If binaries missing:**

```
Release build not found.

Expected locations:
- VST3: build/plugins/[PluginName]/[PluginName]_artefacts/Release/VST3/[Product].vst3
- AU: build/plugins/[PluginName]/[PluginName]_artefacts/Release/AU/[Product].component

Would you like to:
1. Build Release mode now (~2-5 min)
2. Abort installation

Choose (1-2): _
```

**Option 1: Build Release mode now**

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel

# Verify build succeeded
if [ $? -eq 0 ]; then
    echo "✓ Build successful"
else
    echo "✗ Build failed - check compiler errors above"
    exit 1
fi
```

After successful build, proceed to Step 2.

**Option 2: Abort installation**

```
Installation aborted.

To build manually:
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release

Then rerun: /install-plugin [PluginName]
```

**Build verification details:**

- **Release mode required**: Debug builds contain debug symbols (large file size, slower performance)
- **Parallel build**: `--parallel` flag uses all CPU cores (faster compilation)
- **Artefacts directory**: JUCE places built plugins in `[PluginName]_artefacts/Release/`

## Step 2: Product Name Extraction

**Extract the PRODUCT_NAME from CMakeLists.txt** (may differ from plugin directory name).

Read `plugins/[PluginName]/CMakeLists.txt`:

```cmake
juce_add_plugin([PluginName]
    PRODUCT_NAME "[Product Name]"
    COMPANY_NAME "[Company Name]"
    # ... other options
)
```

**Extract PRODUCT_NAME value:**

```bash
PRODUCT_NAME=$(grep 'PRODUCT_NAME' plugins/$PLUGIN_NAME/CMakeLists.txt | sed 's/.*PRODUCT_NAME "\(.*\)".*/\1/')
```

**Why this matters:**

- Plugin directory: `TapeAge` (filesystem, no spaces)
- Product name: `Tape Age` (displayed in DAW, may contain spaces)
- Installed filename: `Tape Age.vst3` (uses product name, not directory name)

**Example variations:**

```
Directory: SimpleCompressor
Product: "Simple Compressor"
Installed: Simple Compressor.vst3

Directory: VintageEQ
Product: "VintageEQ"  (same as directory)
Installed: VintageEQ.vst3

Directory: MyPlugin
Product: "My Amazing Plugin v2"
Installed: My Amazing Plugin v2.vst3
```

**Handle spaces correctly:**

```bash
# Wrong (breaks with spaces)
cp $AU_PATH ~/Library/Audio/Plug-Ins/Components/

# Correct (quotes preserve spaces)
cp "$AU_PATH" ~/Library/Audio/Plug-Ins/Components/
```

## Step 3: Old Version Removal

**Check for existing installations** (prevents stale versions).

Search system folders:

```bash
# VST3
VST3_INSTALLED="$HOME/Library/Audio/Plug-Ins/VST3/${PRODUCT_NAME}.vst3"
if [ -d "$VST3_INSTALLED" ]; then
    echo "Found existing VST3: $VST3_INSTALLED"
fi

# AU
AU_INSTALLED="$HOME/Library/Audio/Plug-Ins/Components/${PRODUCT_NAME}.component"
if [ -d "$AU_INSTALLED" ]; then
    echo "Found existing AU: $AU_INSTALLED"
fi
```

**If old version found:**

```
Old version found:
- VST3: ~/Library/Audio/Plug-Ins/VST3/[Product].vst3 (installed: 2025-01-05)
- AU: ~/Library/Audio/Plug-Ins/Components/[Product].component (installed: 2025-01-05)

Removing old version before installing new version...
```

**Remove old installations:**

```bash
rm -rf "$VST3_INSTALLED"
rm -rf "$AU_INSTALLED"
```

**Why removal is important:**

- **Prevents version conflicts**: DAWs may cache old plugin metadata
- **Ensures clean install**: Avoids file permission issues from partial installs
- **Clears corrupted states**: If previous install was incomplete

**Safety:**

- Only removes from system folders (source code in `plugins/` untouched)
- `-rf` required because plugins are bundles (directories, not single files)
- No confirmation needed (old version being replaced anyway)

## Step 4: Copy to System Folders

**Install plugins to macOS system folders** where DAWs scan for plugins.

Copy binaries:

```bash
# VST3
cp -R "$VST3_PATH" ~/Library/Audio/Plug-Ins/VST3/

# AU
cp -R "$AU_PATH" ~/Library/Audio/Plug-Ins/Components/
```

**Flags:**

- `-R`: Recursive (plugins are bundles containing multiple files)
- Quotes: Handle product names with spaces

**File structure after copy:**

```
~/Library/Audio/Plug-Ins/
├── VST3/
│   └── [Product Name].vst3/
│       └── Contents/
│           └── MacOS/
│               └── [Product Name]  (binary)
└── Components/
    └── [Product Name].component/
        └── Contents/
            ├── Info.plist
            └── MacOS/
                └── [Product Name]  (binary)
```

**Verification during copy:**

```bash
if [ $? -eq 0 ]; then
    echo "✓ VST3 copied successfully"
else
    echo "✗ VST3 copy failed"
    exit 1
fi
```

**Common copy failures:**

- **Permission denied**: System folder not writable (rare on macOS user folders)
- **Disk full**: Not enough space (check with `df -h ~`)
- **Directory doesn't exist**: Create with `mkdir -p ~/Library/Audio/Plug-Ins/{VST3,Components}`

**Plugin bundle structure explained:**

macOS plugins are **bundles** (directories that look like single files in Finder):

```
[Product].vst3/
├── Contents/
│   ├── Info.plist          (plugin metadata: name, version, manufacturer ID)
│   ├── PkgInfo             (bundle type identifier)
│   ├── Resources/          (icons, UI assets)
│   │   └── ui/             (WebView HTML, if using WebBrowserComponent)
│   └── MacOS/
│       └── [Product]       (actual binary: Mach-O executable)
```

DAWs read `Info.plist` to identify plugin type, then load the binary from `MacOS/`.

## Step 5: Permissions Verification

**Set correct permissions** so DAWs can load plugins.

Required permissions: **755** (rwxr-xr-x)

- **Owner**: Read, write, execute
- **Group**: Read, execute
- **Others**: Read, execute

Set permissions:

```bash
chmod -R 755 "$HOME/Library/Audio/Plug-Ins/VST3/${PRODUCT_NAME}.vst3"
chmod -R 755 "$HOME/Library/Audio/Plug-Ins/Components/${PRODUCT_NAME}.component"
```

**Why 755:**

- DAWs run as the user (need read + execute)
- Must execute the binary inside bundle (need execute on directories + binary)
- No write needed (DAWs shouldn't modify plugin files)

**Verify permissions:**

```bash
ls -ld "$HOME/Library/Audio/Plug-Ins/VST3/${PRODUCT_NAME}.vst3"
# Expected output: drwxr-xr-x ... [Product].vst3
```

**Permission troubleshooting:**

```
Issue: Plugin appears in DAW but won't load

Check permissions:
ls -lR ~/Library/Audio/Plug-Ins/VST3/[Product].vst3

Look for:
- Directories without execute bit (d---------): DAW can't traverse
- Binary without execute bit (-rw-r--r--): DAW can't run
- Entire bundle without read (d-wx------): DAW can't read Info.plist

Fix: chmod -R 755 ~/Library/Audio/Plug-Ins/VST3/[Product].vst3
```

**Code signing (future consideration):**

macOS Gatekeeper may require code signing for distribution:

```bash
# Check if signed
codesign -dv ~/Library/Audio/Plug-Ins/VST3/[Product].vst3

# If unsigned, DAWs may show "unverified developer" warning
# For local development: System Preferences → Security → Allow anyway
# For distribution: Sign with Apple Developer certificate
```

## Step 6: Cache Clearing

**Clear DAW caches** so they recognize the newly installed plugin.

DAWs cache plugin metadata (name, parameters, version) for fast scanning. After installation, caches must be cleared.

### Ableton Live

**Cache location:**

```bash
~/Library/Preferences/Ableton/Live */PluginCache.db
```

**Clear cache:**

```bash
rm -f ~/Library/Preferences/Ableton/Live\ */PluginCache.db
```

**Explanation:**

- `Live */` matches all Live versions (Live 10, Live 11, Live 12)
- `PluginCache.db` is an SQLite database of scanned plugins
- After deletion, Live rescans on next launch

**Verification:**

```bash
ls ~/Library/Preferences/Ableton/Live\ */PluginCache.db 2>/dev/null
# Should return nothing (file deleted)
```

### Logic Pro

**Cache location:**

```bash
~/Library/Caches/AudioUnitCache/
```

**Clear cache + restart registration:**

```bash
# Delete cache files
rm -rf ~/Library/Caches/AudioUnitCache/*

# Kill AudioComponentRegistrar (macOS service that indexes Audio Units)
killall AudioComponentRegistrar 2>/dev/null || true
```

**Explanation:**

- `AudioUnitCache/` contains indexed AU metadata
- `AudioComponentRegistrar` is a macOS background service that maintains AU database
- Killing it forces macOS to rebuild AU database on next access
- `2>/dev/null` suppresses error if process not running
- `|| true` prevents script failure if killall returns non-zero

**Verification:**

```bash
# Check if cache cleared
ls ~/Library/Caches/AudioUnitCache/
# Should be empty or show only new cache files

# Check if AudioComponentRegistrar restarted
ps aux | grep AudioComponentRegistrar
# Should show new process (different PID) after a few seconds
```

### Other DAWs

**Reaper:**

- No persistent cache (scans on launch)
- No cache clearing needed

**Pro Tools:**

- Cache location: `~/Library/Application Support/Avid/Audio/Plug-Ins/`
- AAX plugins (future support)

**Cubase/Nuendo:**

- Preferences → Plug-in Manager → Reset
- Manual rescan (no command-line access)

### Cache Clearing Report

**After clearing all caches:**

```
✓ Cache clearing complete

Ableton Live: PluginCache.db deleted (all versions)
Logic Pro: AudioUnitCache cleared, AudioComponentRegistrar restarted

Next steps:
1. Launch your DAW
2. Rescan plugins (or restart DAW)
3. Plugin should appear in plugin list as "[Product Name]"
```

**If cache clearing fails:**

```
⚠️  Warning: Cache clearing failed (not critical)

Ableton Live: Permission denied (may need manual deletion)
Logic Pro: AudioComponentRegistrar not running (may not need restart)

Plugin installed successfully, but DAW caches may be stale.
Manual fix: Restart your DAW to pick up new plugin.
```

## Step 7: Verification

**Verify installation succeeded** with file checks and size validation.

Check installed files:

```bash
# VST3
ls -lh "$HOME/Library/Audio/Plug-Ins/VST3/${PRODUCT_NAME}.vst3"

# AU
ls -lh "$HOME/Library/Audio/Plug-Ins/Components/${PRODUCT_NAME}.component"
```

**Expected output:**

```
drwxr-xr-x  3 user  staff   96B Jan 10 14:32 Tape Age.vst3
drwxr-xr-x  3 user  staff   96B Jan 10 14:32 Tape Age.component
```

**Verification checklist:**

- ✅ **Files exist**: Both VST3 and AU present
- ✅ **Permissions correct**: `drwxr-xr-x` (755)
- ✅ **Timestamps recent**: Within last 60 seconds
- ✅ **Sizes reasonable**: Typically > 100 KB (varies by plugin complexity)

**Size expectations by plugin type:**

- **Simple utility** (gain, pan): ~500 KB - 1 MB
- **Dynamics processor** (compressor, limiter): ~1-3 MB
- **EQ**: ~1-2 MB
- **Time-based effects** (delay, reverb): ~2-5 MB
- **Complex plugins** (synthesizers, spectral): ~5-20 MB
- **Plugins with WebView UI**: +1-2 MB (HTML/CSS/JS assets)

**Size warnings:**

```
⚠️  Size check: VST3 is 47 KB (expected > 100 KB)

This is unusually small. Possible issues:
- Build was incomplete (check build logs)
- Binary is stripped of symbols (expected in Release mode)
- Plugin is very simple (if intentional, ignore this warning)

Recommendation: Test plugin in DAW before assuming it's broken.
```

**Detailed file inspection:**

```bash
# Check binary exists inside bundle
ls -lh "$HOME/Library/Audio/Plug-Ins/VST3/${PRODUCT_NAME}.vst3/Contents/MacOS/"

# Expected: Mach-O executable with same name as product
-rwxr-xr-x  1 user  staff   2.1M Jan 10 14:32 Tape Age

# Verify it's a valid Mach-O binary
file "$HOME/Library/Audio/Plug-Ins/VST3/${PRODUCT_NAME}.vst3/Contents/MacOS/${PRODUCT_NAME}"

# Expected: Mach-O 64-bit dynamically linked shared library arm64
```

**Installation Report:**

```
✓ [PluginName] installed successfully

Installed formats:
- VST3: ~/Library/Audio/Plug-Ins/VST3/[ProductName].vst3 (2.4 MB)
- AU: ~/Library/Audio/Plug-Ins/Components/[ProductName].component (2.4 MB)

Permissions: 755 (correct)
Timestamps: Jan 10, 2025 14:32 (just now)

Cache status: Cleared (Ableton + Logic)

Next steps:
1. Open your DAW (Logic Pro, Ableton Live, Reaper, etc.)
2. Rescan plugins (or restart DAW if it doesn't rescan automatically)
3. Search for "[ProductName]" in plugin list
4. Load plugin in a track
5. Test audio processing and UI

If plugin doesn't appear:
- Check DAW's plugin scan paths include ~/Library/Audio/Plug-Ins/
- Check Console.app for plugin load errors
- Try manual rescan in DAW preferences
```

## Update PLUGINS.md

**Record installation in PLUGINS.md** for tracking.

Read current plugin entry:

```markdown
## [PluginName]

**Status:** ✅ Working
**Created:** 2025-01-08
**Last tested:** 2025-01-10 (pluginval passed)

...
```

Update status to:

```markdown
## [PluginName]

**Status:** 📦 Installed
**Created:** 2025-01-08
**Last tested:** 2025-01-10 (pluginval passed)
**Installed:** 2025-01-10

**Formats:** VST3, AU

**Locations:**
- VST3: `~/Library/Audio/Plug-Ins/VST3/[ProductName].vst3`
- AU: `~/Library/Audio/Plug-Ins/Components/[ProductName].component`

...
```

**Status emoji meanings:**

- **💡 Idea** → Initial concept, creative brief only
- **🚧 In Progress** → Implementation started, not complete
- **✅ Working** → Built and tested, not installed
- **📦 Installed** → Installed to system folders
- **🎯 Production** → Tested in real projects, ready for use

**Installation metadata:**

- **Installed date**: When `/install-plugin` was run
- **Formats**: VST3, AU (future: AAX)
- **Locations**: Full paths for reference

## Uninstall Process

**When user requests uninstall:** `/uninstall-plugin [PluginName]` (future slash command)

### Step 1: Locate Plugin Files

Same as Step 2 (extract product name from CMakeLists.txt):

```bash
PRODUCT_NAME=$(grep 'PRODUCT_NAME' plugins/$PLUGIN_NAME/CMakeLists.txt | ...)

VST3_INSTALLED="$HOME/Library/Audio/Plug-Ins/VST3/${PRODUCT_NAME}.vst3"
AU_INSTALLED="$HOME/Library/Audio/Plug-Ins/Components/${PRODUCT_NAME}.component"
```

Check existence:

```bash
VST3_EXISTS=false
AU_EXISTS=false

[ -d "$VST3_INSTALLED" ] && VST3_EXISTS=true
[ -d "$AU_INSTALLED" ] && AU_EXISTS=true
```

### Step 2: Confirm Removal

**Ask user to confirm:**

```
Remove [PluginName] from system folders?

This will delete:
- VST3: ~/Library/Audio/Plug-Ins/VST3/[ProductName].vst3 (2.4 MB)
- AU: ~/Library/Audio/Plug-Ins/Components/[ProductName].component (2.4 MB)

Source files in plugins/[PluginName]/ will NOT be deleted.
You can reinstall anytime with: /install-plugin [PluginName]

Proceed? (y/n): _
```

**If user confirms (y):** Proceed to Step 3

**If user cancels (n):**

```
Uninstallation cancelled.

Plugin remains installed:
- VST3: ~/Library/Audio/Plug-Ins/VST3/[ProductName].vst3
- AU: ~/Library/Audio/Plug-Ins/Components/[ProductName].component
```

### Step 3: Remove Files

Delete from system folders:

```bash
if [ "$VST3_EXISTS" = true ]; then
    rm -rf "$VST3_INSTALLED"
    echo "✓ VST3 removed"
fi

if [ "$AU_EXISTS" = true ]; then
    rm -rf "$AU_INSTALLED"
    echo "✓ AU removed"
fi
```

**Safety:**

- Only removes from system folders (`~/Library/Audio/Plug-Ins/`)
- Source code in `plugins/[PluginName]/` untouched
- Build artifacts in `build/` untouched

### Step 4: Clear Caches

Same as installation Step 6:

```bash
# Ableton Live
rm -f ~/Library/Preferences/Ableton/Live\ */PluginCache.db

# Logic Pro
rm -rf ~/Library/Caches/AudioUnitCache/*
killall AudioComponentRegistrar 2>/dev/null || true
```

**Why cache clearing after uninstall:**

DAWs may still show removed plugin in list (from cache). Clearing forces rescan.

### Step 5: Update PLUGINS.md

Change status back to **✅ Working**:

```markdown
## [PluginName]

**Status:** ✅ Working (was 📦 Installed)
**Created:** 2025-01-08
**Last tested:** 2025-01-10 (pluginval passed)
**Installed:** 2025-01-10 (uninstalled: 2025-01-11)

...
```

Remove installation metadata:

- ~~**Formats:** VST3, AU~~
- ~~**Locations:** ...~~

### Step 6: Confirmation

```
✓ [PluginName] uninstalled

Removed from system folders:
- VST3: ~/Library/Audio/Plug-Ins/VST3/[ProductName].vst3 (deleted)
- AU: ~/Library/Audio/Plug-Ins/Components/[ProductName].component (deleted)

Cache status: Cleared (Ableton + Logic)

Source code remains: plugins/[PluginName]/

To reinstall: /install-plugin [PluginName]
```

## Decision Menu After Installation

**After successful installation:**

```
✓ [PluginName] installed successfully

What's next?
1. Test in DAW (recommended) → Load in Logic/Ableton/Reaper
2. Create another plugin → /dream
3. Document this plugin → Create user guide/manual
4. Share plugin (export build) → Future: Package for distribution
5. Other

Choose (1-5): _
```

**Handle responses:**

### Option 1: Test in DAW

```
To test [PluginName] in your DAW:

1. Open your DAW (Logic Pro, Ableton Live, Reaper, etc.)
2. If DAW was already open, restart it (to pick up new plugin)
3. Create an audio track or load existing audio
4. Open plugin selector and search for "[ProductName]"
5. Load plugin on track
6. Play audio and test parameters

Common testing tasks:
- Verify UI loads correctly
- Test all parameters respond to changes
- Check sound quality matches expectations
- Test automation (draw automation, verify plugin follows)
- Test preset save/load

Report any issues found and I can help troubleshoot.
```

### Option 2: Create another plugin

Invoke `plugin-ideation` skill:

```
Starting new plugin ideation...

What plugin would you like to create?

(Describe the type of plugin, what it should do, your vision for it)
```

### Option 3: Document this plugin

```
[Future feature - Phase 4]

Plugin documentation will include:
- User manual (how to use, parameter descriptions)
- Developer notes (architecture, DSP details)
- Changelog (version history)
- Known issues

For now, you can manually document in:
plugins/[PluginName]/.ideas/user-manual.md
```

### Option 4: Share plugin

```
[Future feature - Phase 5]

Plugin distribution will include:
- Package installer (drag-and-drop DMG)
- Code signing (Apple Developer certificate)
- License terms
- Version tracking
- Update mechanism

For now, you can manually share:
- Build artifacts: build/plugins/[Name]/[Name]_artefacts/Release/
- Recipient copies to: ~/Library/Audio/Plug-Ins/{VST3,Components}/
```

### Option 5: Other

```
What would you like to do?
```

## Error Handling

### Build Files Not Found

```
Release build not found.

Expected locations:
- VST3: build/plugins/[PluginName]/[PluginName]_artefacts/Release/VST3/[Product].vst3
- AU: build/plugins/[PluginName]/[PluginName]_artefacts/Release/AU/[Product].component

This usually means:
1. Plugin hasn't been built yet
2. Build was in Debug mode (need Release)
3. Build failed (check previous errors)

To build manually:
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel

Then rerun: /install-plugin [PluginName]

Or use: /test [PluginName] build (builds + validates in one step)
```

### Permission Denied

```
Permission denied writing to system folders.

Error: cp: cannot create directory '~/Library/Audio/Plug-Ins/VST3/[Product].vst3': Permission denied

This usually means:
1. System folder doesn't exist (create it first)
2. System folder not writable (check permissions)
3. Disk is read-only (rare on macOS)

Fixes to try:

1. Create directories:
   mkdir -p ~/Library/Audio/Plug-Ins/VST3
   mkdir -p ~/Library/Audio/Plug-Ins/Components

2. Check permissions:
   ls -ld ~/Library/Audio/Plug-Ins/VST3
   # Should show: drwxr-xr-x (755)

3. Fix permissions:
   chmod 755 ~/Library/Audio/Plug-Ins/VST3
   chmod 755 ~/Library/Audio/Plug-Ins/Components

4. Check disk space:
   df -h ~
   # Ensure "Available" > 100 MB

After fixing, rerun: /install-plugin [PluginName]
```

### Cache Clear Failed

```
⚠️  Warning: Cache clear failed (not critical)

Ableton Live: Permission denied deleting PluginCache.db
Logic Pro: AudioComponentRegistrar not found

Plugin installed successfully, but DAW caches may be stale.

Manual fixes:

Ableton Live:
1. Quit Ableton Live
2. Manually delete: ~/Library/Preferences/Ableton/Live */PluginCache.db
3. Relaunch Ableton Live

Logic Pro:
1. Quit Logic Pro
2. Open Terminal, run: killall AudioComponentRegistrar
3. Delete cache: rm -rf ~/Library/Caches/AudioUnitCache/*
4. Relaunch Logic Pro

Or simply restart your DAW - it should rescan automatically.
```

### Plugin Doesn't Appear in DAW

```
Plugin installed but not appearing in DAW?

Troubleshooting checklist:

1. Restart DAW:
   Many DAWs only scan plugins at launch. Quit and relaunch.

2. Check DAW scan paths:
   Verify DAW is scanning ~/Library/Audio/Plug-Ins/VST3 and /Components

   Logic Pro: Should scan automatically (Audio Units)
   Ableton Live: Preferences → Plug-Ins → VST3 Custom Folder
   Reaper: Preferences → Plug-ins → VST → Re-scan

3. Check plugin format:
   Logic Pro: Uses AU only (ignore VST3)
   Ableton Live: Prefers VST3 over AU
   Reaper: Uses both VST3 and AU

4. Check for load errors:
   Open Console.app (macOS utility)
   Search for "[ProductName]" or "AudioComponentRegistrar"
   Look for error messages (missing dependencies, crashes)

5. Verify installation:
   ls -la ~/Library/Audio/Plug-Ins/VST3/[Product].vst3
   ls -la ~/Library/Audio/Plug-Ins/Components/[Product].component

   Check permissions: Should be drwxr-xr-x (755)

6. Try manual rescan:
   Most DAWs: Preferences → Plugins → Rescan or "Clear cache and rescan"

7. Check for Gatekeeper block:
   System Preferences → Security & Privacy → General
   Look for: "[Product] was blocked from use because it is not from an identified developer"
   Click: "Open Anyway"

If still not working, share Console.app errors for debugging.
```

## Integration Points

**Invoked by:**

- `/install-plugin [PluginName]` command
- `plugin-workflow` skill → After Stage 6 (Validation) complete
- `plugin-improve` skill → After successful changes + testing
- Natural language: "Install [PluginName]", "Deploy [PluginName] to system folders"

**Invokes:**

- None (terminal skill, doesn't invoke others)

**Updates:**

- `PLUGINS.md` → Status changes to 📦 Installed, adds installation metadata

**Creates:**

- System folder installations (non-git-tracked):
  - `~/Library/Audio/Plug-Ins/VST3/[Product].vst3`
  - `~/Library/Audio/Plug-Ins/Components/[Product].component`

**Blocks:**

- None (installation is optional, plugins can be tested without installing)

## Success Criteria

Installation is successful when:

- ✅ Both VST3 and AU files copied to system folders
- ✅ Permissions set correctly (755) on all files
- ✅ Caches cleared for all detected DAWs
- ✅ Verification shows recent timestamps (< 60 seconds ago)
- ✅ File sizes are reasonable (> 100 KB typically)
- ✅ PLUGINS.md updated with 📦 status and installation locations
- ✅ User knows next steps (restart DAW, rescan plugins)

**NOT required for success:**

- Plugin appearing in DAW immediately (may need rescan/restart)
- All DAW caches cleared (some DAWs may not be installed)
- Code signing (optional for local development)
