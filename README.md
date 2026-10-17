# TRAPERETOR

Industrial-style oneshot sampler and synth, built as a real **VST3 plugin** (JUCE 8) for FL Studio and any other VST3 host.

Drop your oneshots on the plugin window, play them from MIDI, and shape them with 30 knobs. Every knob is automatable from the host.

## Features
- 8 oneshot slots, drag and drop or LOAD button (WAV, AIFF, MP3, FLAC, OGG). Slots are saved with the project (by file path).
- Sample layer: level, start, length, pitch, fine, loop, reverse.
- Oscillator layer: wave, level, unison (1-5), detune, sub, noise.
- Filter (LP/HP/BP) with its own envelope, amp envelope (ADSR), LFO (sin/tri/saw/sqr to amp, filter or pitch).
- Effects: drive, delay, reverb, tone, master.
- 16-voice polyphony, MIDI note input, built-in test keyboard.
- No presets: every sound is built by you.

## Build with GitHub (no setup on your PC)
1. Create a repo and push this folder to it.
2. Open the **Actions** tab. The `Build plugin` workflow runs on every push (or press *Run workflow*).
3. When it finishes, download the artifact `TRAPERETOR-VST3-Windows` (or `-macOS`).

## Install in FL Studio
1. Copy `TRAPERETOR.vst3` to `C:\Program Files\Common Files\VST3\` (macOS: `/Library/Audio/Plug-Ins/VST3/`).
2. In FL Studio: Options > Manage plugins > **Find installed plugins** (Verify plugins).
3. Add it from the plugin database: Installed > Plugin database > VST3 > TRAPERETOR. It is a generator (instrument).

## Build locally
Requires CMake 3.22+ and a C++17 compiler (Visual Studio 2022 on Windows, Xcode on macOS).

    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --config Release

The plugin ends up in `build/TRAPERETOR_artefacts/Release/VST3/`. A Standalone app is built too, for quick testing.

## Notes
- macOS builds are unsigned: you may need to allow them in System Settings > Privacy & Security.
- JUCE is downloaded automatically by CMake and is licensed under AGPLv3/commercial. If you distribute this plugin, check the JUCE license terms.

## License
Source code: MIT (JUCE itself has its own license, see above).
