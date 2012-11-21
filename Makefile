# Project name
#------------------------------------------------------------------------------
TARGET=acid

# Objects
#------------------------------------------------------------------------------
OBJS += cmsis/startup_stm32f10x_md_vl.o
OBJS += cmsis/system_stm32f10x.o
OBJS += cmsis/core_cm3.o

OBJS += stm32f10x_it.o

OBJS += pwm.o
OBJS += sine.o
OBJS += drive.o
OBJS += vtimers.o
OBJS += state_machine.o
OBJS += main.o
OBJS += usart.o
OBJS += readline.o


# Tools
#------------------------------------------------------------------------------
PREFIX = arm-none-eabi
AS = $(PREFIX)-gcc
CC = $(PREFIX)-gcc
LD = $(PREFIX)-gcc
CP = $(PREFIX)-objcopy
SZ = $(PREFIX)-objdump
RM = rm

# Assembler flags
#------------------------------------------------------------------------------
AFLAGS += -mthumb -mcpu=cortex-m3
AFLAGS += -Wa,-mapcs-32
AFLAGS += -I. -Icmsis
#AFLAGS += -Wa,-ahls

# Compiler flags
#------------------------------------------------------------------------------
CFLAGS += -mthumb -mcpu=cortex-m3
CFLAGS += -ggdb -g3
CFLAGS += -std=gnu99
CFLAGS += -Wall -pedantic
CFLAGS += -O2
CFLAGS += -I. -Icmsis

# Defines
#------------------------------------------------------------------------------
CFLAGS += -DSTM32F10X_MD_VL
CFLAGS += -DGCC_ARMCM3
CFLAGS += -DVECT_TAB_FLASH

# Linker flags
#------------------------------------------------------------------------------
LDFLAGS += -mcpu=cortex-m3 -mthumb
LDFLAGS += -nostartfiles -nostdlib
LDFLAGS += -Wl,-Map=$(TARGET).map
LDFLAGS += -L ldscripts -T ldscripts/stm32f100rb.ld

# Dependency generation
#------------------------------------------------------------------------------
DEPDIR = .deps
CFLAGS += -MMD -MP -MF $(DEPDIR)/$(@F).d

# Build all
#------------------------------------------------------------------------------
all: $(TARGET).elf

# Cleanup
#------------------------------------------------------------------------------
clean:
	$(RM) -f $(OBJS) *.elf *.hex *.o *.d *.map
	$(RM) -rf $(DEPDIR)

# Show size
#------------------------------------------------------------------------------
size:
	$(SZ) $(TARGET).elf

# Link
#------------------------------------------------------------------------------
$(TARGET).elf: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

# Assemble
#------------------------------------------------------------------------------
%.o: %.S
	$(AS) $(AFLAGS) -c $< -o $@

# Compile
#------------------------------------------------------------------------------
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Include dependency files
#------------------------------------------------------------------------------
-include $(shell mkdir $(DEPDIR) 2>/dev/null) $(wildcard $(DEPDIR)/*)

.PHONY : all clean size
