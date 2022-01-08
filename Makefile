SOURCEDIR := $(abspath $(patsubst %/,%,$(dir $(abspath $(lastword \
	$(MAKEFILE_LIST))))))

CC ?= cc
CXX ?= c++
CFLAGS ?= -O2
CXXFLAGS ?= -O2
FLAGS := -std=c++17 -fwrapv
FLAGS_BML := -std=c++14
FLAGS_CO := -std=c89
FLAGS_GB := -std=c11
FLAGS_SAMPLERATE := -std=c99
CPPFLAGS_GB := -DGB_INTERNAL -DGB_DISABLE_CHEATS -DGB_DISABLE_DEBUGGER \
	-D_GNU_SOURCE -DGB_VERSION=\"0.14.7\"

PKGCONF ?= pkg-config
CFLAGS_JG := $(shell $(PKGCONF) --cflags jg)

INCLUDES := -I$(SOURCEDIR)/deps
WARNINGS := -Wall
WARNINGS_BML := -Wall -Wextra -Wshadow -pedantic
WARNINGS_CO := -Wall -Wextra -Wshadow -Wmissing-prototypes
WARNINGS_GB := -Wno-multichar
WARNINGS_SAMPLERATE := -Wall -Wextra -Wshadow -Wmissing-prototypes
LIBS := -lm -lstdc++
PIC := -fPIC
SHARED := $(PIC)

NAME := bsnes
PREFIX ?= /usr/local
DATAROOTDIR ?= $(PREFIX)/share
DATADIR ?= $(DATAROOTDIR)
DOCDIR ?= $(DATAROOTDIR)/doc/$(NAME)
LIBDIR ?= $(PREFIX)/lib

USE_VENDORED_SAMPLERATE ?= 0

UNAME := $(shell uname -s)
ifeq ($(UNAME), Darwin)
	SHARED += -dynamiclib
	TARGET := $(NAME).dylib
else ifeq ($(OS), Windows_NT)
	SHARED += -shared
	TARGET := $(NAME).dll
else
	SHARED += -shared
	TARGET := $(NAME).so
endif

ifeq ($(UNAME), Linux)
	LIBS += -Wl,--no-undefined
endif

ifeq ($(shell $(CXX) -v 2>&1 | grep -c "clang"),1)
	WARNINGS += -Wno-shift-count-overflow -Wno-overloaded-virtual
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
	deps/gb/symbol_hash.c \
	deps/gb/timing.c \
	deps/gb/workboy.c \
	deps/libco/libco.c

CXXSRCS := deps/byuuML/byuuML.cpp \
	src/audio.cpp \
	src/bsmemory.cpp \
	src/cartridge.cpp \
	src/cheat.cpp \
	src/controller/controller.cpp \
	src/controller/gamepad.cpp \
	src/controller/justifier.cpp \
	src/controller/mouse.cpp \
	src/controller/super-multitap.cpp \
	src/controller/super-scope.cpp \
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
	src/dsp/SPC_DSP.cpp \
	src/emulator.cpp \
	src/expansion/expansion.cpp \
	src/expansion/satellaview.cpp \
	src/heuristics.cpp \
	src/interface.cpp \
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
	src/sha256.cpp \
	src/sufamiturbo.cpp \
	src/smp.cpp \
	src/system.cpp \
	jg.cpp

# TODO: Do we need this?
# https://github.com/defparam/21FX
#	src/expansion/21fx.cpp \

ifneq ($(USE_VENDORED_SAMPLERATE), 0)
	Q_SAMPLERATE :=
	CFLAGS_SAMPLERATE := -I$(SOURCEDIR)/deps/libsamplerate
	LIBS_SAMPLERATE :=
	CSRCS += deps/libsamplerate/samplerate.c \
		deps/libsamplerate/src_linear.c \
		deps/libsamplerate/src_sinc.c \
		deps/libsamplerate/src_zoh.c
else
	Q_SAMPLERATE := @
	CFLAGS_SAMPLERATE := $(shell $(PKGCONF) --cflags samplerate)
	LIBS_SAMPLERATE := $(shell $(PKGCONF) --libs samplerate)
endif

INCLUDES += $(CFLAGS_SAMPLERATE)
LIBS += $(LIBS_SAMPLERATE)

# Object dirs
MKDIRS := deps/byuuML \
	deps/gb \
	deps/libco \
	deps/libsamplerate \
	src/controller \
	src/coprocessor \
	src/dsp \
	src/expansion \
	src/processor

OBJDIR := objs

# List of object files
OBJS := $(patsubst %,$(OBJDIR)/%,$(CSRCS:.c=.o) $(CXXSRCS:.cpp=.o))

# Compiler commands
COMPILE = $(strip $(1) $(CPPFLAGS) $(PIC) $(2) -c $< -o $@)
COMPILE_C = $(call COMPILE, $(CC) $(CFLAGS), $(1))
COMPILE_CXX = $(call COMPILE, $(CXX) $(CXXFLAGS), $(1))

# Info command
COMPILE_INFO = $(info $(subst $(SOURCEDIR)/,,$(1)))

# Dependency commands
BUILD_BML = $(call COMPILE_CXX, $(FLAGS_BML) $(WARNINGS_BML))
BUILD_CO = $(call COMPILE_C, $(FLAGS_CO) $(WARNINGS_CO))
BUILD_SAMPLERATE = $(call COMPILE_C, $(FLAGS_SAMPLERATE) $(WARNINGS_SAMPLERATE))

# Core commands
BUILD_GB = $(call COMPILE_C, $(FLAGS_GB) $(WARNINGS_GB) $(CPPFLAGS_GB))
BUILD_JG = $(call COMPILE_CXX, $(FLAGS) $(WARNINGS) $(INCLUDES) $(CFLAGS_JG))
BUILD_MAIN = $(call COMPILE_CXX, $(FLAGS) $(WARNINGS) $(INCLUDES))

.PHONY: all clean install install-strip uninstall

all: $(NAME)/$(TARGET)

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
	$(call COMPILE_INFO, $(BUILD_SAMPLERATE))
	@$(BUILD_SAMPLERATE)

# Game Boy rules
$(OBJDIR)/deps/gb/%.o: $(SOURCEDIR)/deps/gb/%.c $(OBJDIR)/.tag
	$(call COMPILE_INFO, $(BUILD_GB))
	@$(BUILD_GB)

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

$(NAME)/$(TARGET): $(OBJS)
	@mkdir -p $(NAME)
	$(CXX) $^ $(LDFLAGS) $(LIBS) $(SHARED) -o $@
	@cp $(SOURCEDIR)/Database/boards.bml $(NAME)/
	@cp $(SOURCEDIR)/Database/BS\ Memory.bml $(NAME)/
	@cp $(SOURCEDIR)/Database/Sufami\ Turbo.bml $(NAME)/
	@cp $(SOURCEDIR)/Database/Super\ Famicom.bml $(NAME)/

clean:
	rm -rf $(OBJDIR)/ $(NAME)/

install: all
	@mkdir -p $(DESTDIR)$(DATADIR)/jollygood/$(NAME)
	@mkdir -p $(DESTDIR)$(DOCDIR)
	@mkdir -p $(DESTDIR)$(LIBDIR)/jollygood
	cp $(NAME)/$(TARGET) $(DESTDIR)$(LIBDIR)/jollygood/
	cp $(NAME)/boards.bml $(DESTDIR)$(DATADIR)/jollygood/$(NAME)/
	cp $(NAME)/BS\ Memory.bml $(DESTDIR)$(DATADIR)/jollygood/$(NAME)/
	cp $(NAME)/Sufami\ Turbo.bml $(DESTDIR)$(DATADIR)/jollygood/$(NAME)/
	cp $(NAME)/Super\ Famicom.bml $(DESTDIR)$(DATADIR)/jollygood/$(NAME)/
	cp $(SOURCEDIR)/COPYING $(DESTDIR)$(DOCDIR)
	cp $(SOURCEDIR)/README $(DESTDIR)$(DOCDIR)
	cp $(SOURCEDIR)/deps/byuuML/LICENSE $(DESTDIR)$(DOCDIR)/LICENSE-byuuML
	cp $(SOURCEDIR)/deps/gb/LICENSE $(DESTDIR)$(DOCDIR)/LICENSE-gb
	cp $(SOURCEDIR)/deps/libco/LICENSE $(DESTDIR)$(DOCDIR)/LICENSE-libco
	$(Q_SAMPLERATE)if test $(USE_VENDORED_SAMPLERATE) != 0; then \
		cp $(SOURCEDIR)/deps/libsamplerate/COPYING \
			$(DESTDIR)$(DOCDIR)/COPYING-libsamplerate; \
	fi

install-strip: install
	strip $(DESTDIR)$(LIBDIR)/jollygood/$(TARGET)

uninstall:
	rm -rf $(DESTDIR)$(DATADIR)/jollygood/$(NAME)
	rm -rf $(DESTDIR)$(DOCDIR)
	rm -f $(DESTDIR)$(LIBDIR)/jollygood/$(TARGET)
