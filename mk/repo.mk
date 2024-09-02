$(BUILD)/fetch.tag: prefix $(FSTOOLS_TAG) $(FILESYSTEM_CONFIG) $(CONTAINER_TAG)
ifeq ($(PODMAN_BUILD),1)
	$(PODMAN_RUN) $(MAKE) $@
else
	PACKAGES="$$($(INSTALLER) --list-packages -c $(FILESYSTEM_CONFIG))" && \
	cd cookbook && \
	./fetch.sh "$${PACKAGES}"
	mkdir -p $(BUILD)
	touch $@
endif

$(BUILD)/repo.tag: $(BUILD)/fetch.tag $(FSTOOLS_TAG) $(CONTAINER_TAG)
ifeq ($(PODMAN_BUILD),1)
	$(PODMAN_RUN) $(MAKE) $@
else
	export PATH="$(PREFIX_PATH):$$PATH" && \
	PACKAGES="$$($(INSTALLER) --list-packages -c $(FILESYSTEM_CONFIG))" && \
	cd cookbook && \
	./repo.sh $(REPO_NONSTOP) "$${PACKAGES}"
	mkdir -p $(BUILD)
	# make sure fetch.tag is newer than the things repo modifies
	touch $<
	touch $@
endif

# Invoke clean.sh for a single target
c.%: $(FSTOOLS_TAG) FORCE
ifeq ($(PODMAN_BUILD),1)
	$(PODMAN_RUN) $(MAKE) $@
else
	export PATH="$(PREFIX_PATH):$$PATH" && \
	cd cookbook && \
	./clean.sh $*
endif

# Invoke fetch.sh for a single target
f.%: $(FSTOOLS_TAG) FORCE
ifeq ($(PODMAN_BUILD),1)
	$(PODMAN_RUN) $(MAKE) $@
else
	export PATH="$(PREFIX_PATH):$$PATH" && \
	cd cookbook && \
	./fetch.sh $*
endif

# Invoke repo.sh for a single target
r.%: $(FSTOOLS_TAG) FORCE
ifeq ($(PODMAN_BUILD),1)
	$(PODMAN_RUN) $(MAKE) $@
else
	export PATH="$(PREFIX_PATH):$$PATH" && \
	cd cookbook && \
	./repo.sh $*
endif

# Invoke unfetch.sh for a single target
u.%: $(FSTOOLS_TAG) FORCE
ifeq ($(PODMAN_BUILD),1)
	$(PODMAN_RUN) $(MAKE) $@
else
	export PATH="$(PREFIX_PATH):$$PATH" && \
	cd cookbook && \
	./unfetch.sh $*
endif

# Invoke clean.sh, and repo.sh for a single target
cr.%: $(FSTOOLS_TAG) FORCE
	$(MAKE) c.$*
	$(MAKE) r.$*

# Invoke unfetch.sh, clean.sh, and repo.sh for a single target
ucr.%: $(FSTOOLS_TAG) FORCE
	$(MAKE) u.$*
	$(MAKE) cr.$*

uc.%: $(FSTOOLS_TAG) FORCE
	$(MAKE) u.$*
	$(MAKE) c.$*

ucf.%: (FSTOOLS_TAG) FORCE
	$(MAKE) uc.$*
	$(MAKE) f.$*
