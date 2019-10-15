# sjsu_dev2.mk holds the $(SJSU_DEV2_BASE) variable which holds the location of
# the SJSU-Dev2 folder.
include ~/.sjsu_dev2.mk

ifndef SJSU_DEV2_BASE
$(info +-------------- SJSU-Dev2 Location file not found --------------+)
$(info |                                                               |)
$(info |        Run ./setup from within the SJSU-Dev2's folder         |)
$(info |                                                               |)
$(info +---------------------------------------------------------------+)
$(error )
endif

# Using the directory location, include the project makefile
include $(SJSU_DEV2_BASE)/makefile
