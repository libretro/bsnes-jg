$(OBJDIR)/%.o: $(SOURCEDIR)/%.$(EXT) $(PREREQ)
	$(call COMPILE_INFO,$(BUILD_JG))
	@$(BUILD_JG)

$(TARGET_MODULE): $(OBJS_JG) $(OBJS_MODULE) $(OBJDIR)/module.map
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
		'$(strip ASSETS := $(DATA))' \
		'$(strip ICONS := $(ICONS))' \
		'$(strip LIBS_STATIC := $(LIBS) $(LIBS_JG))' > $@

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

ifeq ($(UNAME), Darwin)
$(OBJDIR)/module.map: $(OBJDIR)/.tag
	@printf '%s\n' '_jg_*' > $@
else
$(OBJDIR)/module.map: $(SOURCEDIR)/link.in $(OBJDIR)/.tag
	@sed -e 's|@NAME@||' -e 's|@SYMBOLS@|jg_*;|' $< > $@
endif

ifneq ($(DOXYFILE),)
$(HTML_OUT)/.tag:
	@mkdir -p $(HTML_OUT)
	@touch $@

$(TARGET_HTML): $(HEADERS) $(HTML_OUT)/.tag
	@cp $(HEADERS:%=$(SOURCEDIR)/%) $(HTML_OUT)/

$(HTML_OUT)/Doxyfile: $(DOXYFILE) $(TARGET_HTML)
	@sed -e 's|@NAME@|$(NAME)|' \
		-e 's|@DESCRIPTION@|$(DESCRIPTION)|' \
		-e 's|@VERSION@|$(VERSION)|' \
		$< \
		> $@

doxyfile: $(HTML_OUT)/Doxyfile

$(HTML_OUT)/doxyfile.tag: $(HTML_OUT)/Doxyfile
	cd $(HTML_OUT) && $(DOXYGEN) Doxyfile
	@touch $@

html: $(HTML_OUT)/doxyfile.tag

install-html: html
	@mkdir -p $(DESTDIR)$(HTMLDIR)
	for i in $(HTML_OUT)/html/*; do \
		cp $$i $(DESTDIR)/$(HTMLDIR); \
	done

uninstall::
	rm -rf $(DESTDIR)$(HTMLDIR)
endif

ifneq ($(ICONS),)
$(TARGET_ICONS): $(ICONS_BASE)
	@mkdir -p $(NAME)/icons
	@cp $(subst $(NAME)/icons,$(SOURCEDIR)/icons,$@) $(NAME)/icons/
endif

ifneq ($(INSTALL_DATA), 0)
data: $(PREREQ_DATA)

uninstall::
	rm -rf $(DESTDIR)$(DATADIR)/jollygood/$(NAME)
endif

ifneq ($(INSTALL_EXAMPLE), 0)
$(BIN_EXAMPLE): $(OBJS_BIN) $(OBJS_MODULE)
	$(LINK_BIN)

$(TARGET_BIN): $(SOURCEDIR)/lib/bin.in $(BIN_EXAMPLE)
	@sed -e 's|@EXAMPLE@|$(EXAMPLE)|' $< > $@
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
$(TARGET_SHARED): $(OBJS) $(OBJDIR)/shared.map
	$(LINK_SHARED)

$(TARGET_STATIC): $(OBJS)
	$(AR) -rcs $@ $^

$(OBJDIR)/$(LIB_MAJOR) $(OBJDIR)/$(LIB_SHARED): $(TARGET_SHARED)
	ln -sf $(LIB_VERSION) $@

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

install-pkgconfig: $(SOURCEDIR)/lib/pkgconf.pc.in all
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

ifeq ($(UNAME), Darwin)
$(OBJDIR)/shared.map: $(OBJDIR)/.tag
	@printf '%s\n' '$(SYMBOLS_LIST)' | sed -e 's/ /\n/g' > $@
else
$(OBJDIR)/shared.map: $(SOURCEDIR)/link.in $(OBJDIR)/.tag
	@sed -e 's|@NAME@|$(VERSION_SCRIPT_NAME) |' \
		-e "s|@SYMBOLS@|$(SYMBOLS_LIST)|" $< > $@
endif

install-headers: $(HEADERS:%=$(SOURCEDIR)/%) all
ifneq ($(HEADERS),)
	@mkdir -p $(DESTDIR)$(INCPATH)
	for i in $(HEADERS); do \
		cp $(SOURCEDIR)/$$i $(DESTDIR)$(INCPATH)/; \
	done

uninstall::
	rm -rf $(DESTDIR)$(INCPATH)
endif
endif
