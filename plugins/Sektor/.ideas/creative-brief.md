# Sektor - Creative Brief

## Overview

**Type:** Instrument (Sampler)
**Core Concept:** Minimalistischer Granular Sampler mit interaktiver Region-Markierung und MIDI-Steuerung
**Status:** 💡 Ideated
**Created:** 2025-11-18

## Vision

Sektor ist ein minimalistischer Granular Sampler, der es dem Nutzer ermöglicht, ein Audio-Sample zu laden, eine beliebige Region des Samples zu markieren (durch interaktives Klick-Drag auf der Waveform) und diese Region anschließend über MIDI zu spielen. Die granulare Verarbeitung bietet vollständige Kontrolle über Körnung, Dichte, Pitch-Versatz und Spacing, um kreative Textur-Manipulationen zu ermöglichen.

Das Design ist bewusst minimalistisch: Fokus auf Funktionalität statt Überfluss. Sektor soll schnell einsatzbereit sein und dabei professionelle Granular-Verarbeitung bieten.

## Parameters

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Grain Size | 10 - 500 ms | 100 ms | Größe der einzelnen Körner in Millisekunden |
| Density | 1 - 200 grains/sec | 50 | Anzahl der Körner pro Sekunde |
| Pitch Shift | -12 bis +12 Semitöne | 0 | Transposition des granular abgespielten Samples |
| Spacing | 0.1 - 2.0 | 1.0 | Abstand zwischen Körnern (Multiplikator) |
| Region Start | 0 - [Sample Length] ms | 0 | Startpunkt der zu spielenden Region (interaktiv oder numerisch) |
| Region End | 0 - [Sample Length] ms | [Sample Length] | Endpunkt der zu spielenden Region (interaktiv oder numerisch) |
| Polyphony Mode | Mono / Poly / Both | Poly | Umschaltbar zwischen monophon und polyphon |

## UI Concept

**Layout:**
- Großes Waveform-Display oben (mindestens 60% der Höhe)
- Interaktive Region-Markierung durch Klick-Drag auf der Waveform
- Granular-Parameter unterhalb des Waveform-Displays in horizontaler Anordnung
- Sample-Load-Area (Drag & Drop + File Browser Button)

**Visual Style:**
- Minimalistisches, cleanes Design
- Dunkelgrauer Hintergrund mit helleren Akzenten für Waveform und Parameter
- Einfache, lesbare Fonts
- Fokus auf Funktionalität vor Ästhetik

**Key Elements:**
- Waveform-Display mit Region-Markierung (visueller Highlight der Region)
- Drag-&-Drop-Zone für Sample-Import
- File Browser Button zum Öffnen von Audio-Dateien
- Slider/Knöpfe für Grain Size, Density, Pitch Shift, Spacing
- Mode-Switch für Polyphonie
- Numerische Input-Felder für Region Start/End (optional, aber für Präzision)

## Use Cases

- Glitchy Texturen und experimentelle Sound-Design
- Pitched Loop-Manipulationen im Live-Setting
- Sample-basierte Sequencing mit granularer Verarbeitung
- Retro-Lo-Fi-Effekte durch Körnung und Pitch-Verfremdung
- Atmosphärische Pads aus einzelnen Sample-Regionen

## Inspirations

- Granular Synthesis (akademisches Konzept)
- Native Instruments Monark (granular playback inspiration)
- Ableton Live Sampler (region-based approach)
- Reaktor Ensembles (granular processing)
- Glitch-Elektronik und Microsound (sonic aesthetic)

## Technical Notes

- Granular Playback Engine: Körner werden aus der markierten Region extrahiert und mit Fenster-Funktion (Hann Window) überlagert
- Pitch Shift via Granule Spacing Manipulation (Phase Vocoder oder Stretch-Techniken)
- Sample Loading: WAV, AIFF, MP3 unterstützen (JUCE AudioFormatManager)
- MIDI-Daten: Vollständige Polyphonie-Unterstützung mit Voice Management
- Rendering: Real-time Granular Engine mit optionaler CPU-Optimierung

## Next Steps

- [ ] Create UI mockup (`/dream Sektor` → option 3)
- [ ] Start implementation (`/implement Sektor`)
