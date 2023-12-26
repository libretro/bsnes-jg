$(TARGET_MODULE): $(OBJS_JG) $(OBJS_MODULE)
	@mkdir -p $(NAME)
	$(LINK_MODULE)

$(TARGET_STATIC_JG): $(OBJS_JG) $(OBJS_SHARED)
	@mkdir -p $(NAME)
	$(AR) rcs $@ $^

$(DESKTOP_TARGET): $(SOURCEDIR)/$(DESKTOP)
	@mkdir -p $(NAME)
	@cp $< $@

$(TARGET_STATIC_MK): $(TARGET_STATIC_JG)
	@printf '%s\n%s\n%s\n%s\n' 'NAME := $(JGNAME)' \
		'$(if $(DATA),ASSETS := $(DATA),ASSETS :=)' \
		'$(if $(ICONS),ICONS := $(ICONS),ICONS :=)' \
		'$(if $(LIBS),LIBS_STATIC := $(LIBS),LIBS_STATIC :=)' > $@

ifneq ($(ICONS),)
$(ICONS_TARGET): $(ICONS_BASE)
	@mkdir -p $(NAME)/icons
	@cp $(subst $(NAME)/icons,$(SOURCEDIR)/icons,$@) $(NAME)/icons/
endif

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
ifneq ($(HEADERS),)
	@mkdir -p $(DESTDIR)$(INCPATH)
	for i in $(HEADERS); do \
		cp $(SOURCEDIR)/$$i $(DESTDIR)$(INCPATH)/; \
	done
endif
	sed -e 's|@PREFIX@|$(PREFIX)|' \
		-e 's|@EXEC_PREFIX@|$(PKGCONFEXECDIR)|' \
		-e 's|@LIBDIR@|$(PKGCONFLIBDIR)|' \
		-e 's|@INCLUDEDIR@|$(PKGCONFINCDIR)|' \
		-e 's|@VERSION@|$(VERSION)|' \
		-e 's|@DESCRIPTION@|$(DESCRIPTION)|' \
		-e 's|@NAME@|$(NAME)|' \
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
$(TARGET_SHARED): $(OBJS)
	$(LINK_SHARED)

$(TARGET_STATIC): $(OBJS)
	$(AR) rcs $@ $^

$(OBJDIR)/$(LIB_MAJOR) $(OBJDIR)/$(LIB_SHARED): $(TARGET_SHARED)
	ln -s $(LIB_VERSION) $@

uninstall::
	rm -f $(DESTDIR)$(LIBDIR)/$(LIB_STATIC)
	rm -f $(DESTDIR)$(LIBDIR)/$(LIB_SHARED)
	rm -f $(DESTDIR)$(LIBDIR)/$(LIB_MAJOR)
	rm -f $(DESTDIR)$(LIBDIR)/$(LIB_VERSION)
	rm -f $(DESTDIR)$(LIBDIR)/pkgconfig/$(LIB_PC)

ifneq ($(HEADERS),)
uninstall::
	rm -rf $(DESTDIR)$(INCPATH)
endif
endif
