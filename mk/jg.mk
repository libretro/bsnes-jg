DISABLE_MODULE ?= 0
ENABLE_EXAMPLE ?= 0
ENABLE_SHARED ?= 0
ENABLE_STATIC ?= 0
ENABLE_STATIC_JG ?= 0

include $(SOURCEDIR)/mk/common.mk

override VERSION := $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)

override PIC := -fPIC
override SHARED := $(PIC)

override INCPATH := $(INCLUDEDIR)/$(NAME)
override LIBPATH := $(LIBDIR)/jollygood

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
	override SHARED += -dynamiclib
	override SONAME := -Wl,-install_name,$(LIB_MAJOR)
else
	override LIB_MAJOR := $(LIB_SHARED).$(VERSION_MAJOR)
	override LIB_VERSION := $(LIB_SHARED).$(VERSION)
	override SHARED += -shared
	override SONAME := -Wl,-soname,$(LIB_MAJOR)
endif

ifeq ($(UNAME), Darwin)
	override SHARED += -Wl,-undefined,error
else ifneq ($(UNAME), OpenBSD)
	override SHARED += -Wl,--no-undefined
endif

# Prerequisites
override PREREQ := $(OBJDIR)/.tag

# Desktop File
override DESKTOP := $(JGNAME).desktop

# Example
override BIN_NAME := $(NAME)-example
override BIN_EXAMPLE := $(OBJDIR)/$(EXAMPLE)/$(BIN_NAME)

# Icons
override ICONS_BASE := $(wildcard $(SOURCEDIR)/icons/*.png \
	$(SOURCEDIR)/icons/$(NAME).svg)
override ICONS := $(notdir $(ICONS_BASE))

# Targets
override TARGET :=
override TARGET_INSTALL :=
override TARGET_STRIP :=
override TARGET_BIN := $(OBJDIR)/$(BIN_NAME)
override TARGET_DESKTOP := $(NAME)/$(DESKTOP)
override TARGET_ICONS := $(ICONS:%=$(NAME)/icons/%)
override TARGET_MODULE := $(NAME)/$(LIBRARY)
override TARGET_SHARED := $(OBJDIR)/$(LIB_VERSION)
override TARGET_STATIC := $(OBJDIR)/$(LIB_STATIC)
override TARGET_STATIC_JG := $(NAME)/lib$(NAME)-jg.a
override TARGET_STATIC_MK := $(NAME)/jg-static.mk

override PHONY += module install-module install-strip-module static-jg

ifeq ($(INSTALL_EXAMPLE), 0)
	override ENABLE_EXAMPLE := 0
else
	override PHONY += example install-bin install-strip-bin
	override MKDIRS += $(EXAMPLE)
endif

ifeq ($(INSTALL_DATA), 0)
	override DATA :=
else
	override PHONY += data install-data
	ifneq (,$(or $(filter-out 0,$(ENABLE_EXAMPLE) $(ENABLE_STATIC_JG)), \
			$(filter 0,$(DISABLE_MODULE))))
		override TARGET += data
	endif
endif

ifeq ($(INSTALL_SHARED), 0)
	override HEADERS :=
	override ENABLE_SHARED := 0
	override ENABLE_STATIC := 0
else
	PHONY += \
		shared install-shared install-strip-shared \
		static install-static install-strip-static \
		install-headers install-pkgconfig
endif

ifneq ($(ENABLE_EXAMPLE), 0)
	override TARGET += example
	override TARGET_INSTALL += install-bin
	override TARGET_STRIP += install-strip-bin
endif

ifeq ($(DISABLE_MODULE), 0)
	override TARGET += module
	override TARGET_INSTALL += install-module
	override TARGET_STRIP += install-strip-module
endif

ifneq ($(ENABLE_STATIC), 0)
	override TARGET += static
	override TARGET_INSTALL += install-static
	override OBJS_SHARED := $(TARGET_STATIC)
else
	override OBJS_SHARED = $(OBJS)
endif

ifneq ($(ENABLE_SHARED), 0)
	override TARGET += shared
	override TARGET_INSTALL += install-shared
	override TARGET_STRIP += install-shared-strip
	override LIBS_MODULE := -L$(OBJDIR) -l$(NAME)
else
	override LIBS_MODULE = $(OBJS_SHARED)
endif

ifneq ($(ENABLE_STATIC_JG), 0)
	override TARGET += static-jg
endif

ifneq ($(ENABLE_SHARED), 0)
	override OBJS_MODULE := $(OBJDIR)/$(LIB_MAJOR) $(OBJDIR)/$(LIB_SHARED)
else ifneq ($(ENABLE_STATIC), 0)
	override OBJS_MODULE := $(OBJS_SHARED)
else
	override OBJS_MODULE = $(OBJS)
endif

ifneq (,$(filter-out 0,$(ENABLE_SHARED) $(ENABLE_STATIC)))
	override TARGET_INSTALL += install-headers install-pkgconfig
endif

ifneq ($(TARGET_INSTALL),)
	override TARGET_INSTALL += install-docs
endif

# Compiler commands
override COMPILE = $(strip $(1) $(CPPFLAGS) $(PIC) $(2) -c $< -o $@)
override COMPILE_C = $(call COMPILE, $(CC) $(CFLAGS), $(1))
override COMPILE_CXX = $(call COMPILE, $(CXX) $(CXXFLAGS), $(1))
override COMPILE_C_BUILD = $(strip $(CC_FOR_BUILD) $(1) $< -o $@)
override COMPILE_CXX_BUILD = $(strip $(CXX_FOR_BUILD) $(1) $< -o $@)
override LINK = $(strip $(LINKER) -o $@ $(1) $(LDFLAGS) $(LIBS) $(SHARED))
override LINK_BIN = $(strip $(LINKER) -o $@ $(LDFLAGS) $(OBJS_BIN) $(LIBS_MODULE) $(1))
override LINK_MODULE = $(call LINK, $(OBJS_JG) $(LIBS_MODULE))
override LINK_SHARED = $(call LINK, $^ $(SONAME))
