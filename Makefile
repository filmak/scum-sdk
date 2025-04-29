GENERATOR ?= Ninja
BUILD_TYPE ?= MinSizeRel
SAMPLES ?= \
	hello_world \
	calibration \
	#

RM := rm
MKDIR := mkdir

SAMPLES_SRC_DIR := sdk/samples
SAMPLES_BUILD_DIRS ?= $(foreach project,$(SAMPLES),$(SAMPLES_SRC_DIR)/$(project)/build)

.PHONY: all clean $(SAMPLES)
.DEFAULT_GOAL := all

$(SAMPLES):
	@echo "Building $@"
	cmake -S $(SAMPLES_SRC_DIR)/$@ -B $(SAMPLES_SRC_DIR)/$@/build -G$(GENERATOR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
	cmake --build $(SAMPLES_SRC_DIR)/$@/build --target all --

all: $(SAMPLES)

clean:
	rm -rf $(SAMPLES_BUILD_DIRS)
