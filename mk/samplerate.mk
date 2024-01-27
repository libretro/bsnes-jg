DIR_SAMPLERATE := $(wildcard $(DEPDIR)/libsamplerate)

ifneq ($(DIR_SAMPLERATE),)
	USE_VENDORED_SAMPLERATE ?= 0
else
	override USE_VENDORED_SAMPLERATE := 0
endif

ifneq ($(USE_VENDORED_SAMPLERATE), 0)
	CFLAGS_SAMPLERATE := -I$(DIR_SAMPLERATE)
	LIBS_SAMPLERATE := $(if $(findstring -lm,$(LIBS)),,-lm)
	override LIBS_PRIVATE += $(LIBS_SAMPLERATE)
	MKDIRS += deps/libsamplerate
	SRCS_SAMPLERATE := deps/libsamplerate/samplerate.c \
		deps/libsamplerate/src_linear.c \
		deps/libsamplerate/src_sinc.c \
		deps/libsamplerate/src_zoh.c
	OBJS_SAMPLERATE := $(SRCS_SAMPLERATE:.c=.o)

install-docs::
	cp $(DIR_SAMPLERATE)/COPYING \
		$(DESTDIR)$(DOCDIR)/COPYING-libsamplerate
else
	override REQUIRES_PRIVATE += samplerate
	CFLAGS_SAMPLERATE = $(shell $(PKG_CONFIG) --cflags samplerate)
	LIBS_SAMPLERATE = $(shell $(PKG_CONFIG) --libs samplerate)
	OBJS_SAMPLERATE :=
endif
