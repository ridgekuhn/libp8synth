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
	"-s", "--synth", help="synth oscillators to use for patching. default=\"aliased\"", default="aliased")

args = parser.parse_args()

############
# Pre-flight
############
os.makedirs("./out", exist_ok=True)

phasors = [
    ["sample_pulse", "./src/binary/synth/oscillators/hq/phasors/sample_pulse.c"],
    ["sample_sawtooth", "./src/binary/synth/oscillators/hq/phasors/sample_sawtooth.c"],
    ["sample_square", "./src/binary/synth/oscillators/hq/phasors/sample_square.c"],
    ["sample_tilted", "./src/binary/synth/oscillators/hq/phasors/sample_tilted.c"],
    ["sample_triangle", "./src/binary/synth/oscillators/hq/phasors/sample_triangle.c"],
   	# must come after sample_square, sample_triangle
    ["sample_organ", "./src/binary/synth/oscillators/hq/phasors/sample_organ.c"],
]

aliased_oscillators = [
	["mix_wavetable", "./src/binary/synth/oscillators/aliased/mix_wavetable.c"],
	["mix_brown_noise", "./src/binary/synth/oscillators/aliased/mix_brown_noise.c"],
	["mix_pink_noise", "./src/binary/synth/oscillators/aliased/mix_pink_noise.c"],
	["mix_white_noise", "./src/binary/synth/oscillators/aliased/mix_white_noise.c"],
	["mix_noise", "./src/binary/synth/oscillators/aliased/mix_noise.c"],
]

hq_oscillators = [
	["mix_pulse", "./src/binary/synth/oscillators/hq/mix_pulse.c"],
	["mix_square", "./src/binary/synth/oscillators/hq/mix_square.c"],
	["mix_tilted", "./src/binary/synth/oscillators/hq/mix_tilted.c"],
	["mix_triangle", "./src/binary/synth/oscillators/hq/mix_triangle.c"],
	["mix_organ", "./src/binary/synth/oscillators/hq/mix_organ.c"],
	["mix_sawtooth", "./src/binary/synth/oscillators/hq/mix_sawtooth.c"],
]

#######
# Patch
#######
patcher = Patcherex(args.p8path)

patcher.patches.append(InsertFunctionPatch(
	"mix_reverb",
	Path("./src/binary/synth/mix_reverb.c").read_text(),
	compile_opts={"extra_compiler_flags": [
		"-I", os.path.dirname(os.path.realpath(__file__)) + "/src/binary/synth", "-v"]}
))

# patcher.patches.append(InsertFunctionPatch(
# 	"polyblep",
# 	Path("./src/binary/synth/filters/polyblep.c").read_text(),
# 	compile_opts={"extra_compiler_flags": [
# 		"-I", os.path.dirname(os.path.realpath(__file__)) + "/src/binary/synth/filters", "-v"]}
# ))

for phasor in phasors:
	patcher.patches.append(InsertFunctionPatch(
		phasor[0],
		Path(phasor[1]).read_text(),
		compile_opts={"extra_compiler_flags": ["-I", os.path.dirname(
			os.path.realpath(__file__)) + "/src/binary/synth/oscillators/hq/phasors", "-v"]}
	))

for oscillator in aliased_oscillators:
	patcher.patches.append(InsertFunctionPatch(
		oscillator[0],
		Path(oscillator[1]).read_text(),
		compile_opts={"extra_compiler_flags": ["-I", os.path.dirname(
			os.path.realpath(__file__)) + "/src/binary/synth/oscillators/aliased", "-v"]}
	))

for oscillator in hq_oscillators:
	patcher.patches.append(InsertFunctionPatch(
		oscillator[0],
		Path(oscillator[1]).read_text(),
		compile_opts={"extra_compiler_flags": [
			"-I", os.path.dirname(os.path.realpath(__file__)) + "/src/binary/synth/oscillators/hq", "-v"]}
	))

patcher.patches.append(InsertFunctionPatch(
    "mix_reverb",
   	Path("./src/binary/synth/mix_reverb.c").read_text(),
   	compile_opts={"extra_compiler_flags": [
            "-I", os.path.dirname(os.path.realpath(__file__)) + "/src/binary/synth", "-v"]}
))

patcher.patches.append(ModifyFunctionPatch(
	"mix_osc_tick_new",
	Path("./src/binary/synth/mix_osc_tick.c").read_text(),
	compile_opts={"extra_compiler_flags": [
		"-I", os.path.dirname(os.path.realpath(__file__)) + "/src/binary/synth", "-v"]}
))

patcher.apply_patches()

patcher.save_binary(args.out)
