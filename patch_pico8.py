import argparse
from patcherex2 import InsertFunctionPatch, ModifyFunctionPatch, Patcherex
from pathlib import Path
import os

################
# Parse CLI Args
################
parser = argparse.ArgumentParser(
    prog="libp8synth patcher"
)

parser.add_argument("p8path", help="path to pico8 executable")

parser.add_argument(
	"-o, ", "--out", help="output path. default=\"./out/pico8_patched\"", default="./out/pico8_patched")

parser.add_argument(
	"-s", "--synth", help="synth mixers to use for patching. default=\"aliased\"", default="aliased")

args = parser.parse_args()

############
# Pre-flight
############
os.makedirs("./out", exist_ok=True)

hq_phasors = [
    ["phasor_hq_pulse", "./src/binary/audio/synth/phasors/hq/phasor_hq_pulse.c"],
    ["phasor_hq_sawtooth", "./src/binary/audio/synth/phasors/hq/phasor_hq_sawtooth.c"],
    ["phasor_hq_square", "./src/binary/audio/synth/phasors/hq/phasor_hq_square.c"],
    ["phasor_hq_tilted", "./src/binary/audio/synth/phasors/hq/phasor_hq_tilted.c"],
    ["phasor_hq_triangle", "./src/binary/audio/synth/phasors/hq/phasor_hq_triangle.c"],
   	# must come after phasor_hq_square, phasor_hq_triangle
    ["phasor_hq_organ", "./src/binary/audio/synth/phasors/hq/phasor_hq_organ.c"],
]

hq_oscillators = [
    ["osc_hq_pulse", "./src/binary/audio/synth/oscillators/hq/osc_hq_pulse.c"],
    ["osc_hq_sawtooth", "./src/binary/audio/synth/oscillators/hq/osc_hq_sawtooth.c"],
    ["osc_hq_square", "./src/binary/audio/synth/oscillators/hq/osc_hq_square.c"],
    ["osc_hq_triangle", "./src/binary/audio/synth/oscillators/hq/osc_hq_triangle.c"],
]

aliased_mixers = [
	["mix_wavetable", "./src/binary/audio/synth/mixers/aliased/mix_wavetable.c"],
	["mix_brown_noise", "./src/binary/audio/synth/mixers/aliased/mix_brown_noise.c"],
	["mix_pink_noise", "./src/binary/audio/synth/mixers/aliased/mix_pink_noise.c"],
	["mix_white_noise", "./src/binary/audio/synth/mixers/aliased/mix_white_noise.c"],
	["mix_noise", "./src/binary/audio/synth/mixers/aliased/mix_noise.c"],
]

hq_mixers = [
	["mix_pulse", "./src/binary/audio/synth/mixers/hq/mix_pulse.c"],
	["mix_square", "./src/binary/audio/synth/mixers/hq/mix_square.c"],
	["mix_tilted", "./src/binary/audio/synth/mixers/hq/mix_tilted.c"],
	["mix_triangle", "./src/binary/audio/synth/mixers/hq/mix_triangle.c"],
	["mix_organ", "./src/binary/audio/synth/mixers/hq/mix_organ.c"],
	["mix_sawtooth", "./src/binary/audio/synth/mixers/hq/mix_sawtooth.c"],
]

#######
# Patch
#######
patcher = Patcherex(args.p8path)

patcher.patches.append(InsertFunctionPatch(
	"mix_reverb",
	Path("./src/binary/audio/synth/mix_reverb.c").read_text(),
	compile_opts={"extra_compiler_flags": [
		"-I", os.path.dirname(os.path.realpath(__file__)) + "/src/binary/audio/synth", "-v"]}
))

# patcher.patches.append(InsertFunctionPatch(
# 	"polyblep",
# 	Path("./src/binary/audio/synth/filters/polyblep.c").read_text(),
# 	compile_opts={"extra_compiler_flags": [
# 		"-I", os.path.dirname(os.path.realpath(__file__)) + "/src/binary/audio/synth/filters", "-v"]}
# ))

for phasor in hq_phasors:
	patcher.patches.append(InsertFunctionPatch(
		phasor[0],
		Path(phasor[1]).read_text(),
		compile_opts={"extra_compiler_flags": ["-I", os.path.dirname(
			os.path.realpath(__file__)) + "/src/binary/audio/synth/phasors/hq", "-v"]}
	))

for oscillator in hq_oscillators:
	patcher.patches.append(InsertFunctionPatch(
		oscillator[0],
		Path(oscillator[1]).read_text(),
		compile_opts={"extra_compiler_flags": ["-I", os.path.dirname(
			os.path.realpath(__file__)) + "/src/binary/audio/synth/oscillators/hq", "-v"]}
	))

for mixer in aliased_mixers:
	patcher.patches.append(InsertFunctionPatch(
		mixer[0],
		Path(mixer[1]).read_text(),
		compile_opts={"extra_compiler_flags": ["-I", os.path.dirname(
			os.path.realpath(__file__)) + "/src/binary/audio/synth/mixers/aliased", "-v"]}
	))

for mixer in hq_mixers:
	patcher.patches.append(InsertFunctionPatch(
		mixer[0],
		Path(mixer[1]).read_text(),
		compile_opts={"extra_compiler_flags": [
			"-I", os.path.dirname(os.path.realpath(__file__)) + "/src/binary/audio/synth/mixers/hq", "-v"]}
	))

patcher.patches.append(InsertFunctionPatch(
    "mix_reverb",
   	Path("./src/binary/audio/synth/mix_reverb.c").read_text(),
   	compile_opts={"extra_compiler_flags": [
            "-I", os.path.dirname(os.path.realpath(__file__)) + "/src/binary/audio/synth", "-v"]}
))

patcher.patches.append(ModifyFunctionPatch(
	"mix_osc_tick_new",
	Path("./src/binary/audio/synth/mix_osc_tick.c").read_text(),
	compile_opts={"extra_compiler_flags": [
		"-I", os.path.dirname(os.path.realpath(__file__)) + "/src/binary/audio/synth", "-v"]}
))

patcher.apply_patches()

patcher.save_binary(args.out)
