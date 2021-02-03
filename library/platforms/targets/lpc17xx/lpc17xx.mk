LIBRARY_LPC17XX += $(LIBRARY_DIR)/platforms/targets/lpc17xx/startup.cpp
LIBRARY_LPC17XX += $(LIBRARY_DIR)/platforms/processors/arm_cortex/m3/ARM_CM3/port.c
LIBRARY_LPC17XX += $(LIBRARY_DIR)/platforms/processors/arm_cortex/exceptions.cpp

OPENOCD_CONFIG  ?= $(LIBRARY_DIR)/platforms/lpc17xx/lpc17xx.cfg

$(eval $(call BUILD_LIBRARY,liblpc17xx,LIBRARY_LPC17XX))

include $(LIBRARY_DIR)/platforms/processors/arm_cortex/m3/m3.mk

platform-flash:
	@echo
	@bash -c "\
	source $(SJ2_TOOLS_DIR)/nxpprog/modules/bin/activate && \
	python3 $(SJ2_TOOLS_DIR)/nxpprog/nxpprog.py \
	--binary=\"$(BINARY)\" --device=\"$(SJDEV)\" \
	--osfreq=12000000 --baud=230400 --control"
	@echo
