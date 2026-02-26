// COMPATIBILITY: 0.2.7

/*
 * Binary named globals
 */
var advanced_pattern = 0x88048;
var codo_audio_buffer = 0xb555b;
var codo_post_mix_func = 0xb555a;
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
// program sample rate
var codo_state_660 = 0x877df;
// # program channels
var codo_state_664 = 0x877e0;
// sdl sample rate
var codo_state_668 = 0x877e1;
// # sdl channels
var codo_state_672 = 0x877e2;
var fade_vol = 0x88047;
var inside_codo_mixer_callback = 0xb5553;
var last_callback_len = 0x2222;
var note_dx = 0x75a0;
var ms0 = 0x2a064c;
var ps0 = 0x88049;
var xmbuf = 0x1a5d20;

/*
 * Binary anonymous globals
 */
// 0x00b5d0f0 in p8 binary
var AUDIO_CLOCK_MASK = 0xbec46;
// 0x00b5d0f8 in p8 binary
var GLOBAL_BITCRUSH = 0xbec48;
// 0x00b5d0fc in p8 binary
var GLOBAL_DAMPEN = 0xbec49;
// 0x00b5d0f4 in p8 binary
var GLOBAL_REVERB = 0xbec47;
// 0x00b34e28 in p8 binary
var MUSIC_PLAYING = 0xa8192;
// 0x00a74d80 in p8 binary
var MUSIC_VOLUME = 0x878f0;
// 0x00a74dcc in p8 binary
var SOUND_INITIALIZED = 0x87903;
// 0x00a74d7c in p8 binary
var SOUND_VOLUME = 0x878ef;

/*
 * Re-minify function names
 */
// var Jr = sdl_mixer_cb;
// var nn = mix_serial_sound_buffer;
var mix_serial_sound_buffer = nn;
var Iq = codo_mixer_callback;
var Jq = codo_mixer_callback_0;
// var Kq = mix_channel;
var mix_channel = Kq;
// var Jr = sdl_mixer_cb;
var sdl_mixer_cb = Jr;
// var Lq = mix_sfx_channel;
var mix_sfx_channel = Lq;
// var Mq = mix_sfx_tick;
var mix_sfx_tick = Mq;
// var Oq = calculate_osc_state;
var calculate_osc_state = Oq;
var Nq = mix_osc_tick;

/*
 * Un-minify function names
 */
var codo_get_time = Xr;
var codo_memset = wq;
var codo_fwrite_int16 = Ko;
var memcpy = Vv;
var memset = Wv;
var apply_meta_instrument = Qq;
