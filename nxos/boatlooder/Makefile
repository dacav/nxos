.PHONY : all clean objects cleandeps

TOOLCHAIN_PREFIX := arm-elf-
CC := $(TOOLCHAIN_PREFIX)gcc
LD := $(TOOLCHAIN_PREFIX)ld
AS := $(TOOLCHAIN_PREFIX)as
OBJCOPY := $(TOOLCHAIN_PREFIX)objcopy

# compilation flags
LIBGCC := $(shell $(CC) -print-libgcc-file-name -mthumb-interwork)
CFLAGS := -mcpu=arm7tdmi -Os -Wextra -Wall -Werror -Wno-div-by-zero \
          -Wfloat-equal -Wshadow -Wpointer-arith -Wbad-function-cast \
          -Wmissing-prototypes -ffreestanding -fsigned-char \
          -ffunction-sections -std=gnu99 -fdata-sections \
          -fomit-frame-pointer -msoft-float -mthumb-interwork -mthumb \
          -Wall -Werror -Os -Wa,-mcpu=arm7tdmi,-mfpu=softfpa,-mthumb-interwork

ROOT := ../

ifdef LINKERSCRIPT
    LDFLAGS := -T$(LINKERSCRIPT)
else
    LDFLAGS := -T$(ROOT)boatlooder/boatlooder.ld
endif
LDFLAGS += --no-check-sections

ASFLAGS := -mcpu=arm7tdmi -mthumb-interwork

ifndef APPKERNEL
  APPKERNEL := tests
endif
SYSTEM := systems/$(APPKERNEL)

SOURCES := $(shell find $(ROOT)$(SYSTEM) $(ROOT)base -regex '.*\.\(c\|S\|s\)')
OBJECTS := $(addsuffix .o, $(basename $(SOURCES)))
DEPS := $(OBJECTS:.o=.d)
ELF_OBJECT := $(ROOT)$(SYSTEM)/$(APPKERNEL)_boatlood

# library path flags
PATHS := -I$(ROOT) -I$(ROOT)$(SYSTEM)
CFLAGS += $(PATHS)

all: objects
	$(LD) $(LDFLAGS) $(OBJECTS) $(LIBGCC) -o $(ELF_OBJECT)

objects:
	@patch -p1 < ./init.patch
	$(MAKE) $(OBJECTS)
	@patch -p1 -R < ./init.patch

clean: cleandeps
	@echo "Removing object files..."
	@rm -f $(OBJECTS) $(ELF_OBJECT)

cleandeps:
	@echo "Removing dependency files..."
	@find $(ROOT) -name '*.d' | xargs rm

%.d: %.c
	$(CC) $(PATHS) -MM $< -MF $@
-include $(DEPS)
