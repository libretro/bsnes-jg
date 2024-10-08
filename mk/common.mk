AR ?= ar
CC ?= cc
CXX ?= c++
CC_FOR_BUILD ?= $(CC)
CXX_FOR_BUILD ?= $(CXX)
DOXYGEN ?= doxygen
PKG_CONFIG ?= pkg-config

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

WARNINGS_DEF := -Wall -Wextra -Wshadow -Wmissing-declarations
WARNINGS_DEF_C := $(WARNINGS_DEF) -Wmissing-prototypes -pedantic
WARNINGS_DEF_CXX := $(WARNINGS_DEF) -pedantic

override LIBS_PRIVATE := Libs.private:
override REQUIRES_PRIVATE := Requires.private:

override DEPDIR := $(SOURCEDIR)/deps
override OBJDIR := objs
override UNAME := $(shell uname -s)

# Info command
override COMPILE_INFO = $(info $(subst $(SOURCEDIR)/,,$(1)))

override .DEFAULT_GOAL := all
override PHONY := all clean install install-docs install-strip uninstall

$(OBJDIR)/.tag:
	@mkdir -p -- $(if $(MKDIRS),$(MKDIRS:%=$(OBJDIR)/%),$(OBJDIR))
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
