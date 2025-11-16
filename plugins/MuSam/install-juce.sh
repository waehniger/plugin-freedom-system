#!/bin/bash
# JUCE Installation Script für MuSam

set -e

echo "============================================================================"
echo "JUCE Installation für Plugin Freedom System"
echo "============================================================================"
echo ""

# Prüfe, ob JUCE bereits installiert ist
if [ -d ~/JUCE ] && [ -f ~/JUCE/modules/juce_core/juce_core.h ]; then
    echo "✓ JUCE bereits installiert bei ~/JUCE"
    echo "  Version: $(grep 'JUCE_MAJOR_VERSION' ~/JUCE/modules/juce_core/system/juce_StandardHeader.h 2>/dev/null | head -1 || echo 'unbekannt')"
    echo ""
    echo "JUCE ist bereit für den Build!"
    exit 0
fi

# Prüfe Git
if ! command -v git &> /dev/null; then
    echo "❌ Git nicht gefunden. Bitte installiere Git zuerst:"
    echo "   macOS: xcode-select --install"
    exit 1
fi

echo "→ Installiere JUCE 8.0.9 nach ~/JUCE..."
echo ""

# Clone JUCE
cd ~
if [ -d JUCE ]; then
    echo "⚠️  ~/JUCE existiert bereits. Überspringe Installation."
    echo "   Falls Probleme auftreten, entferne ~/JUCE und führe das Script erneut aus."
else
    echo "→ Klone JUCE Repository..."
    git clone --branch 8.0.9 --depth 1 https://github.com/juce-framework/JUCE.git
    
    if [ $? -eq 0 ]; then
        echo "✓ JUCE erfolgreich geklont"
    else
        echo "❌ Fehler beim Klonen von JUCE"
        echo "   Versuche alternativ die neueste Version..."
        git clone --depth 1 https://github.com/juce-framework/JUCE.git
    fi
fi

# Verifiziere Installation
if [ -f ~/JUCE/modules/juce_core/juce_core.h ]; then
    echo ""
    echo "✓ JUCE erfolgreich installiert!"
    echo ""
    echo "Installationspfad: ~/JUCE"
    echo "Version:"
    grep "JUCE_MAJOR_VERSION\|JUCE_MINOR_VERSION\|JUCE_BUILDNUMBER" ~/JUCE/modules/juce_core/system/juce_StandardHeader.h 2>/dev/null | head -3 || echo "  (Version-Info nicht verfügbar)"
    echo ""
    echo "Nächste Schritte:"
    echo "  1. Teste den Build: ./scripts/build-and-install.sh MuSam --no-install"
    echo "  2. Oder starte Stage 2: /continue MuSam"
else
    echo ""
    echo "❌ Installation fehlgeschlagen"
    echo "   Bitte prüfe die Fehlermeldungen oben"
    exit 1
fi

