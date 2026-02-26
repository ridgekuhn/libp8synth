# Pico-8 Host Application Memory

## Notice

* This document is incomplete and may contain inaccuracies.

* This listing is for the binary only. In web exports, memory addressing is optimized and contains no unused bytes. As a result, various pointer addresses are offset differently than in the listing below, with increasingly larger discrepancies as addresses increase.

## Table of Contents

* [LOADED CART ROM](#loaded-cart-rom)

* [MIXER CHANNEL STATES](#mixer-channel-states)

* [GLOBALS](#globals)

## LOADED CART ROM

The host representation of the cart ROM running on the guest. This representation is not 1:1 with the guest ROM/RAM state.

```
+0x0020: SFX
+0xaa20: MUSIC PATTERNS
+0xae20: MUSIC PATTERN LOOP FLAGS
```

### SFX

```
LOADED_CART_ROM + 0x0020, 0xaa00 total, 0x02a8 each

+0x0000: filter byte
+0x0004: editor mode
+0x0008: spd
+0x000c: loop start
+0x0010: loop end
+0x0014: STEP DATA
```

#### STEP DATA

```
+0x0000: pitch
+0x0004: waveform
+0x0008: vol
+0x000c: effect cmd
+0x0010: 1 if step uses meta instrument
```

### MUSIC PATTERNS

```
LOADED_CART_ROM + 0xaa20, 0x0100 total, 0x0004 each

+0x0000: ch 0 sfx idx
+0x0001: ch 1 sfx idx
+0x0002: ch 2 sfx idx
+0x0003: ch 3 sfx idx
```

### MUSIC PATTERN LOOP FLAGS

```
LOADED_CART_ROM + 0xae20, 0x0100 total, 0x0004 each

+0x0000: x & 4 == 4 if stop loop, x & 2 == 2 if end loop, x & 1 == 1 if start loop
```

---

## MIXER CHANNEL STATES

The mixer state accommodates 16 channels, but only channels `8` - `11` are used, for VM channels `0` - `3`.

```
0x00037000 total, 0x3700 each

+0x0000: channel chunk buffer
+0x2000: ?
+0x2004: unused?
+0x2008: unused?
+0x2010: pointer to voxatron channel? (voxatron-only?)
+0x201c: voxatron samples remaining? (voxatron-only?)
+0x2020: pointer to LOADED CART ROM
+0x2024: ?
+0x2028: STEP STATE
+0x2040: TICK BUFFER
+0x21ac: ?
+0x21ae: TICK BUFFER HISTORY
+0x2d20: PATTERN STATE
+0x2d30: OSCILLATOR STATE
+0x2ee0: cur pattern tick
+0x2ee4: total reverb (sfx + oscillator + global), 1 or 2 if enabled
+0x2ee8: total dampen (sfx + oscillator + global), if enabled: 8 if 1, 12 if 2, 15 if 0x5f43 hi + lo bits set
+0x2eec: pattern ticks remaining
+0x2ef0: current HISTORY idx
+0x2ef4: previous mix expire
+0x2ef8: mix expire
+0x2f00: HISTORY
```

### STEP STATE

Shared with TICK BUFFER

```
CHANNEL STATE + 0x2028

0x? total, 0x? ea

+0x0000: pointer to step parent sfx addr in LOADED CART ROM
+0x0004: unused?
+0x0008: current sfx tick
+0x000c: current step tick
+0x0010: spd
+0x0014: ch target & 0x3
+0x0018: ?

+0x0028: pitch num
+0x0030: waveform
+0x0038: vol
+0x0040: effect cmd
```

### TICK BUFFER

Shared with STEP STATE

```
CHANNEL STATE + 0x2040

0x16e total
```

### TICK BUFFER HISTORY

Shared with PATTERN STATE

```
CHANNEL STATE + 0x21ae

0xb70 total, 0x16e ea

+0x0000: tick buffer 0
+0x016e: tick buffer 1
+0x02dc: tick buffer 2
+0x034a: tick buffer 3
+0x04b8: tick buffer 4
+0x0626: tick buffer 5
+0x0694: tick buffer 6
+0x0802: tick buffer 7

+0x0b72: last mixed tick buffer idx
```

### PATTERN STATE

```
CHANNEL STATE + 0x2d20

+0x0000: current pattern tick
+0x0004: chunk_buffer samples remaining
+0x0008: 1 if channel pattern enabled?
+0x000c: current pattern idx
```

### OSCILLATOR STATES

#### Notes

* The primary phasor period is represented by an unsigned 16-bit integer, `0` - `0xffff`. The detune phasor period is represented by a 17-bit integer, `0` - `0x0001ffff`.

* Reverb is the value set by current sfx, or the value at `0x5f41` in guest memory, whichever is greater.

```
CHANNEL STATE + 0x2d30

0x???? total, 0x016c(?) ea

+0x0000: waveform 0-7, or 8 if wavetable bit set
+0x0004: current phase
+0x0008: phase increment (freq << 16 / 22050)
+0x000c: current detune phase
+0x0010: detune phase increment (same as phase increment if no detune)
+0x0014: ?
+0x0018: base sfx step pitch (sliding from)
+0x001c: target vol (sfx step vol << 8) (post-fx)
+0x0020: target pitch (sfx step pitch << 16) (post-fx)
+0x0024: current pitch
+0x0028: current vol
+0x002c: noise seed
+0x0030: noise accumulator 1
+0x0034: noise accumulator 2
+0x0038: sfx step vol
+0x003c: sfx step pitch
+0x0040: sfx step waveform
+0x0044: meta instrument effect cmd
+0x0048: meta instrument filter byte
+0x004c: ?
+0x0050: detune, 1 or 2 if enabled
+0x0054: buzz, 1 if enabled
+0x0058: noiz, 1 if enabled, 2 if (CHANNEL STATE +0x2ee8 < 0xb)
+0x005c: reverb, 1 or 2 if enabled

+0x0160: prev tick pitch
+0x0164: prev tick waveform
+0x0168: prev tick vol
```

### HISTORY

```
CHANNEL STATE + 0x2f00

0x800 total, 0x20 ea

+0x0000: sfx tick
+0x0004: step tick
+0x0008: pointer to step parent sfx addr in LOADED CART ROM
+0x0010: step idx
+0x0014: pattern idx
+0x0018: # patterns played
```

# GLOBALS

## Named

### Key / Value Pairs

#### `advanced_pattern`

`1` if pattern boundary crossed in mix_sfx_tick() call

#### `codo_audio_buffer`

Pointer to main audio buffer

#### `codo_post_mix_func`

Pointer to SDL postmix callback, set to `mix_serial_sound_buffer()` at init.

#### `fade_vol`

Current fade in / out gain.

#### `inside_codo_mixer_callback`

`1` if SDL mixer callback is running

#### `last_callback_len`

Chunk length of previous SDL mixer callback

#### `ms0`

Pointer to [MIXER CHANNEL STATES](#mixer-channel-states)

#### `ps0`

Pointer to Voxatron audio state (Voxatron only)

#### `ramp_buf`

Pointer to ramp buffer

#### `xmbuf`

Pointer to xm audio buffer (Voxatron only)

#### `note_dx`

Note lookup table

```
+0x0000: 0000020b 523Hz C 5
+0x0004: 0000022a 554Hz C#5
+0x0008: 0000024b 587Hz D 5
+0x000c: 0000026e 622Hz D#5
+0x0010: 00000293 659Hz E 5
+0x0014: 000002ba 698Hz F 5
+0x0018: 000002e4 740Hz F#5
+0x001c: 00000310 784Hz G 5
+0x0020: 0000033f 831Hz G#5
+0x0024: 00000370 880Hz A 5
+0x0028: 000003a4 932Hz A#5
+0x002c: 000003d8 984Hz B 5
```

#### `codo_state`

Various virtual machine states

```
+604: guest paused
+608: unused?
+612: audio playing
+632: codo_audio_buffer initialized
+636: # patterns played
+660: program sample rate
+664: # program channels
+668: sdl sample rate
+672: # sdl channels
```
