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
	$(OBJDIR)/sfc/coprocessor/armdsp/armdsp.cpp \
	$(OBJDIR)/sfc/coprocessor/cx4/cx4.cpp \
	$(OBJDIR)/sfc/coprocessor/dip/dip.cpp \
	$(OBJDIR)/sfc/coprocessor/dsp1/dsp1.cpp \
	$(OBJDIR)/sfc/coprocessor/dsp2/dsp2.cpp \
	$(OBJDIR)/sfc/coprocessor/dsp4/dsp4.cpp \
	$(OBJDIR)/sfc/coprocessor/epsonrtc/epsonrtc.cpp \
	$(OBJDIR)/sfc/coprocessor/event/event.cpp \
	$(OBJDIR)/sfc/coprocessor/hitachidsp/hitachidsp.cpp \
	$(OBJDIR)/sfc/coprocessor/icd/icd.cpp \
	$(OBJDIR)/sfc/coprocessor/mcc/mcc.cpp \
	$(OBJDIR)/sfc/coprocessor/msu1/msu1.cpp \
	$(OBJDIR)/sfc/coprocessor/necdsp/necdsp.cpp \
	$(OBJDIR)/sfc/coprocessor/obc1/obc1.cpp \
	$(OBJDIR)/sfc/coprocessor/sa1/sa1.cpp \
	$(OBJDIR)/sfc/coprocessor/sdd1/sdd1.cpp \
	$(OBJDIR)/sfc/coprocessor/sharprtc/sharprtc.cpp \
	$(OBJDIR)/sfc/coprocessor/spc7110/spc7110.cpp \
	$(OBJDIR)/sfc/coprocessor/st0010/st0010.cpp \
	$(OBJDIR)/sfc/coprocessor/superfx/superfx.cpp \
	$(OBJDIR)/sfc/cpu/cpu.cpp \
	$(OBJDIR)/sfc/dsp/dsp.cpp \
	$(OBJDIR)/sfc/expansion/expansion.cpp \
	$(OBJDIR)/sfc/expansion/satellaview/satellaview.cpp \
	$(OBJDIR)/sfc/interface/interface.cpp \
	$(OBJDIR)/sfc/memory/memory.cpp \
	$(OBJDIR)/sfc/ppu/ppu.cpp \
	$(OBJDIR)/sfc/ppu-fast/ppu.cpp \
	$(OBJDIR)/sfc/slot/bsmemory/bsmemory.cpp \
	$(OBJDIR)/sfc/slot/sufamiturbo/sufamiturbo.cpp \
	$(OBJDIR)/sfc/smp/smp.cpp \
	$(OBJDIR)/sfc/system/system.cpp \
	$(OBJDIR)/jg.cpp

# TODO: Do we need this?
# https://github.com/defparam/21FX
#	$(OBJDIR)/sfc/expansion/21fx/21fx.cpp \

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
	$(OBJDIR)/sfc/expansion/satellaview \
	$(OBJDIR)/sfc/coprocessor/armdsp \
	$(OBJDIR)/sfc/coprocessor/cx4 \
	$(OBJDIR)/sfc/coprocessor/dip \
	$(OBJDIR)/sfc/coprocessor/dsp1 \
	$(OBJDIR)/sfc/coprocessor/dsp2 \
	$(OBJDIR)/sfc/coprocessor/dsp4 \
	$(OBJDIR)/sfc/coprocessor/epsonrtc \
	$(OBJDIR)/sfc/coprocessor/event \
	$(OBJDIR)/sfc/coprocessor/hitachidsp \
	$(OBJDIR)/sfc/coprocessor/icd \
	$(OBJDIR)/sfc/coprocessor/mcc \
	$(OBJDIR)/sfc/coprocessor/msu1 \
	$(OBJDIR)/sfc/coprocessor/necdsp \
	$(OBJDIR)/sfc/coprocessor/obc1 \
	$(OBJDIR)/sfc/coprocessor/sa1 \
	$(OBJDIR)/sfc/coprocessor/sdd1 \
	$(OBJDIR)/sfc/coprocessor/sharprtc \
	$(OBJDIR)/sfc/coprocessor/spc7110 \
	$(OBJDIR)/sfc/coprocessor/st0010 \
	$(OBJDIR)/sfc/coprocessor/superfx \
	$(OBJDIR)/sfc/slot/bsmemory \
	$(OBJDIR)/sfc/slot/sufamiturbo \
	$(OBJDIR)/processor/arm7tdmi \
	$(OBJDIR)/processor/spc700 \
	$(OBJDIR)/processor/wdc65816 \
	$(OBJDIR)/gb/Core \
	$(OBJDIR)/libco

#	$(OBJDIR)/sfc/expansion/21fx \

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
