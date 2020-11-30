LIBRARY_LPC40XX += $(LIBRARY_DIR)/L0_Platform/lpc40xx/startup.cpp
LIBRARY_LPC40XX += $(LIBRARY_DIR)/L0_Platform/arm_cortex/m4/ARM_CM4F/port.c
LIBRARY_LPC40XX += $(LIBRARY_DIR)/L0_Platform/arm_cortex/exceptions.cpp

OPENOCD_CONFIG  ?= $(LIBRARY_DIR)/L0_Platform/lpc40xx/lpc40xx.cfg

$(eval $(call BUILD_LIBRARY,liblpc40xx,LIBRARY_LPC40XX))

include $(LIBRARY_DIR)/L0_Platform/arm_cortex/m4/m4.mk

platform-flash:
	@echo
	@bash -c "\
	source $(SJ2_TOOLS_DIR)/nxpprog/modules/bin/activate && \
	python3 $(SJ2_TOOLS_DIR)/nxpprog/nxpprog.py \
	--binary=\"$(BINARY)\" --device=\"$(PORT)\" \
	--osfreq=12000000 --baud=115200 --control"
	@echo
