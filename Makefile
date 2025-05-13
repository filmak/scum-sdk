GENERATOR ?= Ninja
BUILD_TYPE ?= MinSizeRel
SAMPLES ?= \
	hello_world \
	calibration \
	radio_example_tx \
	radio_example_rx \
	rftimer \
	systick \
	uart \
	#

RM := rm
MKDIR := mkdir

SAMPLES_SRC_DIR := sdk/samples
SAMPLES_SRC_DIRS ?= $(foreach project,$(SAMPLES),$(SAMPLES_SRC_DIR)/$(project))
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

INCLUDES_FILES_OPTS := \
	-wholename "./sdk/bsp/adc.[c|h]" \
	-or -wholename "./sdk/bsp/gpio.[c|h]" \
	-or -wholename "./sdk/bsp/helpers.[c|h]" \
	-or -wholename "./sdk/bsp/ieee_802_15_4.[c|h]" \
	-or -wholename "./sdk/bsp/lighthouse.[c|h]" \
	-or -wholename "./sdk/bsp/matrix.[c|h]" \
	-or -wholename "./sdk/bsp/optical.[c|h]" \
	-or -wholename "./sdk/bsp/rftimer.[c|h]" \
	-or -wholename "./sdk/bsp/scum.[c|h]" \
	-or -wholename "./sdk/bsp/spi.[c|h]" \
	-or -wholename "./sdk/bsp/startup.[c|h]" \
	-or -wholename "./sdk/bsp/syscalls.[c|h]" \
	-or -wholename "./sdk/bsp/systick.[c|h]" \
	-or -wholename "./sdk/bsp/tuning.[c|h]" \
	-or -wholename "./sdk/bsp/uart.[c|h]" \
	-or -wholename "./sdk/bsp/scm3c_hw_interface.[c|h]" \
	-or -wholename "./scum_programmer/nrf-fw/*.[c|h"] \
	#
EXCLUDED_DIRS := \
	./build \
	./scum_programmer/nrf-fw/nRF \
	#
EXCLUDE_OPTS := $(foreach dir,$(sort $(EXCLUDED_DIRS)),-and -not -path "$(dir)/*")
SRCS ?= $(shell find . $(INCLUDES_FILES_OPTS) $(EXCLUDE_OPTS))
CLANG_FORMAT ?= clang-format
CLANG_FORMAT_TYPE ?= file
.PHONY: format format-list check-format

format-list:
	@echo $(SRCS) | tr ' ' '\n' | sort

format:
	@$(CLANG_FORMAT) -i --style=$(CLANG_FORMAT_TYPE) $(SRCS)

check-format:
	@$(CLANG_FORMAT) --dry-run --Werror --style=$(CLANG_FORMAT_TYPE) $(SRCS)
