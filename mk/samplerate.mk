USE_VENDORED_SAMPLERATE ?= 0

ifneq ($(USE_VENDORED_SAMPLERATE), 0)
	CFLAGS_SAMPLERATE := -I$(DEPDIR)/libsamplerate
	LIBS_SAMPLERATE := $(if $(findstring -lm,$(LIBS)),,-lm)
	override LIBS_PRIVATE += $(LIBS_SAMPLERATE)
	MKDIRS += deps/libsamplerate
	CSRCS += deps/libsamplerate/samplerate.c \
		deps/libsamplerate/src_linear.c \
		deps/libsamplerate/src_sinc.c \
		deps/libsamplerate/src_zoh.c
else
	override REQUIRES_PRIVATE += samplerate
	CFLAGS_SAMPLERATE = $(shell $(PKG_CONFIG) --cflags samplerate)
	LIBS_SAMPLERATE = $(shell $(PKG_CONFIG) --libs samplerate)
endif
