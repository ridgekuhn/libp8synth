NAME=z8synth
CC=cc
CFLAGS=$(INC)
OUT=./out
SRC=./src/binary
INC=-I $(SRC)

default: main

help:
	@echo "usage: make {default|clean|help}"

out_dir:
	mkdir -p $(OUT)

#########
# GLOBALS
#########
globals: $(OUT)/globals.o out_dir

$(OUT)/globals.o: $(SRC)/globals.h
	$(CC) $(CFLAGS) -o $@ -c $(SRC)/globals.c

######
# Cart
######
cart: cart_dir $(CART_OBJ)

CART_OBJ=$(patsubst $(SRC)/cart/%.c,$(OUT)/cart/%.o,$(wildcard $(SRC)/cart/*.c))

cart_dir:
	mkdir -p $(OUT)/cart

$(CART_OBJ): $(OUT)/cart/%.o : $(SRC)/cart/%.c $(SRC)/cart/%.h cart_dir globals
	$(CC) $(CFLAGS) -o $@ -c $<

#####
# CLI
#####
cli_dir:
	mkdir -p $(OUT)/cli

CLI_OBJ=$(patsubst $(SRC)/cli/%.c,$(OUT)/cli/%.o,$(wildcard $(SRC)/cli/*.c))

$(CLI_OBJ): $(OUT)/cli/%.o : $(SRC)/cli/%.c $(SRC)/cli/%.h cli_dir globals cart
	$(CC) $(CFLAGS) -o $@ -c $<

#######
# Synth
#######
# HQ Phasors
PHASOR_HQ_DIR=audio/synth/phasors/hq

phasor_hq_dir:
	mkdir -p $(OUT)/$(PHASOR_HQ_DIR)

PHASOR_HQ_OBJ=$(patsubst $(SRC)/$(PHASOR_HQ_DIR)/%.c,$(OUT)/$(PHASOR_HQ_DIR)/%.o,$(wildcard $(SRC)/$(PHASOR_HQ_DIR)/*.c))

$(PHASOR_HQ_OBJ): $(OUT)/$(PHASOR_HQ_DIR)/%.o : $(SRC)/$(PHASOR_HQ_DIR)/%.c $(SRC)/$(PHASOR_HQ_DIR)/%.h phasor_hq_dir
	$(CC) $(CFLAGS) -o $@ -c $<

phasors: $(PHASOR_HQ_OBJ)

######
# Main
######
main: $(CART_OBJ) $(CLI_OBJ) $(OUT)/main.o $(OUT)/globals.o
	$(CC) $(CFLAGS) -o $(OUT)/$(NAME) $^ -lm

$(OUT)/main.o: $(SRC)/main.h out_dir globals
	mkdir -p $(OUT)
	$(CC) $(CFLAGS) -o $@ -c $(SRC)/main.c

#######
# Clean
#######
clean:
	$(shell find ./$(OUT) -type f -name "*.o" | xargs -I{} rm {})
	rm -r $(OUT)/cart
	rm -r $(OUT)/cli
	rm $(OUT)/$(NAME)
