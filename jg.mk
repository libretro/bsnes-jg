DISABLE_MODULE ?= 0
ENABLE_SHARED ?= 0
ENABLE_STATIC ?= 0
ENABLE_STATIC_JG ?= 0

AR ?= ar
CC ?= cc
CXX ?= c++
CC_FOR_BUILD ?= $(CC)
CXX_FOR_BUILD ?= $(CXX)

VERSION := $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)

PKG_CONFIG ?= pkg-config
CFLAGS_JG := $(shell $(PKG_CONFIG) --cflags jg)

PIC := -fPIC
SHARED := $(PIC)

PREFIX ?= /usr/local
EXEC_PREFIX ?= $(PREFIX)
LIBDIR ?= $(EXEC_PREFIX)/lib
INCLUDEDIR ?= $(PREFIX)/include
DATAROOTDIR ?= $(PREFIX)/share
DATADIR ?= $(DATAROOTDIR)
DOCDIR ?= $(DATAROOTDIR)/doc/$(NAME)

INCPATH := $(INCLUDEDIR)/$(JGNAME)
LIBPATH := $(LIBDIR)/jollygood
OBJDIR := objs

LIBS_PRIVATE := Libs.private: $(LIBS)
REQUIRES_PRIVATE := Requires.private:

PKGCONF_SH := $(wildcard $(SOURCEDIR)/lib/pkgconf.sh)

ifneq ($(PKGCONF_SH),)
	PKGCONFLIBDIR := $(shell $(PKGCONF_SH) "$(EXEC_PREFIX)" "$(LIBDIR)" exec_)
	PKGCONFINCDIR := $(shell $(PKGCONF_SH) "$(PREFIX)" "$(INCLUDEDIR)")
endif

ifeq ($(PREFIX), $(EXEC_PREFIX))
	PKGCONFEXECDIR := $${prefix}
else
	PKGCONFEXECDIR := $(EXEC_PREFIX)
endif

UNAME := $(shell uname -s)
ifeq ($(UNAME), Darwin)
	LIBRARY := $(NAME).dylib
else ifeq ($(OS), Windows_NT)
	LIBRARY := $(NAME).dll
else
	LIBRARY := $(NAME).so
endif

LIB_PC := lib$(NAME).pc
LIB_SHARED := lib$(LIBRARY)
LIB_STATIC := lib$(NAME).a

ifeq ($(UNAME), Darwin)
	LIB_MAJOR := lib$(NAME).$(VERSION_MAJOR).dylib
	LIB_VERSION := lib$(NAME).$(VERSION).dylib
	SHARED += -dynamiclib -Wl,-undefined,error
	SONAME := -Wl,-install_name,$(LIB_MAJOR)
else
	LIB_MAJOR := $(LIB_SHARED).$(VERSION_MAJOR)
	LIB_VERSION := $(LIB_SHARED).$(VERSION)
	SHARED += -shared -Wl,--no-undefined
	SONAME := -Wl,-soname,$(LIB_MAJOR)
endif

# Desktop File
DESKTOP := $(JGNAME).desktop

DESKTOP_TARGET := $(NAME)/$(DESKTOP)

# Icons
ICONS := $(wildcard $(SOURCEDIR)/icons/*.png $(SOURCEDIR)/icons/$(NAME).svg)

ICONS_BASE := $(notdir $(ICONS))
ICONS_TARGET := $(ICONS_BASE:%=$(NAME)/icons/%)

# Library targets
TARGET :=
TARGET_MODULE := $(NAME)/$(LIBRARY)
TARGET_SHARED := $(OBJDIR)/$(LIB_VERSION)
TARGET_STATIC := $(OBJDIR)/$(LIB_STATIC)
TARGET_STATIC_JG := $(NAME)/lib$(NAME)-jg.a

ifeq ($(DISABLE_MODULE), 0)
	TARGET += $(TARGET_MODULE)
endif

ifneq ($(ENABLE_STATIC), 0)
	TARGET += $(TARGET_STATIC)
	OBJS_SHARED := $(TARGET_STATIC)
else
	OBJS_SHARED = $(OBJS)
endif

ifneq ($(ENABLE_SHARED), 0)
	TARGET += $(OBJDIR)/$(LIB_MAJOR) $(OBJDIR)/$(LIB_SHARED)
	LIBS_MODULE := -L$(OBJDIR) -l$(NAME)
else
	LIBS_MODULE = $(OBJS_SHARED)
endif

ifneq ($(ENABLE_STATIC_JG), 0)
	TARGET += $(DESKTOP_TARGET) $(ICONS_TARGET) $(NAME)/jg-static.mk
endif

ifneq ($(ENABLE_SHARED), 0)
	OBJS_MODULE := $(OBJDIR)/$(LIB_MAJOR) $(OBJDIR)/$(LIB_SHARED)
else ifneq ($(ENABLE_STATIC), 0)
	OBJS_MODULE := $(OBJS_SHARED)
else
	OBJS_MODULE = $(OBJS)
endif

ifneq (,$(filter-out 0,$(ENABLE_SHARED) $(ENABLE_STATIC)))
	ENABLE_LIB := 1
	ENABLE_PKGCONF := 1
else ifeq ($(DISABLE_MODULE), 0)
	ENABLE_LIB := 1
	ENABLE_PKGCONF := 0
else
	ENABLE_LIB := 0
	ENABLE_PKGCONF := 0
endif

# Compiler commands
COMPILE = $(strip $(1) $(CPPFLAGS) $(PIC) $(2) -c $< -o $@)
COMPILE_C = $(call COMPILE, $(CC) $(CFLAGS), $(1))
COMPILE_CXX = $(call COMPILE, $(CXX) $(CXXFLAGS), $(1))
COMPILE_C_BUILD = $(strip $(CC_FOR_BUILD) $(1) $< -o $@)
COMPILE_CXX_BUILD = $(strip $(CXX_FOR_BUILD) $(1) $< -o $@)

# Info command
COMPILE_INFO = $(info $(subst $(SOURCEDIR)/,,$(1)))
