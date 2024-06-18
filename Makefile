SOURCEDIR := $(abspath $(patsubst %/,%,$(dir $(abspath $(lastword \
	$(MAKEFILE_LIST))))))

NAME := bsnes
JGNAME := $(NAME)-jg

FLAGS := -std=c++11
FLAGS_C99 := -std=c99
FLAGS_CO := -std=c89
FLAGS_GB := -std=gnu11
CPPFLAGS_GB := -DGB_INTERNAL -DGB_DISABLE_CHEATS -DGB_DISABLE_DEBUGGER \
	-D_GNU_SOURCE -DGB_VERSION=\"0.16.6\"

SRCDIR := $(SOURCEDIR)/src

INCLUDES = -I$(SRCDIR)
INCLUDES_JG = -I$(SRCDIR)

LIBS = -lm -lstdc++

LIBS_REQUIRES := samplerate

DOCS := COPYING README

# Object dirs
MKDIRS := deps/byuuML \
	deps/gb \
	deps/libco \
	deps/snes_spc \
	src/coprocessor \
	src/expansion \
	src/processor

override INSTALL_DATA := 1
override INSTALL_EXAMPLE := 0
override INSTALL_SHARED := 0

include $(SOURCEDIR)/version.h
include $(SOURCEDIR)/mk/jg.mk

INCLUDES += $(CFLAGS_SAMPLERATE) -I$(DEPDIR)
LIBS += $(LIBS_SAMPLERATE)

EXT := cpp
LINKER := $(CXX)

# TODO: Use -Wstrict-overflow=5 which is the highest level
WARNINGS_MIN := $(WARNINGS_DEF) -Wformat=2 -Wstrict-overflow=2
WARNINGS_ALL := $(WARNINGS_MIN) -Wmissing-noreturn -Wcast-align -Wcast-qual
WARNINGS_CXX := $(WARNINGS_ALL) -Wnon-virtual-dtor -Woverloaded-virtual
WARNINGS_C := $(WARNINGS_ALL) -Wmissing-prototypes -pedantic

WARNINGS := $(WARNINGS_CXX) -pedantic
WARNINGS_CO := $(WARNINGS_MIN) -Wmissing-prototypes
WARNINGS_ICD := $(WARNINGS_CXX)
WARNINGS_GB := -Wall -Wno-missing-braces -Wno-multichar -Wno-unused-result

CSRCS := deps/gb/apu.c \
	deps/gb/camera.c \
	deps/gb/display.c \
	deps/gb/gb.c \
	deps/gb/joypad.c \
	deps/gb/mbc.c \
	deps/gb/memory.c \
	deps/gb/printer.c \
	deps/gb/random.c \
	deps/gb/rewind.c \
	deps/gb/rumble.c \
	deps/gb/save_state.c \
	deps/gb/sgb.c \
	deps/gb/sm83_cpu.c \
	deps/gb/timing.c \
	deps/gb/workboy.c \
	deps/libco/libco.c \
	deps/snes_spc/spc_dsp.c

CXXSRCS := deps/byuuML/byuuML.cpp \
	src/audio.cpp \
	src/bsmemory.cpp \
	src/bsnes.cpp \
	src/cartridge.cpp \
	src/cheat.cpp \
	src/controller.cpp \
	src/coprocessor/armdsp.cpp \
	src/coprocessor/cx4.cpp \
	src/coprocessor/dip.cpp \
	src/coprocessor/dsp1.cpp \
	src/coprocessor/dsp1emu.cpp \
	src/coprocessor/dsp2.cpp \
	src/coprocessor/dsp4.cpp \
	src/coprocessor/dsp4emu.cpp \
	src/coprocessor/epsonrtc.cpp \
	src/coprocessor/event.cpp \
	src/coprocessor/hitachidsp.cpp \
	src/coprocessor/icd.cpp \
	src/coprocessor/mcc.cpp \
	src/coprocessor/msu1.cpp \
	src/coprocessor/necdsp.cpp \
	src/coprocessor/obc1.cpp \
	src/coprocessor/sa1.cpp \
	src/coprocessor/sdd1.cpp \
	src/coprocessor/sharprtc.cpp \
	src/coprocessor/spc7110.cpp \
	src/coprocessor/st0010.cpp \
	src/coprocessor/superfx.cpp \
	src/cpu.cpp \
	src/dsp.cpp \
	src/emulator.cpp \
	src/expansion/expansion.cpp \
	src/expansion/satellaview.cpp \
	src/heuristics.cpp \
	src/logger.cpp \
	src/markup.cpp \
	src/memory.cpp \
	src/ppu.cpp \
	src/processor/arm7tdmi.cpp \
	src/processor/gsu.cpp \
	src/processor/hg51b.cpp \
	src/processor/spc700.cpp \
	src/processor/upd96050.cpp \
	src/processor/wdc65816.cpp \
	src/random.cpp \
	src/serializer.cpp \
	src/sfc.cpp \
	src/sha256.cpp \
	src/smp.cpp \
	src/sufamiturbo.cpp \
	src/system.cpp

# TODO: Do we need this?
# https://github.com/defparam/21FX
#	src/expansion/21fx.cpp \

JGSRCS := jg.cpp

# Assets
DATA_BASE := Database/boards.bml \
	Database/BSMemory.bml \
	Database/SufamiTurbo.bml \
	Database/SuperFamicom.bml

DATA := $(notdir $(DATA_BASE))
DATA_TARGET := $(DATA:%=$(NAME)/%)

# List of object files
OBJS := $(patsubst %,$(OBJDIR)/%,$(CSRCS:.c=.o) $(CXXSRCS:.cpp=.o) \
	$(OBJS_SAMPLERATE))
OBJS_JG := $(patsubst %,$(OBJDIR)/%,$(JGSRCS:.cpp=.o))

# Dependency commands
BUILD_BML = $(call COMPILE_CXX, $(FLAGS) $(WARNINGS))
BUILD_C99 = $(call COMPILE_C, $(FLAGS_C99) $(WARNINGS_C))
BUILD_CO = $(call COMPILE_C, $(FLAGS_CO) $(WARNINGS_CO))
BUILD_ICD = $(call COMPILE_CXX, $(FLAGS) $(WARNINGS_ICD) $(INCLUDES))
BUILD_GB = $(call COMPILE_C, $(FLAGS_GB) $(WARNINGS_GB) $(CPPFLAGS_GB))

# Core commands
BUILD_JG = $(call COMPILE_CXX, $(FLAGS) $(WARNINGS) $(INCLUDES_JG) $(CFLAGS_JG))
BUILD_MAIN = $(call COMPILE_CXX, $(FLAGS) $(WARNINGS) $(INCLUDES))

.PHONY: $(PHONY)

all: $(TARGET)

# byuuML rules
$(OBJDIR)/deps/byuuML/%.o: $(DEPDIR)/byuuML/%.$(EXT) $(PREREQ)
	$(call COMPILE_INFO,$(BUILD_BML))
	@$(BUILD_BML)

# libco rules
$(OBJDIR)/deps/libco/%.o: $(DEPDIR)/libco/%.c $(PREREQ)
	$(call COMPILE_INFO,$(BUILD_CO))
	@$(BUILD_CO)

# Game Boy rules
$(OBJDIR)/deps/gb/%.o: $(DEPDIR)/gb/%.c $(PREREQ)
	$(call COMPILE_INFO,$(BUILD_GB))
	@$(BUILD_GB)

# snes_spc rules
$(OBJDIR)/deps/snes_spc/%.o: $(DEPDIR)/snes_spc/%.c $(PREREQ)
	$(call COMPILE_INFO,$(BUILD_C99))
	@$(BUILD_C99)

# snes_icd rules
$(OBJDIR)/src/coprocessor/icd.o: $(SRCDIR)/coprocessor/icd.cpp $(PREREQ)
	$(call COMPILE_INFO,$(BUILD_ICD))
	@$(BUILD_ICD)

# SNES rules
$(OBJDIR)/src/%.o: $(SRCDIR)/%.$(EXT) $(PREREQ)
	$(call COMPILE_INFO,$(BUILD_MAIN))
	@$(BUILD_MAIN)

# Data rules
$(DATA_TARGET): $(DATA_BASE:%=$(SOURCEDIR)/%)
	@mkdir -p $(NAME)
	@cp $(subst $(NAME),$(SOURCEDIR)/Database,$@) $(NAME)/

install-data: all
	@mkdir -p $(DESTDIR)$(DATADIR)/jollygood/$(NAME)
	cp $(NAME)/boards.bml $(DESTDIR)$(DATADIR)/jollygood/$(NAME)/
	cp $(NAME)/BSMemory.bml $(DESTDIR)$(DATADIR)/jollygood/$(NAME)/
	cp $(NAME)/SufamiTurbo.bml $(DESTDIR)$(DATADIR)/jollygood/$(NAME)/
	cp $(NAME)/SuperFamicom.bml $(DESTDIR)$(DATADIR)/jollygood/$(NAME)/

install-docs::
	cp $(DEPDIR)/byuuML/LICENSE $(DESTDIR)$(DOCDIR)/LICENSE-byuuML
	cp $(DEPDIR)/gb/LICENSE $(DESTDIR)$(DOCDIR)/LICENSE-gb
	cp $(DEPDIR)/libco/LICENSE $(DESTDIR)$(DOCDIR)/LICENSE-libco
	cp $(DEPDIR)/snes_spc/LICENSE $(DESTDIR)$(DOCDIR)/LICENSE-spc

include $(SOURCEDIR)/mk/rules.mk
