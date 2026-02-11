# libp8synth

A library for community patching of [Pico-8](https://www.lexaloffle.com/pico-8.php)'s synthesizer.

---

Pico-8 is the property of [Lexaloffle Games LLP](https://www.lexaloffle.com/). This project is intended for educational purposes only.

---

### Project State

Pico-8's oscillators output "naive" waveforms, resulting in [aliasing distortion](https://en.wikipedia.org/wiki/Aliasing). Some output signals also contain a [DC offset](https://en.wikipedia.org/wiki/DC_bias#Audio). The goal of this project is to address these issues by providing patches for Pico-8's synthesizer.

* [Aliased oscillators](/src/binary/oscillators/aliased) are reimplemented.

* [High-quality oscillators](/src/binary/oscillators/hq) are implemented (tone-generating oscillators only). A fast CPU is recommended for realtime playback.

* [Fast oscillators](/src/binary/fast), such as using [polyblep](https://www.kvraudio.com/forum/viewtopic.php?t=375517) are partially implemented. Oversampling, or other algorithms should be explored. Pull requests welcome.

## Development

Please make pull requests against the [dev branch](https://github.com/ridgekuhn/libz8synth/tree/dev).

### Mixer Logic

The Pico-8 host uses SDL2 for audio output, supplied by an [SDL_AudioCallback](https://wiki.libsdl.org/SDL2/SDL_AudioCallback) function.

On SDL audio initialization, Pico-8 is capable of requesting either 2-channel stereo output at 44100Hz, or 1-channel mono output at 22050Hz; though internally, the mixing buffer is always a 1-channel 22050Hz stream, and most references to it and related calculations are hard-coded.

Inside the mixer callback:

* The host loops over the 4 guest audio channels. 

	* A function to update channel oscillator state is called. Each oscillator holds state data for two phasors, one for the primary phase and one for the `detune` phase. Phase is expressed as a 16-bit integer for the primary phasor, and a 17-bit integer for the detune phasor.

	* After oscillator state is updated, [mix_osc_tick()](/src/binary/mix_osc_tick.c) is called. Each channel tick buffer holds 183 signed 16-bit samples, totalling 366 bytes.

* Once the channel loop is complete, each channel's tick buffer is mixed into to the main audio stream.

Finally, the [undocumented PCM channel](https://www.lexaloffle.com/bbs/?tid=41991) is mixed in via SDL's [Mix_SetPostMix](https://wiki.libsdl.org/SDL2_mixer/Mix_SetPostMix) hook.

See [memory.md](/memory.md) for a list of known host application memory states.

## Patching

### For Linux Developers

A [Patcherex2](https://purseclab.github.io/Patcherex2/) script ([patch_pico8.py](/patch_pico8.py)) is provided for convenience. Edit this file and replace oscillator functions as needed.

You may also need to install dependencies as required by the original [Patcherex](https://github.com/angr/patcherex).

```shell
python3 patch_pico8.py [/path/to/pico8/executable]
```

### For Windows/MacOS Developers

Pull requests welcome.

### For Web Developers

See [/src/web](/src/web).
