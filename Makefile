ROOT							?= $(CURDIR)
API_ROOT						?= $(ROOT)/api
MANAGER_ROOT					?= $(ROOT)/manager
SETUP_ROOT						?= $(ROOT)/setup

.PHONY: all
all: api

.PHONY: api
api: manager
	$(MAKE) -C $(API_ROOT) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		PLATFORM=$(PLATFORM) \
		TA_DEV_KIT_DIR=$(TA_DEV_KIT_DIR)

.PHONY: manager
manager:
	$(MAKE) -C $(MANAGER_ROOT) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		PLATFORM=$(PLATFORM) \
		TA_DEV_KIT_DIR=$(TA_DEV_KIT_DIR)

.PHONY: setup
setup:
	$(MAKE) -C $(SETUP_ROOT) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		TEEC_EXPORT=$(TEEC_EXPORT) \
		--no-builtin-variables

.PHONY: install
install:
	mv $(API_ROOT)/tui.a $(TA_DEV_KIT_DIR)/lib/. && \
	mkdir -p $(TA_DEV_KIT_DIR)/include/tui && \
	cp $(API_ROOT)/include/* $(TA_DEV_KIT_DIR)/include/tui/.

.PHONY: clean
clean: api_clean manager_clean

.PHONY: setup_clean
setup_clean:
	$(MAKE) -C $(SETUP_ROOT) clean

.PHONY: api_clean
api_clean:
	$(MAKE) -C $(API_ROOT) clean \
		TA_DEV_KIT_DIR=$(TA_DEV_KIT_DIR)

.PHONY: manager_clean
manager_clean:
	$(MAKE) -C $(MANAGER_ROOT) clean \
		TA_DEV_KIT_DIR=$(TA_DEV_KIT_DIR)

.PHONY: uninstall
uninstall:
	rm -r $(TA_DEV_KIT_DIR)/include/tui; \
	rm $(TA_DEV_KIT_DIR)/lib/tui.a;