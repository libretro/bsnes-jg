DISABLE_MODULE ?= 0
ENABLE_SHARED ?= 0
ENABLE_STATIC ?= 0
ENABLE_STATIC_JG ?= 0

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

CFLAGS_JG = $(shell $(PKG_CONFIG) --cflags jg)

override VERSION := $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)

override PIC := -fPIC
override SHARED := $(PIC)

override INCPATH := $(INCLUDEDIR)/$(JGNAME)
override LIBPATH := $(LIBDIR)/jollygood
override OBJDIR := objs

override LIBS_PRIVATE := Libs.private:
override REQUIRES_PRIVATE := Requires.private:

override PKGCONF_SH := $(wildcard $(SOURCEDIR)/lib/pkgconf.sh)

ifneq ($(PKGCONF_SH),)
	override PKGCONFLIBDIR := \
		$(shell $(PKGCONF_SH) "$(EXEC_PREFIX)" "$(LIBDIR)" exec_)
	override PKGCONFINCDIR := \
		$(shell $(PKGCONF_SH) "$(PREFIX)" "$(INCLUDEDIR)")
endif

ifeq ($(PREFIX), $(EXEC_PREFIX))
	override PKGCONFEXECDIR := $${prefix}
else
	override PKGCONFEXECDIR := $(EXEC_PREFIX)
endif

UNAME := $(shell uname -s)
ifeq ($(UNAME), Darwin)
	override LIBRARY := $(NAME).dylib
else ifeq ($(OS), Windows_NT)
	override LIBRARY := $(NAME).dll
else
	override LIBRARY := $(NAME).so
endif

override LIB_PC := lib$(NAME).pc
override LIB_SHARED := lib$(LIBRARY)
override LIB_STATIC := lib$(NAME).a

ifeq ($(UNAME), Darwin)
	override LIB_MAJOR := lib$(NAME).$(VERSION_MAJOR).dylib
	override LIB_VERSION := lib$(NAME).$(VERSION).dylib
	override SHARED += -dynamiclib -Wl,-undefined,error
	override SONAME := -Wl,-install_name,$(LIB_MAJOR)
else
	override LIB_MAJOR := $(LIB_SHARED).$(VERSION_MAJOR)
	override LIB_VERSION := $(LIB_SHARED).$(VERSION)
	override SHARED += -shared -Wl,--no-undefined
	override SONAME := -Wl,-soname,$(LIB_MAJOR)
endif

# Desktop File
override DESKTOP := $(JGNAME).desktop

override DESKTOP_TARGET := $(NAME)/$(DESKTOP)

# Icons
override ICONS := $(wildcard $(SOURCEDIR)/icons/*.png \
	$(SOURCEDIR)/icons/$(NAME).svg)

override ICONS_BASE := $(notdir $(ICONS))
override ICONS_TARGET := $(ICONS_BASE:%=$(NAME)/icons/%)

# Library targets
override TARGET :=
override TARGET_INSTALL := install-data install-docs install-library
override TARGET_MODULE := $(NAME)/$(LIBRARY)
override TARGET_SHARED := $(OBJDIR)/$(LIB_VERSION)
override TARGET_STATIC := $(OBJDIR)/$(LIB_STATIC)
override TARGET_STATIC_JG := $(NAME)/lib$(NAME)-jg.a
override TARGET_STATIC_MK := $(NAME)/jg-static.mk

ifeq ($(DISABLE_MODULE), 0)
	override TARGET += $(TARGET_MODULE)
endif

ifneq ($(ENABLE_STATIC), 0)
	override TARGET += $(TARGET_STATIC)
	override OBJS_SHARED := $(TARGET_STATIC)
else
	override OBJS_SHARED = $(OBJS)
endif

ifneq ($(ENABLE_SHARED), 0)
	override TARGET += $(OBJDIR)/$(LIB_MAJOR) $(OBJDIR)/$(LIB_SHARED)
	override LIBS_MODULE := -L$(OBJDIR) -l$(NAME)
else
	override LIBS_MODULE = $(OBJS_SHARED)
endif

ifneq ($(ENABLE_STATIC_JG), 0)
	override TARGET += $(DESKTOP_TARGET) $(ICONS_TARGET) $(TARGET_STATIC_MK)
endif

ifneq ($(ENABLE_SHARED), 0)
	override OBJS_MODULE := $(OBJDIR)/$(LIB_MAJOR) $(OBJDIR)/$(LIB_SHARED)
else ifneq ($(ENABLE_STATIC), 0)
	override OBJS_MODULE := $(OBJS_SHARED)
else
	override OBJS_MODULE = $(OBJS)
endif

ifneq (,$(filter-out 0,$(ENABLE_SHARED) $(ENABLE_STATIC)))
	override ENABLE_INSTALL := 1
	override ENABLE_LIBRARY := 1
else ifeq ($(DISABLE_MODULE), 0)
	override ENABLE_INSTALL := 1
	override ENABLE_LIBRARY := 0
else
	override ENABLE_INSTALL := 0
	override ENABLE_LIBRARY := 0
endif

# Compiler commands
override COMPILE = $(strip $(1) $(CPPFLAGS) $(PIC) $(2) -c $< -o $@)
override COMPILE_C = $(call COMPILE, $(CC) $(CFLAGS), $(1))
override COMPILE_CXX = $(call COMPILE, $(CXX) $(CXXFLAGS), $(1))
override COMPILE_C_BUILD = $(strip $(CC_FOR_BUILD) $(1) $< -o $@)
override COMPILE_CXX_BUILD = $(strip $(CXX_FOR_BUILD) $(1) $< -o $@)

# Info command
override COMPILE_INFO = $(info $(subst $(SOURCEDIR)/,,$(1)))

override .DEFAULT_GOAL := all
override PHONY := all clean install install-strip uninstall $(TARGET_INSTALL)

$(OBJDIR)/.tag:
	@mkdir -p -- $(if $(MKDIRS),$(MKDIRS:%=$(OBJDIR)/%),$(OBJDIR))
	@touch $@

clean::
	rm -rf $(OBJDIR) $(NAME)

install-data: all

install-library: all
ifeq ($(DISABLE_MODULE), 0)
	@mkdir -p $(DESTDIR)$(LIBPATH)
	cp $(TARGET_MODULE) $(DESTDIR)$(LIBPATH)/
endif
ifneq ($(ENABLE_LIBRARY), 0)
	@mkdir -p $(DESTDIR)$(LIBDIR)/pkgconfig
ifneq ($(ENABLE_SHARED), 0)
	cp $(TARGET_SHARED) $(DESTDIR)$(LIBDIR)/
	cp -P $(OBJDIR)/$(LIB_MAJOR) $(DESTDIR)$(LIBDIR)/
	cp -P $(OBJDIR)/$(LIB_SHARED) $(DESTDIR)$(LIBDIR)/
endif
ifneq ($(ENABLE_STATIC), 0)
	cp $(TARGET_STATIC) $(DESTDIR)$(LIBDIR)/
endif
	sed -e 's|@PREFIX@|$(PREFIX)|' \
		-e 's|@EXEC_PREFIX@|$(PKGCONFEXECDIR)|' \
		-e 's|@LIBDIR@|$(PKGCONFLIBDIR)|' \
		-e 's|@INCLUDEDIR@|$(PKGCONFINCDIR)|' \
		-e 's|@VERSION@|$(VERSION)|' \
		-e 's|@DESCRIPTION@|$(DESCRIPTION)|' \
		-e 's|@NAME@|$(NAME)|' -e 's|@JGNAME@|$(JGNAME)|' \
		-e '/Libs:/a\' -e '$(LIBS_PRIVATE)' \
		-e '/URL:/a\' -e '$(REQUIRES_PRIVATE)' \
		$(SOURCEDIR)/lib/pkgconf.pc.in \
		> $(DESTDIR)$(LIBDIR)/pkgconfig/$(LIB_PC)
endif

ifneq ($(ENABLE_INSTALL), 0)
install: $(TARGET_INSTALL)
else
install: all
	@echo 'Nothing to install'
endif

install-strip: install
ifeq ($(DISABLE_MODULE), 0)
	strip $(DESTDIR)$(LIBPATH)/$(LIBRARY)
endif
ifneq ($(ENABLE_SHARED), 0)
	strip $(DESTDIR)$(LIBDIR)/$(LIB_VERSION)
endif

uninstall::
	rm -f $(DESTDIR)$(LIBPATH)/$(LIBRARY)
	rm -rf $(DESTDIR)$(DOCDIR)

ifneq ($(INSTALL_DATA), 0)
uninstall::
	rm -rf $(DESTDIR)$(DATADIR)/jollygood/$(NAME)
endif

ifneq ($(INSTALL_SHARED), 0)
uninstall::
	rm -f $(DESTDIR)$(LIBDIR)/$(LIB_STATIC)
	rm -f $(DESTDIR)$(LIBDIR)/$(LIB_SHARED)
	rm -f $(DESTDIR)$(LIBDIR)/$(LIB_MAJOR)
	rm -f $(DESTDIR)$(LIBDIR)/$(LIB_VERSION)
	rm -f $(DESTDIR)$(LIBDIR)/pkgconfig/$(LIB_PC)
endif
