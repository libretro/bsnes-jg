AR ?= ar
CC ?= cc
CXX ?= c++
CC_FOR_BUILD ?= $(CC)
CXX_FOR_BUILD ?= $(CXX)
DOXYGEN ?= doxygen
GLSLANG ?= glslangValidator
PKG_CONFIG ?= pkg-config
STRIP ?= strip

PREFIX ?= /usr/local
EXEC_PREFIX ?= $(PREFIX)
BINDIR ?= $(EXEC_PREFIX)/bin
LIBDIR ?= $(EXEC_PREFIX)/lib
INCLUDEDIR ?= $(PREFIX)/include
DATAROOTDIR ?= $(PREFIX)/share
DATADIR ?= $(DATAROOTDIR)
DOCDIR ?= $(DATAROOTDIR)/doc/$(NAME)
HTMLDIR ?= $(DOCDIR)
MANDIR ?= $(DATAROOTDIR)/man

CFLAGS ?= -O2
CXXFLAGS ?= -O2

HOST = $(shell $(LINKER) -dumpmachine)

WARNINGS_DEF := -Wall -Wextra -Wshadow -Wmissing-declarations
WARNINGS_DEF_C := $(WARNINGS_DEF) -Wmissing-prototypes -pedantic
WARNINGS_DEF_CXX := $(WARNINGS_DEF) -pedantic

override LIBS_PRIVATE := Libs.private:
override REQUIRES_PRIVATE := Requires.private:

override DEPDIR := $(SOURCEDIR)/deps
override OBJDIR := objs
override PREREQ := $(OBJDIR)/.tag
override HOST := $(HOST)

ifneq (,$(findstring darwin, $(HOST)))
	override PLATFORM := Darwin
else ifneq (,$(findstring dragonfly, $(HOST)))
	override PLATFORM := DragonFlyBSD
else ifneq (,$(findstring freebsd, $(HOST)))
	override PLATFORM := FreeBSD
else ifneq (,$(findstring haiku, $(HOST)))
	override PLATFORM := Haiku
else ifneq (,$(findstring linux, $(HOST)))
	override PLATFORM := Linux
else ifneq (,$(findstring midipix, $(HOST)))
	override PLATFORM := Midipix
else ifneq (,$(findstring netbsd, $(HOST)))
	override PLATFORM := NetBSD
else ifneq (,$(findstring openbsd, $(HOST)))
	override PLATFORM := OpenBSD
else ifneq (,$(or $(findstring cygwin, $(HOST)), \
		$(findstring mingw, $(HOST)), \
		$(findstring msvc, $(HOST)), \
		$(findstring msys, $(HOST)), \
		$(findstring windows, $(HOST))))
	override PLATFORM := Windows
else
	override PLATFORM :=
endif

ifeq ($(PLATFORM), Windows)
	override BIN_EXT := .exe
else
	override BIN_EXT :=
endif

# Info command
override COMPILE_INFO = $(info $(subst $(SOURCEDIR)/,,$(1)))

override .DEFAULT_GOAL := all
override PHONY := all clean install install-docs install-strip uninstall

# Object dirs
override ALL_DIRS = $(patsubst %/,%,$(dir $(OBJS)))
override MKDIRS = $(sort $(foreach d, \
	$(ALL_DIRS),$(if $(filter $(d)/%,$(ALL_DIRS)),,$d)))

$(OBJDIR)/.tag:
	@mkdir -p -- $(MKDIRS)
	@touch $@

install-docs:: all
	@mkdir -p $(DESTDIR)$(DOCDIR)
ifneq ($(DOCS),)
	for i in $(DOCS); do \
		cp $(SOURCEDIR)/$$i \
			$(DESTDIR)$(DOCDIR); \
	done
endif

include $(SOURCEDIR)/mk/deps.mk
