GENERATOR ?= Ninja
BUILD_TYPE ?= MinSizeRel
SAMPLES ?= \
	hello_world \
	calibration \
	#

RM := rm
MKDIR := mkdir

SAMPLES_BUILD_DIRS ?= $(foreach project,$(SAMPLES),samples/$(project)/build)

.PHONY: all clean $(SAMPLES)
.DEFAULT_GOAL := all

$(SAMPLES):
	@echo "Building $@"
	cmake -S samples/$@ -B samples/$@/build -G$(GENERATOR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
	cmake --build samples/$@/build --target all --

all: $(SAMPLES)

clean:
	rm -rf $(SAMPLES_BUILD_DIRS)
