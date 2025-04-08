REPO_ROOT := $(shell git rev-parse --show-toplevel)
BASE_DIR ?= $(REPO_ROOT)
BUILD_DIR ?= $(CURDIR)/build
LOADER ?= $(REPO_ROOT)/../SCuM-programmer/programmer.py

ifdef DEBUG
	NO_ECHO :=
else
	NO_ECHO := @
endif

SRCS += \
	$(BASE_DIR)/bsp/adc.c \
	$(BASE_DIR)/bsp/gpio.c \
	$(BASE_DIR)/bsp/ieee_802_15_4.c \
	$(BASE_DIR)/bsp/matrix.c \
	$(BASE_DIR)/bsp/optical.c \
	$(BASE_DIR)/bsp/radio.c \
	$(BASE_DIR)/bsp/rftimer.c \
	$(BASE_DIR)/bsp/ring_buffer.c \
	$(BASE_DIR)/bsp/scm3c_hw_interface.c \
	$(BASE_DIR)/bsp/spi.c \
	$(BASE_DIR)/bsp/startup.c \
	$(BASE_DIR)/bsp/syscalls.c \
	$(BASE_DIR)/bsp/tuning.c \
	$(BASE_DIR)/bsp/uart.c \
	#

INCLUDES += \
	$(BASE_DIR)/bsp \
	#

CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
OBJSIZE = arm-none-eabi-size
MKDIR = mkdir

CFLAGS += \
	-mcpu=cortex-m0 \
	-mthumb \
	-std=c17 \
	-Os \
	-fdebug-prefix-map=$(REPO_ROOT)= \
	-g \
	-ffunction-sections \
	-fdata-sections \
	-fshort-wchar \
	#

LDFLAGS += \
	-specs=nano.specs \
	-Wl,--gc-sections \
	-Wl,--print-memory-usage \
	-Wl,--no-wchar-size-warning \
	-Wl,-Map=$(BUILD_DIR)/$(APPLICATION).map \
	-T $(BASE_DIR)/bsp/scum.ld \
	#

DEFINES += \
	#

CFLAGS += $(foreach i,$(INCLUDES),-I$(i))
CFLAGS += $(foreach d,$(DEFINES),-D$(d))

OBJ_DIR = $(BUILD_DIR)/objs
OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))

.PHONY: all flash
all: $(BUILD_DIR)/$(APPLICATION).bin

$(BUILD_DIR):
	$(NO_ECHO)$(MKDIR) -p $(BUILD_DIR)

$(OBJ_DIR):
	$(NO_ECHO)$(MKDIR) -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.c $(OBJ_DIR)
	@echo "Compiling $<"
	$(NO_ECHO)$(MKDIR) -p $(dir $@)
	$(NO_ECHO)$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/$(APPLICATION).bin: $(BUILD_DIR)/$(APPLICATION).elf
	$(NO_ECHO)$(OBJCOPY) $< $@ -O binary
	$(NO_ECHO)$(OBJSIZE) $<

$(BUILD_DIR)/$(APPLICATION).lst: $(BUILD_DIR)/$(APPLICATION).elf $(BUILD_DIR)
	$(NO_ECHO)$(OBJDUMP) -D $< > $@

$(BUILD_DIR)/$(APPLICATION).elf: $(OBJS)
	$(NO_ECHO)$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

load: $(BUILD_DIR)/$(APPLICATION).bin
	$(NO_ECHO)$(LOADER) $(BUILD_DIR)/$(APPLICATION).bin

.PHONY: clean
clean:
	$(NO_ECHO)rm -rf $(BUILD_DIR)
