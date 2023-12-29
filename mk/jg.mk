DISABLE_MODULE ?= 0
ENABLE_SHARED ?= 0
ENABLE_STATIC ?= 0
ENABLE_STATIC_JG ?= 0

include $(SOURCEDIR)/mk/common.mk

override VERSION := $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)

override PIC := -fPIC
override SHARED := $(PIC)

override INCPATH := $(INCLUDEDIR)/$(NAME)
override LIBPATH := $(LIBDIR)/jollygood

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
override ICONS_BASE := $(wildcard $(SOURCEDIR)/icons/*.png \
	$(SOURCEDIR)/icons/$(NAME).svg)
override ICONS := $(notdir $(ICONS_BASE))
override ICONS_TARGET := $(ICONS:%=$(NAME)/icons/%)

# Library targets
override TARGET :=
override TARGET_INSTALL := install-data install-docs install-library
override TARGET_MODULE := $(NAME)/$(LIBRARY)
override TARGET_SHARED := $(OBJDIR)/$(LIB_VERSION)
override TARGET_STATIC := $(OBJDIR)/$(LIB_STATIC)
override TARGET_STATIC_JG := $(NAME)/lib$(NAME)-jg.a
override TARGET_STATIC_MK := $(NAME)/jg-static.mk

ifeq ($(INSTALL_DATA), 0)
	override DATA :=
endif

ifeq ($(INSTALL_SHARED), 0)
	override HEADERS :=
	override ENABLE_SHARED := 0
	override ENABLE_STATIC := 0
endif

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
override LINK = $(strip $(LINKER) -o $@ $(1) $(LDFLAGS) $(LIBS) $(SHARED))
override LINK_MODULE = $(call LINK, $(OBJS_JG) $(LIBS_MODULE))
override LINK_SHARED = $(call LINK, $^ $(SONAME))

override PHONY += $(TARGET_INSTALL)
