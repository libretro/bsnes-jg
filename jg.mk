AR ?= ar
CC ?= cc
CXX ?= c++
CC_FOR_BUILD ?= $(CC)
CXX_FOR_BUILD ?= $(CXX)

PKG_CONFIG ?= pkg-config
CFLAGS_JG := $(shell $(PKG_CONFIG) --cflags jg)

PIC := -fPIC
SHARED := $(PIC)

PREFIX ?= /usr/local
EXEC_PREFIX ?= $(PREFIX)
LIBDIR ?= $(EXEC_PREFIX)/lib
DATAROOTDIR ?= $(PREFIX)/share
DATADIR ?= $(DATAROOTDIR)
DOCDIR ?= $(DATAROOTDIR)/doc/$(NAME)

LIBPATH := $(LIBDIR)/jollygood
OBJDIR := objs

DISABLE_MODULE ?= 0
ENABLE_STATIC_JG ?= 0

UNAME := $(shell uname -s)
ifeq ($(UNAME), Darwin)
	LIBRARY := $(NAME).dylib
else ifeq ($(OS), Windows_NT)
	LIBRARY := $(NAME).dll
else
	LIBRARY := $(NAME).so
endif

ifeq ($(UNAME), Darwin)
	SHARED += -dynamiclib -Wl,-undefined,error
else
	SHARED += -shared -Wl,--no-undefined
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
TARGET_STATIC_JG := $(NAME)/lib$(NAME)-jg.a

ifeq ($(DISABLE_MODULE), 0)
	TARGET += $(TARGET_MODULE)
endif

ifneq ($(ENABLE_STATIC_JG), 0)
	TARGET += $(DESKTOP_TARGET) $(ICONS_TARGET) $(NAME)/jg-static.mk
endif

# Compiler commands
COMPILE = $(strip $(1) $(CPPFLAGS) $(PIC) $(2) -c $< -o $@)
COMPILE_C = $(call COMPILE, $(CC) $(CFLAGS), $(1))
COMPILE_CXX = $(call COMPILE, $(CXX) $(CXXFLAGS), $(1))
COMPILE_C_BUILD = $(strip $(CC_FOR_BUILD) $(1) $< -o $@)
COMPILE_CXX_BUILD = $(strip $(CXX_FOR_BUILD) $(1) $< -o $@)

# Info command
COMPILE_INFO = $(info $(subst $(SOURCEDIR)/,,$(1)))
