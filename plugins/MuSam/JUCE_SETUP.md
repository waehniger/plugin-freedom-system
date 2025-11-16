# JUCE Setup für MuSam Build

## Problem
Der Build schlägt fehl, weil JUCE nicht am erwarteten Ort gefunden wird.

## Lösung

Die Root-CMakeLists.txt wurde aktualisiert, um mehrere Standardorte zu prüfen:
- `~/JUCE` (empfohlen)
- `/Applications/JUCE`
- `~/Developer/JUCE`
- `/usr/local/JUCE`

## JUCE Installation

### Option 1: Git Clone (empfohlen)

```bash
# Clone JUCE 8.0.9 (wie in system-config.json erwartet)
cd ~
git clone --branch 8.0.9 https://github.com/juce-framework/JUCE.git

# Oder neueste Version
git clone https://github.com/juce-framework/JUCE.git
```

### Option 2: Download von GitHub

1. Besuche: https://github.com/juce-framework/JUCE/releases
2. Lade `juce-8.0.9-osx.zip` herunter
3. Extrahiere nach `~/JUCE`:

```bash
cd ~
unzip ~/Downloads/juce-8.0.9-osx.zip
mv juce-8.0.9-osx JUCE
```

### Option 3: Symlink erstellen (falls JUCE bereits installiert)

Wenn JUCE bereits an einem anderen Ort installiert ist:

```bash
# Finde JUCE
find ~ -name "juce_core.h" -path "*/modules/juce_core/juce_core.h" 2>/dev/null | head -1

# Erstelle Symlink (ersetze /path/to/JUCE mit dem gefundenen Pfad)
ln -s /path/to/JUCE ~/JUCE
```

## Verifizierung

Nach der Installation prüfe:

```bash
# Prüfe, ob JUCE installiert ist
test -f ~/JUCE/modules/juce_core/juce_core.h && echo "✓ JUCE gefunden" || echo "✗ JUCE nicht gefunden"

# Prüfe Version
grep "JUCE_MAJOR_VERSION" ~/JUCE/modules/juce_core/system/juce_StandardHeader.h
```

## Build-Test

Nach der Installation:

```bash
cd /Users/willybeyer/Documents/Code/plugin-freedom-system
./scripts/build-and-install.sh MuSam --no-install
```

Die CMakeLists.txt findet JUCE automatisch an einem der Standardorte.

## Alternative: Umgebungsvariable

Falls JUCE an einem benutzerdefinierten Ort installiert ist:

```bash
export JUCE_PATH=/path/to/JUCE
./scripts/build-and-install.sh MuSam --no-install
```

