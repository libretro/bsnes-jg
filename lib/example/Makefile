.POSIX:

PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

CXX = c++
CXXFLAGS = -O2

PKG_CONFIG = pkg-config

NAME = libbsnes-example

CFLAGS_LIBBSNES = `$(PKG_CONFIG) --cflags libbsnes`
LIBS_LIBBSNES = `$(PKG_CONFIG) --libs libbsnes`

CFLAGS_SDL2 = `$(PKG_CONFIG) --cflags sdl2`
LIBS_SDL2 = `$(PKG_CONFIG) --libs sdl2`

CFLAGS_SAMPLERATE = `$(PKG_CONFIG) --cflags samplerate`
LIBS_SAMPLERATE = `$(PKG_CONFIG) --libs samplerate`

INCLUDES = $(CFLAGS_LIBBSNES) $(CFLAGS_SDL2) $(CFLAGS_SAMPLERATE)
LIBS = $(LIBS_LIBBSNES) $(LIBS_SDL2) $(LIBS_SAMPLERATE)

all: $(NAME)

$(NAME): example.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDES) -o $@ $? $(LDFLAGS) $(LIBS)

install: all
	mkdir -p $(DESTDIR)$(BINDIR)
	cp $(NAME) $(DESTDIR)$(BINDIR)/

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(NAME)

clean:
	rm -f $(NAME)
