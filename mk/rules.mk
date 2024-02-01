$(OBJDIR)/%.o: $(SOURCEDIR)/%.$(EXT) $(PREREQ)
	$(call COMPILE_INFO,$(BUILD_JG))
	@$(BUILD_JG)

$(TARGET_MODULE): $(OBJS_JG) $(OBJS_MODULE)
	@mkdir -p $(NAME)
	$(LINK_MODULE)

$(TARGET_STATIC_JG): $(OBJS_JG) $(OBJS_SHARED)
	@mkdir -p $(NAME)
	$(AR) -rcs $@ $^

$(TARGET_DESKTOP): $(SOURCEDIR)/$(DESKTOP)
	@mkdir -p $(NAME)
	@cp $< $@

$(TARGET_STATIC_MK): $(TARGET_STATIC_JG)
	@printf '%s\n%s\n%s\n%s\n' 'NAME := $(JGNAME)' \
		'$(if $(DATA),ASSETS := $(DATA),ASSETS :=)' \
		'$(if $(ICONS),ICONS := $(ICONS),ICONS :=)' \
		'$(if $(LIBS),LIBS_STATIC := $(LIBS),LIBS_STATIC :=)' > $@

static-jg: $(TARGET_DESKTOP) $(TARGET_ICONS) $(TARGET_STATIC_MK)

clean::
	rm -rf $(OBJDIR) $(NAME)

module: $(TARGET_MODULE)

install-module: module
	@mkdir -p $(DESTDIR)$(LIBPATH)
	cp $(TARGET_MODULE) $(DESTDIR)$(LIBPATH)/

install-strip-module: install-module
	strip $(DESTDIR)$(LIBPATH)/$(LIBRARY)

install: $(if $(TARGET_INSTALL),$(TARGET_INSTALL),all)
	@$(if $(TARGET_INSTALL),,echo 'Nothing to install')

install-strip: install $(TARGET_STRIP)

uninstall::
	rm -f $(DESTDIR)$(LIBPATH)/$(LIBRARY)
	rm -rf $(DESTDIR)$(DOCDIR)

ifneq ($(ICONS),)
$(TARGET_ICONS): $(ICONS_BASE)
	@mkdir -p $(NAME)/icons
	@cp $(subst $(NAME)/icons,$(SOURCEDIR)/icons,$@) $(NAME)/icons/
endif

ifneq ($(INSTALL_DATA), 0)
data: $(DATA_TARGET)

install-data:

uninstall::
	rm -rf $(DESTDIR)$(DATADIR)/jollygood/$(NAME)
endif

ifneq ($(INSTALL_EXAMPLE), 0)
$(OBJDIR)/$(EXAMPLE)/%.o: $(SOURCEDIR)/$(EXAMPLE)/%.$(EXT) $(OBJDIR)/.tag
	$(call COMPILE_INFO,$(BUILD_EXAMPLE))
	@$(BUILD_EXAMPLE)

$(BIN_EXAMPLE): $(OBJS_BIN) $(OBJS_MODULE)
	$(call LINK_BIN,$(LIBS_BIN))

$(TARGET_BIN): $(BIN_EXAMPLE)
	@sed 's|@EXAMPLE@|$(EXAMPLE)|' $(SOURCEDIR)/lib/bin.in > $@
	@chmod 0755 $@

example: $(TARGET_BIN)

install-bin: example
	@mkdir -p $(DESTDIR)$(BINDIR)
	cp $(BIN_EXAMPLE) $(DESTDIR)$(BINDIR)/

install-strip-bin: install-bin
	strip $(DESTDIR)$(BINDIR)/$(BIN_NAME)

uninstall::
	rm -f $(DESTDIR)$(BINDIR)/$(BIN_NAME)

ifneq ($(ENABLE_EXAMPLE), 0)
ifneq ($(DOCS_EXAMPLE),)
install-docs::
	for i in $(DOCS_EXAMPLE); do \
		cp $(SOURCEDIR)/$(EXAMPLE)/$$i \
			$(DESTDIR)$(DOCDIR)/$$i-example; \
	done
endif
endif
endif

ifneq ($(INSTALL_SHARED), 0)
$(TARGET_SHARED): $(OBJS)
	$(LINK_SHARED)

$(TARGET_STATIC): $(OBJS)
	$(AR) -rcs $@ $^

$(OBJDIR)/$(LIB_MAJOR) $(OBJDIR)/$(LIB_SHARED): $(TARGET_SHARED)
	ln -s $(LIB_VERSION) $@

shared: $(OBJDIR)/$(LIB_MAJOR) $(OBJDIR)/$(LIB_SHARED)

install-shared: shared
	@mkdir -p $(DESTDIR)$(LIBDIR)
	cp $(TARGET_SHARED) $(DESTDIR)$(LIBDIR)/
	cp -P $(OBJDIR)/$(LIB_MAJOR) $(DESTDIR)$(LIBDIR)/
	cp -P $(OBJDIR)/$(LIB_SHARED) $(DESTDIR)$(LIBDIR)/

install-strip-shared: install-shared
	strip $(DESTDIR)$(LIBDIR)/$(LIB_VERSION)

static: $(TARGET_STATIC)

install-static: static
	@mkdir -p $(DESTDIR)$(LIBDIR)
	cp $(TARGET_STATIC) $(DESTDIR)$(LIBDIR)/

install-pkgconfig: $(SOURCEDIR)/lib/pkgconf.pc.in
	@mkdir -p $(DESTDIR)$(LIBDIR)/pkgconfig
	sed -e 's|@PREFIX@|$(PREFIX)|' \
		-e 's|@EXEC_PREFIX@|$(PKGCONFEXECDIR)|' \
		-e 's|@LIBDIR@|$(PKGCONFLIBDIR)|' \
		-e 's|@INCLUDEDIR@|$(PKGCONFINCDIR)|' \
		-e 's|@VERSION@|$(VERSION)|' \
		-e 's|@DESCRIPTION@|$(DESCRIPTION)|' \
		-e 's|@NAME@|$(NAME)|' \
		-e '/Libs:/a\' -e '$(LIBS_PRIVATE)' \
		-e '/URL:/a\' -e '$(REQUIRES_PRIVATE)' \
		$< \
		> $(DESTDIR)$(LIBDIR)/pkgconfig/$(LIB_PC)

uninstall::
	rm -f $(DESTDIR)$(LIBDIR)/$(LIB_STATIC)
	rm -f $(DESTDIR)$(LIBDIR)/$(LIB_SHARED)
	rm -f $(DESTDIR)$(LIBDIR)/$(LIB_MAJOR)
	rm -f $(DESTDIR)$(LIBDIR)/$(LIB_VERSION)
	rm -f $(DESTDIR)$(LIBDIR)/pkgconfig/$(LIB_PC)

install-headers: $(HEADERS:%=$(SOURCEDIR)/%)
ifneq ($(HEADERS),)
	@mkdir -p $(DESTDIR)$(INCPATH)
	for i in $(HEADERS); do \
		cp $(SOURCEDIR)/$$i $(DESTDIR)$(INCPATH)/; \
	done

uninstall::
	rm -rf $(DESTDIR)$(INCPATH)
endif
endif
