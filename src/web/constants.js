// COMPATIBILITY: 0.2.7
// Listed binary addresses are for Linux x86_64 pico8_dyn build

/*
 * Globals
 */
// 0x00798c60 in p8 binary
var last_callback_len = 0x2222;

// 0x00798c80 in p8 binary
var note_dx = 0x75a0;

// 0x00876c00 in p8 binary
var inside_codo_mixer_callback = 0xb5553;
// 0x00876c04 in p8 binary
var advanced_pattern = 0x88048;
// 0x00876c08 in p8 binary
var codo_current_music = undefined;

// 0x00876c10 in p8 binary
var fade_vol = 0x88047;
// 0x00876c14 in p8 binary
var fade1 = 0x88046;
// 0x00876c18 in p8 binary
var fade0 = 0x88045;
// 0x00876c1c in p8 binary
var fade_len = 0x88044;
// 0x00876c20 in p8 binary
var fade_start_t = 0x88043;

// 0x00877048 in p8 binary
var codo_post_mix_func = 0xb555a;

// 0x00877060 in p8 binary
var ramp_buf = 0x1a7da0;

// 0x008770e0 in p8 binary
var codo_audio_buffer = 0xb555b;

// 0x0087714c in p8 binary
var codo_audio_is_locked = 0xb556e;

// UNNAMED
// 0x00a74d7c in p8 binary
var SOUND_VOLUME = 0x878ef;
// UNNAMED
// 0x00a74d80 in p8 binary
var MUSIC_VOLUME = 0x878f0;

// UNNAMED
// 0x00a74dcc in p8 binary
var SOUND_INITIALIZED = 0x87903;

// codo_state = 0x00a77a60 + _xxx in p8 binary
// guest paused
var codo_state_604 = 0x877d2;
// unused in binary
var codo_state_608 = 0x877d3;
// audio playing?
var codo_state_612 = 0x877d4;
// codo_audio_buffer initialized
var codo_state_632 = 0x877d9;
// patterns played
var codo_state_636 = 0x877da;
// current pattern index
var codo_state_640 = 0x877db;
// program sample rate
var codo_state_660 = 0x877df;
// # program channels
var codo_state_664 = 0x877e0;
// sdl sample rate
var codo_state_668 = 0x877e1;
// # sdl channels
var codo_state_672 = 0x877e2;

// 0x00a7b8e0 in p8 binary
var ms0 = 0x2a064c;

// 0x00ab28e0 in p8 binary
var xmbuf = 0x1a5d20;

// 0x00ab4900 in p8 binary
// Voxatron-only
var ps0 = 0x88049;

// UNNAMED
// Voxatron-only
// 0x00b34e28 in p8 binary
var MUSIC_PLAYING = 0xa8192;

// UNNAMED
// 0x00b5d0ec in p8 binary
var MUSIC_CH_MASK = 0xbec45;
// UNNAMED
// 0x00b5d0f0 in p8 binary
var AUDIO_CLOCK_MASK = 0xbec46;
// UNNAMED
// 0x00b5d0f4 in p8 binary
var GLOBAL_REVERB = 0xbec47;
// UNNAMED
// 0x00b5d0f8 in p8 binary
var GLOBAL_BITCRUSH = 0xbec48;
// UNNAMED
// 0x00b5d0fc in p8 binary
var GLOBAL_DAMPEN = 0xbec49;

/*
 * Re-minify function names
 */
// var Jr = sdl_mixer_cb;
// var nn = mix_serial_sound_buffer;
var Iq = codo_mixer_callback;
var Jq = codo_mixer_callback_0;
var Kq = mix_channel;
var Lq = mix_sfx_channel;
var Mq = mix_sfx_tick;
var Oq = calculate_osc_state;
var Pq = get_pattern_ticks_length;
var Qq = apply_meta_instrument;
var Nq = mix_osc_tick;
var fr = codo_play_pico8_song;
var Zq = codo_play_sfx_ex;
var Rr = codo_lock_audio_plat;
var Sr = codo_unlock_audio_plat;
var er = codo_stop_pico8_song;
var gr = codo_update_music_fading;
var Xr = codo_get_time;
var wq = codo_memset;

/*
 * Un-minify function names
 */
var codo_fwrite_int16 = Ko;
var memcpy = Vv;
var memset = Wv;
var SDL_GetTicks = env.i;
var SDL_LockAudio = env.l;
var SDL_UnlockAudio = env.p;
