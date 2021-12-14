SOURCEDIR := $(abspath $(patsubst %/,%,$(dir $(abspath $(lastword \
	$(MAKEFILE_LIST))))))

CC ?= cc
CXX ?= c++
CFLAGS ?= -O2
CXXFLAGS ?= -O2
FLAGS := -fPIC -std=c++17 -fno-strict-aliasing -fwrapv
FLAGS_CO := -fPIC -std=c89
FLAGS_GB := -fPIC -std=c11
CPPFLAGS_GB := -DGB_INTERNAL -DGB_DISABLE_CHEATS -DGB_DISABLE_DEBUGGER \
	-D_GNU_SOURCE -DGB_VERSION=\"0.14.7\"

PKGCONF ?= pkg-config
CFLAGS_JG := $(shell $(PKGCONF) --cflags jg)

INCLUDES := -I$(SOURCEDIR)/. $(CFLAGS_JG)
INCLUDES_GB := -I$(SOURCEDIR)/sameboy
WARNINGS :=
WARNINGS_CO := -Wall
WARNINGS_GB := -Wno-multichar
LIBS := -lm -lstdc++
SHARED := -fPIC

NAME := bsnes
PREFIX ?= /usr/local
DATAROOTDIR ?= $(PREFIX)/share
DATADIR ?= $(DATAROOTDIR)
DOCDIR ?= $(DATAROOTDIR)/doc/$(NAME)
LIBDIR ?= $(PREFIX)/lib

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

OBJDIR := objs

CSRCS := $(OBJDIR)/gb/Core/apu.c \
	$(OBJDIR)/gb/Core/camera.c \
	$(OBJDIR)/gb/Core/display.c \
	$(OBJDIR)/gb/Core/gb.c \
	$(OBJDIR)/gb/Core/joypad.c \
	$(OBJDIR)/gb/Core/mbc.c \
	$(OBJDIR)/gb/Core/memory.c \
	$(OBJDIR)/gb/Core/printer.c \
	$(OBJDIR)/gb/Core/random.c \
	$(OBJDIR)/gb/Core/rewind.c \
	$(OBJDIR)/gb/Core/rumble.c \
	$(OBJDIR)/gb/Core/save_state.c \
	$(OBJDIR)/gb/Core/sgb.c \
	$(OBJDIR)/gb/Core/sm83_cpu.c \
	$(OBJDIR)/gb/Core/symbol_hash.c \
	$(OBJDIR)/gb/Core/timing.c \
	$(OBJDIR)/gb/Core/workboy.c \
	$(OBJDIR)/libco/libco.c

CXXSRCS := $(OBJDIR)/emulator/audio/audio.cpp \
	$(OBJDIR)/emulator/emulator.cpp \
	$(OBJDIR)/heuristics/heuristics.cpp \
	$(OBJDIR)/processor/arm7tdmi/arm7tdmi.cpp \
	$(OBJDIR)/processor/spc700/spc700.cpp \
	$(OBJDIR)/processor/wdc65816/wdc65816.cpp \
	$(OBJDIR)/sfc/cartridge/cartridge.cpp \
	$(OBJDIR)/sfc/controller/controller.cpp \
	$(OBJDIR)/sfc/coprocessor/coprocessor.cpp \
	$(OBJDIR)/sfc/cpu/cpu.cpp \
	$(OBJDIR)/sfc/dsp/dsp.cpp \
	$(OBJDIR)/sfc/expansion/expansion.cpp \
	$(OBJDIR)/sfc/interface/interface.cpp \
	$(OBJDIR)/sfc/memory/memory.cpp \
	$(OBJDIR)/sfc/ppu/ppu.cpp \
	$(OBJDIR)/sfc/ppu-fast/ppu.cpp \
	$(OBJDIR)/sfc/slot/slot.cpp \
	$(OBJDIR)/sfc/smp/smp.cpp \
	$(OBJDIR)/sfc/system/system.cpp \
	$(OBJDIR)/jg.cpp

# Object dirs
MKDIRS := $(OBJDIR)/emulator/audio \
	$(OBJDIR)/heuristics \
	$(OBJDIR)/sfc/interface \
	$(OBJDIR)/sfc/system \
	$(OBJDIR)/sfc/controller \
	$(OBJDIR)/sfc/cartridge \
	$(OBJDIR)/sfc/memory \
	$(OBJDIR)/sfc/cpu \
	$(OBJDIR)/sfc/smp \
	$(OBJDIR)/sfc/dsp \
	$(OBJDIR)/sfc/ppu \
	$(OBJDIR)/sfc/ppu-fast \
	$(OBJDIR)/sfc/expansion \
	$(OBJDIR)/sfc/coprocessor \
	$(OBJDIR)/sfc/slot \
	$(OBJDIR)/processor/arm7tdmi \
	$(OBJDIR)/processor/spc700 \
	$(OBJDIR)/processor/wdc65816 \
	$(OBJDIR)/gb/Core \
	$(OBJDIR)/libco

# List of object files
OBJS := $(CSRCS:.c=.o) $(CXXSRCS:.cpp=.o)

.PHONY: all clean install install-strip uninstall

# libco rules
$(OBJDIR)/libco/%.o: $(SOURCEDIR)/libco/%.c $(OBJDIR)/.tag
	$(info $(CC) $(CFLAGS) $(FLAGS_CO) $(WARNINGS_CO) -c \
		$(SUBST $(SOURCEDIR)/,,$<) -o $@)
	@$(CC) $(CFLAGS) $(FLAGS_CO) $(WARNINGS_CO) -c $< -o $@

# Game Boy rules
$(OBJDIR)/gb/Core/%.o: $(SOURCEDIR)/gb/Core/%.c $(OBJDIR)/.tag
	$(info $(CC) $(CFLAGS) $(FLAGS_GB) $(WARNINGS_GB) $(CPPFLAGS_GB) \
		$(subst $(SOURCEDIR)/,,$(INCLUDES_GB) -c $<) -o $@)
	@$(CC) $(CFLAGS) $(FLAGS_GB) $(WARNINGS_GB) $(CPPFLAGS_GB) \
		$(INCLUDES_GB) -c $< -o $@

# SNES rules
$(OBJDIR)/%.o: $(SOURCEDIR)/%.cpp $(OBJDIR)/.tag
	$(info $(CXX) $(CXXFLAGS) $(FLAGS) $(WARNINGS) \
		$(subst $(SOURCEDIR)/,,$(INCLUDES) -c $<) -o $@)
	@$(CXX) $(CXXFLAGS) $(FLAGS) $(WARNINGS) $(INCLUDES) -c $< -o $@

all: $(TARGET)

$(OBJDIR)/.tag:
	@mkdir -p -- $(sort $(MKDIRS))
	@touch $@

$(TARGET): $(OBJS)
	@mkdir -p $(NAME)
	$(CXX) $^ $(LDFLAGS) $(LIBS) $(SHARED) -o $(NAME)/$(TARGET)
	@cp $(SOURCEDIR)/Database/boards.bml $(NAME)/
	@cp $(SOURCEDIR)/Database/BS\ Memory.bml $(NAME)/
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
	cp $(NAME)/Super\ Famicom.bml $(DESTDIR)$(DATADIR)/jollygood/$(NAME)/
	cp $(NAME)/Sufami\ Turbo.bml $(DESTDIR)$(DATADIR)/jollygood/$(NAME)/
	cp $(SOURCEDIR)/COPYING $(DESTDIR)$(DOCDIR)
	cp $(SOURCEDIR)/LICENSE $(DESTDIR)$(DOCDIR)
	cp $(SOURCEDIR)/README $(DESTDIR)$(DOCDIR)
	cp $(SOURCEDIR)/libco/LICENSE $(DESTDIR)$(DOCDIR)/LICENSE-libco
	cp $(SOURCEDIR)/gb/LICENSE $(DESTDIR)$(DOCDIR)/LICENSE-gb

install-strip: install
	strip $(DESTDIR)$(LIBDIR)/jollygood/$(TARGET)

uninstall:
	rm -rf $(DESTDIR)$(DATADIR)/jollygood/$(NAME)
	rm -rf $(DESTDIR)$(DOCDIR)
	rm -f $(DESTDIR)$(LIBDIR)/jollygood/$(TARGET)
