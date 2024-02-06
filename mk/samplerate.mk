DIR_SAMPLERATE := $(wildcard $(DEPDIR)/libsamplerate)

ifneq ($(DIR_SAMPLERATE),)
	USE_VENDORED_SAMPLERATE ?= 0
else
	override USE_VENDORED_SAMPLERATE := 0
endif

ifneq ($(USE_VENDORED_SAMPLERATE), 0)
	CFLAGS_SAMPLERATE := -I$(DIR_SAMPLERATE)
	LIBS_SAMPLERATE := $(if $(findstring -lm,$(LIBS)),,-lm)
	MKDIRS += deps/libsamplerate
	SRCS_SAMPLERATE := deps/libsamplerate/samplerate.c \
		deps/libsamplerate/src_linear.c \
		deps/libsamplerate/src_sinc.c \
		deps/libsamplerate/src_zoh.c
	OBJS_SAMPLERATE := $(SRCS_SAMPLERATE:.c=.o)
else
	CFLAGS_SAMPLERATE = $(shell $(PKG_CONFIG) --cflags samplerate)
	LIBS_SAMPLERATE = $(shell $(PKG_CONFIG) --libs samplerate)
	OBJS_SAMPLERATE :=
endif

ifneq ($(findstring samplerate,$(LIBS_REQUIRES)),)
	ifneq ($(USE_VENDORED_SAMPLERATE), 0)
		override LIBS_PRIVATE += $(LIBS_SAMPLERATE)
	else
		override REQUIRES_PRIVATE += samplerate
	endif
endif

ifneq ($(DIR_SAMPLERATE),)
FLAGS_SAMPLERATE := -std=c99 $(WARNINGS_DEF_C)

BUILD_SAMPLERATE = $(call COMPILE_C, $(FLAGS_SAMPLERATE))

$(OBJDIR)/deps/libsamplerate/%.o: $(DIR_SAMPLERATE)/%.c $(OBJDIR)/.tag
	$(call COMPILE_INFO,$(BUILD_SAMPLERATE))
	@$(BUILD_SAMPLERATE)

ifneq ($(USE_VENDORED_SAMPLERATE), 0)
install-docs::
	cp $(DIR_SAMPLERATE)/COPYING \
		$(DESTDIR)$(DOCDIR)/COPYING-libsamplerate
endif
endif
