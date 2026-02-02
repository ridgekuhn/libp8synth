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
    ["sample_pulse", "./src/binary/oscillators/hq/phasors/sample_pulse.c"],
    ["sample_sawtooth", "./src/binary/oscillators/hq/phasors/sample_sawtooth.c"],
    ["sample_square", "./src/binary/oscillators/hq/phasors/sample_square.c"],
    ["sample_tilted", "./src/binary/oscillators/hq/phasors/sample_tilted.c"],
    ["sample_triangle", "./src/binary/oscillators/hq/phasors/sample_triangle.c"],
]

oscillators = [
	["mix_organ", "./src/binary/oscillators/aliased/mix_organ.c"],
	["mix_pulse", "./src/binary/oscillators/hq/mix_pulse.c"],
	# mix_square must come after mix_pulse
	["mix_square", "./src/binary/oscillators/hq/mix_square.c"],
	["mix_sawtooth", "./src/binary/oscillators/hq/mix_sawtooth.c"],
	["mix_tilted", "./src/binary/oscillators/aliased/mix_tilted.c"],
	["mix_triangle", "./src/binary/oscillators/aliased/mix_triangle.c"],
	["mix_wavetable", "./src/binary/oscillators/aliased/mix_wavetable.c"],
	["mix_brown_noise", "./src/binary/oscillators/aliased/mix_brown_noise.c"],
	["mix_pink_noise", "./src/binary/oscillators/aliased/mix_pink_noise.c"],
	["mix_white_noise", "./src/binary/oscillators/aliased/mix_white_noise.c"],
	# mix_noise must come after other noise oscillators
	["mix_noise", "./src/binary/oscillators/aliased/mix_noise.c"],
]

#######
# Patch
#######
p = Patcherex(args.p8path)

# p.patches.append(InsertFunctionPatch("polyblep", Path(
# 	"./src/binary/filters/polyblep.c").read_text()))

for x in phasors:
	p.patches.append(InsertFunctionPatch(x[0], Path(x[1]).read_text()))

for o in oscillators:
	p.patches.append(InsertFunctionPatch(o[0], Path(o[1]).read_text()))

p.patches.append(InsertFunctionPatch(
    "mix_reverb", Path("./src/binary/mix_reverb.c").read_text()))

p.patches.append(ModifyFunctionPatch("mix_osc_tick_new",
                                     Path("./src/binary/mix_osc_tick.c").read_text()))

p.apply_patches()

p.save_binary(args.out)
