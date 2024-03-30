-include $(SOURCEDIR)/mk/miniz.mk
-include $(SOURCEDIR)/mk/samplerate.mk
-include $(SOURCEDIR)/mk/speexdsp.mk
-include $(SOURCEDIR)/mk/soxr.mk

CFLAGS_JG = $(shell $(PKG_CONFIG) --cflags jg)

CFLAGS_EPOXY = $(shell $(PKG_CONFIG) --cflags epoxy)
LIBS_EPOXY = $(shell $(PKG_CONFIG) --libs epoxy)

CFLAGS_FLAC = $(shell $(PKG_CONFIG) --cflags flac)
LIBS_FLAC = $(shell $(PKG_CONFIG) --libs flac)

CFLAGS_LZO = $(shell $(PKG_CONFIG) --cflags lzo2)
LIBS_LZO = $(shell $(PKG_CONFIG) --libs lzo2)

CFLAGS_SDL2 = $(shell $(PKG_CONFIG) --cflags sdl2)
LIBS_SDL2 = $(shell $(PKG_CONFIG) --libs sdl2)

CFLAGS_ZLIB = $(shell $(PKG_CONFIG) --cflags zlib)
LIBS_ZLIB = $(shell $(PKG_CONFIG) --libs zlib)

CFLAGS_ZSTD = $(shell $(PKG_CONFIG) --cflags libzstd)
LIBS_ZSTD = $(shell $(PKG_CONFIG) --libs libzstd)
