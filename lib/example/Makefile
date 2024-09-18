.POSIX:

PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
DATAROOTDIR ?= $(PREFIX)/share
DATADIR ?= $(DATAROOTDIR)

CXX ?= c++
CXXFLAGS ?= -O2

PKG_CONFIG ?= pkg-config

NAME := bsnes-example

BML := boards.bml \
	BSMemory.bml \
	CheatCodes.bml \
	SufamiTurbo.bml \
	SuperFamicom.bml

BML_SOURCES != find ../../Database -type f -name '*.bml'

CFLAGS_LIBBSNES != $(PKG_CONFIG) --cflags libbsnes
LIBS_LIBBSNES != $(PKG_CONFIG) --libs libbsnes

CFLAGS_SDL2 != $(PKG_CONFIG) --cflags sdl2
LIBS_SDL2 != $(PKG_CONFIG) --libs sdl2

CFLAGS_SAMPLERATE != $(PKG_CONFIG) --cflags samplerate
LIBS_SAMPLERATE != $(PKG_CONFIG) --libs samplerate

DEFINES += -DDATADIR="\"$(DATADIR)/$(NAME)\""
INCLUDES := $(CFLAGS_LIBBSNES) $(CFLAGS_SDL2) $(CFLAGS_SAMPLERATE)
LIBS := $(LIBS_LIBBSNES) $(LIBS_SDL2) $(LIBS_SAMPLERATE)

all: $(NAME) $(BML)

$(BML): $(BML_SOURCES)
	cp ../../Database/$@ .

$(NAME): example.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(DEFINES) $(INCLUDES) -o $@ $? $(LDFLAGS) $(LIBS)

install: all
	mkdir -p $(DESTDIR)$(BINDIR)
	mkdir -p $(DESTDIR)$(DATADIR)/$(NAME)
	cp $(NAME) $(DESTDIR)$(BINDIR)/
	cp *.bml $(DESTDIR)$(DATADIR)/$(NAME)/

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(NAME)
	rm -rf $(DESTDIR)$(DATADIR)/$(NAME)

clean:
	rm -f $(NAME)
	rm -f *.bml
