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

INCLUDES := -I$(SOURCEDIR)/. -I$(SOURCEDIR)/deps $(CFLAGS_JG)
INCLUDES_GB := -I$(SOURCEDIR)/sameboy
WARNINGS := -Wreturn-type -Wno-unused-result
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

CSRCS := $(OBJDIR)/deps/gb/apu.c \
	$(OBJDIR)/deps/gb/camera.c \
	$(OBJDIR)/deps/gb/display.c \
	$(OBJDIR)/deps/gb/gb.c \
	$(OBJDIR)/deps/gb/joypad.c \
	$(OBJDIR)/deps/gb/mbc.c \
	$(OBJDIR)/deps/gb/memory.c \
	$(OBJDIR)/deps/gb/printer.c \
	$(OBJDIR)/deps/gb/random.c \
	$(OBJDIR)/deps/gb/rewind.c \
	$(OBJDIR)/deps/gb/rumble.c \
	$(OBJDIR)/deps/gb/save_state.c \
	$(OBJDIR)/deps/gb/sgb.c \
	$(OBJDIR)/deps/gb/sm83_cpu.c \
	$(OBJDIR)/deps/gb/symbol_hash.c \
	$(OBJDIR)/deps/gb/timing.c \
	$(OBJDIR)/deps/gb/workboy.c \
	$(OBJDIR)/deps/libco/libco.c

CXXSRCS := $(OBJDIR)/emulator/audio.cpp \
	$(OBJDIR)/emulator/emulator.cpp \
	$(OBJDIR)/src/cartridge.cpp \
	$(OBJDIR)/src/controller/controller.cpp \
	$(OBJDIR)/src/controller/gamepad.cpp \
	$(OBJDIR)/src/controller/justifier.cpp \
	$(OBJDIR)/src/controller/mouse.cpp \
	$(OBJDIR)/src/controller/super-multitap.cpp \
	$(OBJDIR)/src/controller/super-scope.cpp \
	$(OBJDIR)/src/coprocessor/armdsp.cpp \
	$(OBJDIR)/src/coprocessor/cx4.cpp \
	$(OBJDIR)/src/coprocessor/dip.cpp \
	$(OBJDIR)/src/coprocessor/dsp1.cpp \
	$(OBJDIR)/src/coprocessor/dsp2.cpp \
	$(OBJDIR)/src/coprocessor/dsp4.cpp \
	$(OBJDIR)/src/coprocessor/epsonrtc.cpp \
	$(OBJDIR)/src/coprocessor/event.cpp \
	$(OBJDIR)/src/coprocessor/hitachidsp.cpp \
	$(OBJDIR)/src/coprocessor/icd.cpp \
	$(OBJDIR)/src/coprocessor/mcc.cpp \
	$(OBJDIR)/src/coprocessor/msu1.cpp \
	$(OBJDIR)/src/coprocessor/necdsp.cpp \
	$(OBJDIR)/src/coprocessor/obc1.cpp \
	$(OBJDIR)/src/coprocessor/sa1.cpp \
	$(OBJDIR)/src/coprocessor/sdd1.cpp \
	$(OBJDIR)/src/coprocessor/sharprtc.cpp \
	$(OBJDIR)/src/coprocessor/spc7110.cpp \
	$(OBJDIR)/src/coprocessor/st0010.cpp \
	$(OBJDIR)/src/coprocessor/superfx.cpp \
	$(OBJDIR)/src/cpu.cpp \
	$(OBJDIR)/src/dsp.cpp \
	$(OBJDIR)/src/dsp/SPC_DSP.cpp \
	$(OBJDIR)/src/expansion/expansion.cpp \
	$(OBJDIR)/src/expansion/satellaview.cpp \
	$(OBJDIR)/src/heuristics.cpp \
	$(OBJDIR)/src/interface.cpp \
	$(OBJDIR)/src/memory.cpp \
	$(OBJDIR)/src/ppu.cpp \
	$(OBJDIR)/src/processor/arm7tdmi.cpp \
	$(OBJDIR)/src/processor/gsu.cpp \
	$(OBJDIR)/src/processor/hg51b.cpp \
	$(OBJDIR)/src/processor/spc700.cpp \
	$(OBJDIR)/src/processor/upd96050.cpp \
	$(OBJDIR)/src/processor/wdc65816.cpp \
	$(OBJDIR)/src/slot/bsmemory.cpp \
	$(OBJDIR)/src/slot/sufamiturbo.cpp \
	$(OBJDIR)/src/smp.cpp \
	$(OBJDIR)/src/system.cpp \
	$(OBJDIR)/jg.cpp

# TODO: Do we need this?
# https://github.com/defparam/21FX
#	$(OBJDIR)/src/expansion/21fx.cpp \

# Object dirs
MKDIRS := $(OBJDIR)/deps/gb \
	$(OBJDIR)/deps/libco \
	$(OBJDIR)/emulator \
	$(OBJDIR)/src/controller \
	$(OBJDIR)/src/dsp \
	$(OBJDIR)/src/expansion \
	$(OBJDIR)/src/coprocessor \
	$(OBJDIR)/src/processor \
	$(OBJDIR)/src/slot

# List of object files
OBJS := $(CSRCS:.c=.o) $(CXXSRCS:.cpp=.o)

.PHONY: all clean install install-strip uninstall

# libco rules
$(OBJDIR)/deps/libco/%.o: $(SOURCEDIR)/deps/libco/%.c $(OBJDIR)/.tag
	$(info $(CC) $(CFLAGS) $(FLAGS_CO) $(WARNINGS_CO) -c \
		$(SUBST $(SOURCEDIR)/,,$<) -o $@)
	@$(CC) $(CFLAGS) $(FLAGS_CO) $(WARNINGS_CO) -c $< -o $@

# Game Boy rules
$(OBJDIR)/deps/gb/%.o: $(SOURCEDIR)/deps/gb/%.c $(OBJDIR)/.tag
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
	cp $(SOURCEDIR)/LICENSE $(DESTDIR)$(DOCDIR)
	cp $(SOURCEDIR)/README $(DESTDIR)$(DOCDIR)
	cp $(SOURCEDIR)/deps/gb/LICENSE $(DESTDIR)$(DOCDIR)/LICENSE-gb
	cp $(SOURCEDIR)/deps/libco/LICENSE $(DESTDIR)$(DOCDIR)/LICENSE-libco

install-strip: install
	strip $(DESTDIR)$(LIBDIR)/jollygood/$(TARGET)

uninstall:
	rm -rf $(DESTDIR)$(DATADIR)/jollygood/$(NAME)
	rm -rf $(DESTDIR)$(DOCDIR)
	rm -f $(DESTDIR)$(LIBDIR)/jollygood/$(TARGET)
