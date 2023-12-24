AR ?= ar
CC ?= cc
CXX ?= c++
CC_FOR_BUILD ?= $(CC)
CXX_FOR_BUILD ?= $(CXX)
PKG_CONFIG ?= pkg-config

PREFIX ?= /usr/local
EXEC_PREFIX ?= $(PREFIX)
LIBDIR ?= $(EXEC_PREFIX)/lib
INCLUDEDIR ?= $(PREFIX)/include
DATAROOTDIR ?= $(PREFIX)/share
DATADIR ?= $(DATAROOTDIR)
DOCDIR ?= $(DATAROOTDIR)/doc/$(NAME)
MANDIR ?= $(DATAROOTDIR)/man

CFLAGS_JG = $(shell $(PKG_CONFIG) --cflags jg)

override OBJDIR := objs
override UNAME := $(shell uname -s)

# Info command
override COMPILE_INFO = $(info $(subst $(SOURCEDIR)/,,$(1)))

override .DEFAULT_GOAL := all
override PHONY := all clean install install-strip uninstall

$(OBJDIR)/.tag:
	@mkdir -p -- $(if $(MKDIRS),$(MKDIRS:%=$(OBJDIR)/%),$(OBJDIR))
	@touch $@

install-docs:: all
	@mkdir -p $(DESTDIR)$(DOCDIR)
ifneq ($(DOCS),)
	for i in $(DOCS); do \
		cp $(SOURCEDIR)/$$i $(DESTDIR)$(DOCDIR); \
	done
endif
