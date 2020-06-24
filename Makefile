CC ?= cc
CXX ?= c++
CFLAGS ?= -O2
CXXFLAGS ?= -O2
FLAGS := -fPIC -std=c++17 -fno-strict-aliasing -fwrapv
FLAGS_CO := -fPIC -std=c89
FLAGS_GB := -fPIC -std=c11
CPPFLAGS_GB := -DGB_INTERNAL -DGB_DISABLE_CHEATS -DGB_DISABLE_DEBUGGER \
	-D_GNU_SOURCE
INCLUDES := -I. -I..
INCLUDES_GB := -I../sameboy
WARNINGS := -Wno-multichar -Wno-narrowing -Wno-trigraphs
WARNINGS_CO := -Wall
WARNINGS_GB := -Wno-multichar
LIBS := -lm -lstdc++
SHARED := -fPIC

NAME := bsnes
PREFIX ?= /usr/local
DATADIR ?= $(PREFIX)/share
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

CSRCS := objs/gb/Core/apu.c \
	objs/gb/Core/camera.c \
	objs/gb/Core/display.c \
	objs/gb/Core/gb.c \
	objs/gb/Core/joypad.c \
	objs/gb/Core/mbc.c \
	objs/gb/Core/memory.c \
	objs/gb/Core/printer.c \
	objs/gb/Core/random.c \
	objs/gb/Core/rewind.c \
	objs/gb/Core/rumble.c \
	objs/gb/Core/save_state.c \
	objs/gb/Core/sgb.c \
	objs/gb/Core/sm83_cpu.c \
	objs/gb/Core/symbol_hash.c \
	objs/gb/Core/timing.c \
	objs/libco/libco.c

CXXSRCS := objs/emulator/emulator.cpp \
	objs/filter/filter.cpp \
	objs/lzma/lzma.cpp \
	objs/processor/arm7tdmi/arm7tdmi.cpp \
	objs/processor/spc700/spc700.cpp \
	objs/processor/wdc65816/wdc65816.cpp \
	objs/sfc/cartridge/cartridge.cpp \
	objs/sfc/controller/controller.cpp \
	objs/sfc/coprocessor/coprocessor.cpp \
	objs/sfc/cpu/cpu.cpp \
	objs/sfc/dsp/dsp.cpp \
	objs/sfc/expansion/expansion.cpp \
	objs/sfc/interface/interface.cpp \
	objs/sfc/memory/memory.cpp \
	objs/sfc/ppu/ppu.cpp \
	objs/sfc/ppu-fast/ppu.cpp \
	objs/sfc/slot/slot.cpp \
	objs/sfc/smp/smp.cpp \
	objs/sfc/system/system.cpp \
	objs/jg.cpp

# Object dirs
OBJDIRS := objs objs/emulator objs/filter objs/lzma objs/sfc/interface \
	objs/sfc/system objs/sfc/controller objs/sfc/cartridge objs/sfc/memory \
	objs/sfc/cpu objs/sfc/smp objs/sfc/dsp objs/sfc/ppu objs/sfc/ppu-fast \
	objs/sfc/expansion objs/sfc/coprocessor objs/sfc/slot \
	objs/processor/arm7tdmi objs/processor/gsu objs/processor/hg51b \
	objs/processor/sm83 objs/processor/spc700 objs/processor/upd96050 \
	objs/processor/wdc65816 objs/gb/Core objs/libco

# List of object files
OBJS := $(CSRCS:.c=.o) $(CXXSRCS:.cpp=.o)

# libco rules
objs/libco/%.o: libco/%.c
	$(CC) $(CFLAGS) $(FLAGS_CO) $(WARNINGS_CO) -c $< -o $@

# Game Boy rules
objs/gb/Core/%.o: gb/Core/%.c
	$(CC) $(CFLAGS) $(FLAGS_GB) $(INCLUDES_GB) $(WARNINGS_GB) $(CPPFLAGS_GB) -c $< -o $@

# SNES rules
objs/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(FLAGS) $(INCLUDES) $(WARNINGS) -c $< -o $@

all: maketree $(TARGET)

maketree: $(sort $(OBJDIRS))

$(sort $(OBJDIRS)):
	@mkdir -p $@

$(TARGET): $(OBJS)
	@mkdir -p $(NAME)
	$(CXX) $^ $(LDFLAGS) $(LIBS) $(SHARED) -o $(NAME)/$(TARGET)
	@cp Database/boards.bml $(NAME)/

clean:
	rm -rf $(OBJDIRS) $(NAME)/

install:
	@mkdir -p $(DESTDIR)$(DATADIR)/jollygood/$(NAME)
	@mkdir -p $(DESTDIR)$(LIBDIR)/jollygood
	cp $(NAME)/$(TARGET) $(DESTDIR)$(LIBDIR)/jollygood/
	cp $(NAME)/boards.bml $(DESTDIR)$(DATADIR)/jollygood/$(NAME)/

uninstall:
	rm -rf $(DESTDIR)$(DATADIR)/jollygood/$(NAME)
	rm $(DESTDIR)$(LIBDIR)/jollygood/$(TARGET)
