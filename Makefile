SOURCEDIR := $(abspath $(patsubst %/,%,$(dir $(abspath $(lastword \
	$(MAKEFILE_LIST))))))

AR ?= ar
CC ?= cc
CXX ?= c++
CFLAGS ?= -O2
CXXFLAGS ?= -O2
FLAGS := -std=c++11
FLAGS_C99 := -std=c99
FLAGS_CO := -std=c89
FLAGS_GB := -std=gnu11
CPPFLAGS_GB := -DGB_INTERNAL -DGB_DISABLE_CHEATS -DGB_DISABLE_DEBUGGER \
	-D_GNU_SOURCE -DGB_VERSION=\"0.15.7\"
INCLUDES := -I$(SOURCEDIR)/deps -I$(SOURCEDIR)/src

PKG_CONFIG ?= pkg-config
CFLAGS_JG := $(shell $(PKG_CONFIG) --cflags jg)

# TODO: Use -Wstrict-overflow=5 which is the highest level
WARNINGS_MIN := -Wall -Wextra -Wshadow -Wformat=2 -Wstrict-overflow=2 \
	-Wmissing-declarations
WARNINGS_ALL := $(WARNINGS_MIN) -Wmissing-noreturn -Wcast-align -Wcast-qual
WARNINGS_CXX := $(WARNINGS_ALL) -Wnon-virtual-dtor -Woverloaded-virtual
WARNINGS_C := $(WARNINGS_ALL) -Wmissing-prototypes -pedantic

WARNINGS := $(WARNINGS_CXX) -pedantic
WARNINGS_CO := $(WARNINGS_MIN) -Wmissing-prototypes
WARNINGS_ICD := $(WARNINGS_CXX)
WARNINGS_GB := -Wno-multichar -Wno-unused-result

LIBS := -lm -lstdc++
PIC := -fPIC
SHARED := $(PIC)

NAME := bsnes
PREFIX ?= /usr/local
EXEC_PREFIX ?= $(PREFIX)
LIBDIR ?= $(EXEC_PREFIX)/lib
DATAROOTDIR ?= $(PREFIX)/share
DATADIR ?= $(DATAROOTDIR)
DOCDIR ?= $(DATAROOTDIR)/doc/$(NAME)

LIBPATH := $(LIBDIR)/jollygood

DISABLE_MODULE ?= 0
ENABLE_STATIC_JG ?= 0
USE_VENDORED_SAMPLERATE ?= 0

UNAME := $(shell uname -s)
ifeq ($(UNAME), Darwin)
	LIBRARY := $(NAME).dylib
else ifeq ($(OS), Windows_NT)
	LIBRARY := $(NAME).dll
else
	LIBRARY := $(NAME).so
endif

ifeq ($(UNAME), Darwin)
	SHARED += -dynamiclib -Wl,-undefined,error
else
	SHARED += -shared -Wl,--no-undefined
endif

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
	src/interface.cpp \
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
	src/system.cpp \
	jg.cpp

# TODO: Do we need this?
# https://github.com/defparam/21FX
#	src/expansion/21fx.cpp \

ifneq ($(USE_VENDORED_SAMPLERATE), 0)
	CFLAGS_SAMPLERATE := -I$(SOURCEDIR)/deps/libsamplerate
	LIBS_SAMPLERATE :=
	CSRCS += deps/libsamplerate/samplerate.c \
		deps/libsamplerate/src_linear.c \
		deps/libsamplerate/src_sinc.c \
		deps/libsamplerate/src_zoh.c
else
	CFLAGS_SAMPLERATE := $(shell $(PKG_CONFIG) --cflags samplerate)
	LIBS_SAMPLERATE := $(shell $(PKG_CONFIG) --libs samplerate)
endif

INCLUDES += $(CFLAGS_SAMPLERATE)
LIBS += $(LIBS_SAMPLERATE)

# Assets
ASSETS := Database/boards.bml \
	Database/BSMemory.bml \
	Database/SufamiTurbo.bml \
	Database/SuperFamicom.bml

ASSETS_BASE := $(notdir $(ASSETS))
ASSETS_TARGET := $(ASSETS_BASE:%=$(NAME)/%)

# Desktop File
DESKTOP := $(NAME)-jg.desktop

DESKTOP_TARGET := $(NAME)/$(DESKTOP)

# Icons
ICONS := $(wildcard $(SOURCEDIR)/icons/*.png) $(SOURCEDIR)/icons/$(NAME).svg

ICONS_BASE := $(notdir $(ICONS))
ICONS_TARGET := $(ICONS_BASE:%=$(NAME)/icons/%)

# Object dirs
MKDIRS := deps/byuuML \
	deps/gb \
	deps/libco \
	deps/libsamplerate \
	deps/snes_spc \
	src/coprocessor \
	src/expansion \
	src/processor

OBJDIR := objs

# List of object files
OBJS := $(patsubst %,$(OBJDIR)/%,$(CSRCS:.c=.o) $(CXXSRCS:.cpp=.o))

# Library targets
TARGET :=
TARGET_MODULE := $(NAME)/$(LIBRARY)
TARGET_STATIC_JG := $(NAME)/lib$(NAME)-jg.a

ifeq ($(DISABLE_MODULE), 0)
	TARGET += $(TARGET_MODULE)
endif

ifneq ($(ENABLE_STATIC_JG), 0)
	TARGET += $(DESKTOP_TARGET) $(ICONS_TARGET) $(NAME)/jg-static.mk
endif

# Compiler commands
COMPILE = $(strip $(1) $(CPPFLAGS) $(PIC) $(2) -c $< -o $@)
COMPILE_C = $(call COMPILE, $(CC) $(CFLAGS), $(1))
COMPILE_CXX = $(call COMPILE, $(CXX) $(CXXFLAGS), $(1))

# Info command
COMPILE_INFO = $(info $(subst $(SOURCEDIR)/,,$(1)))

# Dependency commands
BUILD_BML = $(call COMPILE_CXX, $(FLAGS) $(WARNINGS))
BUILD_C99 = $(call COMPILE_C, $(FLAGS_C99) $(WARNINGS_C))
BUILD_CO = $(call COMPILE_C, $(FLAGS_CO) $(WARNINGS_CO))
BUILD_ICD = $(call COMPILE_CXX, $(FLAGS) $(WARNINGS_ICD) $(INCLUDES))
BUILD_GB = $(call COMPILE_C, $(FLAGS_GB) $(WARNINGS_GB) $(CPPFLAGS_GB))

# Core commands
BUILD_JG = $(call COMPILE_CXX, $(FLAGS) $(WARNINGS) $(INCLUDES) $(CFLAGS_JG))
BUILD_MAIN = $(call COMPILE_CXX, $(FLAGS) $(WARNINGS) $(INCLUDES))

.PHONY: all clean install install-strip uninstall

all: $(ASSETS_TARGET) $(TARGET)

# byuuML rules
$(OBJDIR)/deps/byuuML/%.o: $(SOURCEDIR)/deps/byuuML/%.cpp $(OBJDIR)/.tag
	$(call COMPILE_INFO, $(BUILD_BML))
	@$(BUILD_BML)

# libco rules
$(OBJDIR)/deps/libco/%.o: $(SOURCEDIR)/deps/libco/%.c $(OBJDIR)/.tag
	$(call COMPILE_INFO, $(BUILD_CO))
	@$(BUILD_CO)

# libsamplerate rules
$(OBJDIR)/deps/libsamplerate/%.o: $(SOURCEDIR)/deps/libsamplerate/%.c $(OBJDIR)/.tag
	$(call COMPILE_INFO, $(BUILD_C99))
	@$(BUILD_C99)

# Game Boy rules
$(OBJDIR)/deps/gb/%.o: $(SOURCEDIR)/deps/gb/%.c $(OBJDIR)/.tag
	$(call COMPILE_INFO, $(BUILD_GB))
	@$(BUILD_GB)

# snes_spc rules
$(OBJDIR)/deps/snes_spc/%.o: $(SOURCEDIR)/deps/snes_spc/%.c $(OBJDIR)/.tag
	$(call COMPILE_INFO, $(BUILD_C99))
	@$(BUILD_C99)

# snes_icd rules
$(OBJDIR)/src/coprocessor/icd.o: $(SOURCEDIR)/src/coprocessor/icd.cpp $(OBJDIR)/.tag
	$(call COMPILE_INFO, $(BUILD_ICD))
	@$(BUILD_ICD)

# SNES rules
$(OBJDIR)/src/%.o: $(SOURCEDIR)/src/%.cpp $(OBJDIR)/.tag
	$(call COMPILE_INFO, $(BUILD_MAIN))
	@$(BUILD_MAIN)

# Shim rules
$(OBJDIR)/%.o: $(SOURCEDIR)/%.cpp $(OBJDIR)/.tag
	$(call COMPILE_INFO, $(BUILD_JG))
	@$(BUILD_JG)

$(OBJDIR)/.tag:
	@mkdir -p -- $(patsubst %,$(OBJDIR)/%,$(MKDIRS))
	@touch $@

$(TARGET_MODULE): $(OBJS)
	@mkdir -p $(NAME)
	$(strip $(CXX) -o $@ $^ $(LDFLAGS) $(LIBS) $(SHARED))

$(TARGET_STATIC_JG): $(OBJS)
	@mkdir -p $(NAME)
	$(AR) rcs $@ $^

$(ASSETS_TARGET): $(ASSETS:%=$(SOURCEDIR)/%)
	@mkdir -p $(NAME)
	@cp $(subst $(NAME),$(SOURCEDIR)/Database,$@) $(NAME)/

$(DESKTOP_TARGET): $(SOURCEDIR)/$(DESKTOP)
	@mkdir -p $(NAME)
	@cp $< $@

$(ICONS_TARGET): $(ICONS)
	@mkdir -p $(NAME)/icons
	@cp $(subst $(NAME)/icons,$(SOURCEDIR)/icons,$@) $(NAME)/icons/

$(NAME)/jg-static.mk: $(TARGET_STATIC_JG)
	@printf '%s\n%s\n%s\n%s\n' 'NAME := $(NAME)-jg' \
		'ASSETS := $(ASSETS_BASE)' 'ICONS := $(ICONS_BASE)' \
		'LIBS_STATIC := $(strip $(LIBS))' > $@

clean:
	rm -rf $(OBJDIR) $(NAME)

ifeq ($(DISABLE_MODULE), 0)
install: all
	@mkdir -p $(DESTDIR)$(DATADIR)/jollygood/$(NAME)
	@mkdir -p $(DESTDIR)$(DOCDIR)
	@mkdir -p $(DESTDIR)$(LIBPATH)
	cp $(TARGET_MODULE) $(DESTDIR)$(LIBPATH)/
	cp $(NAME)/boards.bml $(DESTDIR)$(DATADIR)/jollygood/$(NAME)/
	cp $(NAME)/BSMemory.bml $(DESTDIR)$(DATADIR)/jollygood/$(NAME)/
	cp $(NAME)/SufamiTurbo.bml $(DESTDIR)$(DATADIR)/jollygood/$(NAME)/
	cp $(NAME)/SuperFamicom.bml $(DESTDIR)$(DATADIR)/jollygood/$(NAME)/
	cp $(SOURCEDIR)/COPYING $(DESTDIR)$(DOCDIR)
	cp $(SOURCEDIR)/README $(DESTDIR)$(DOCDIR)
	cp $(SOURCEDIR)/deps/byuuML/LICENSE $(DESTDIR)$(DOCDIR)/LICENSE-byuuML
	cp $(SOURCEDIR)/deps/gb/LICENSE $(DESTDIR)$(DOCDIR)/LICENSE-gb
	cp $(SOURCEDIR)/deps/libco/LICENSE $(DESTDIR)$(DOCDIR)/LICENSE-libco
	cp $(SOURCEDIR)/deps/snes_spc/license.txt $(DESTDIR)$(DOCDIR)/LICENSE-spc
ifneq ($(USE_VENDORED_SAMPLERATE), 0)
	cp $(SOURCEDIR)/deps/libsamplerate/COPYING \
		$(DESTDIR)$(DOCDIR)/COPYING-libsamplerate
endif

install-strip: install
	strip $(DESTDIR)$(LIBPATH)/$(LIBRARY)
else
install: all
	@echo 'Nothing to install'

install-strip: install
endif

uninstall:
	rm -rf $(DESTDIR)$(DATADIR)/jollygood/$(NAME)
	rm -rf $(DESTDIR)$(DOCDIR)
	rm -f $(DESTDIR)$(LIBPATH)/$(LIBRARY)
