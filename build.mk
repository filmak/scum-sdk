REPO_ROOT := $(shell git rev-parse --show-toplevel)
BASE_DIR ?= $(REPO_ROOT)
BUILD_DIR ?= $(CURDIR)/build

ifdef DEBUG
	NO_ECHO :=
else
	NO_ECHO := @
endif

SRCS += \
	$(BASE_DIR)/bsp/adc.c \
	$(BASE_DIR)/bsp/gpio.c \
	$(BASE_DIR)/bsp/ieee_802_15_4.c \
	$(BASE_DIR)/bsp/lighthouse.c \
	$(BASE_DIR)/bsp/matrix.c \
	$(BASE_DIR)/bsp/optical.c \
	$(BASE_DIR)/bsp/radio.c \
	$(BASE_DIR)/bsp/retarget.c \
	$(BASE_DIR)/bsp/rftimer.c \
	$(BASE_DIR)/bsp/ring_buffer.c \
	$(BASE_DIR)/bsp/scm3c_hw_interface.c \
	$(BASE_DIR)/bsp/spi.c \
	$(BASE_DIR)/bsp/startup.c \
	$(BASE_DIR)/bsp/tuning.c \
	$(BASE_DIR)/bsp/uart.c \
	#

INCLUDES += \
	$(BASE_DIR)/bsp \
	$(BASE_DIR)/CMSIS/Include \
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
	-Wall \
	-Werror \
	-std=c11 \
	-O0 \
	-fdebug-prefix-map=$(REPO_ROOT)= \
	-g \
	-ffreestanding \
	-ffunction-sections \
	-fdata-sections \
	#

LDFLAGS += \
	-specs=nano.specs \
	-Wl,--gc-sections \
	-Wl,--print-memory-usage \
	-Wl,-Map=$(BUILD_DIR)/$(PROJECT).map \
	-T $(BASE_DIR)/scum.ld \
	#

DEFINES += \
	CYCLE_MODE \
	#

CFLAGS += $(foreach i,$(INCLUDES),-I$(i))
CFLAGS += $(foreach d,$(DEFINES),-D$(d))

OBJ_DIR = $(BUILD_DIR)/objs
OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))

.PHONY: all
all: $(BUILD_DIR)/$(PROJECT).bin

$(BUILD_DIR):
	@echo $(NO_ECHO)$(MKDIR) -p $(BUILD_DIR)

$(OBJ_DIR):
	@echo $(NO_ECHO)$(MKDIR) -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.c $(OBJ_DIR)
	@echo "Compiling $<"
	@echo $(NO_ECHO)$(MKDIR) -p $(dir $@)
	@echo $(NO_ECHO)$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).lst
	@echo $(OBJCOPY) $< $@ -O binary
	@echo $(OBJSIZE) $<

$(BUILD_DIR)/$(PROJECT).lst: $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)
	@echo $(OBJDUMP) -D $< > $@

$(BUILD_DIR)/$(PROJECT).elf: $(OBJS)
	@echo "Linking $@"
	@echo $(NO_ECHO)$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@


.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
