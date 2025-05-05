GENERATOR ?= Ninja
BUILD_TYPE ?= MinSizeRel
SAMPLES ?= \
	hello_world \
	calibration \
	radio_example_tx \
	radio_example_rx \
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


DOCKER_IMAGE ?= aabadie/dotbot:latest
PACKAGES_DIR_OPT ?=
SEGGER_DIR ?= /opt/segger
BUILD_CONFIG ?= Release

.PHONY: scum-programmer scum-programmer-in-docker

scum-programmer:
	@echo "\e[1mBuilding $@ application\e[0m"
	"$(SEGGER_DIR)/bin/emBuild" scum_programmer/nrf-fw/scum-programmer.emProject -project $@ -config $(BUILD_CONFIG) $(PACKAGES_DIR_OPT) -rebuild -verbose
	@echo "\e[1mDone\e[0m\n"

scum-programmer-in-docker:
	docker run --rm -i \
		-e BUILD_CONFIG="$(BUILD_CONFIG)" \
		-e PACKAGES_DIR_OPT="-packagesdir $(SEGGER_DIR)/packages" \
		-e SEGGER_DIR="$(SEGGER_DIR)" \
		-v $(PWD):/dotbot $(DOCKER_IMAGE) \
		make scum-programmer
