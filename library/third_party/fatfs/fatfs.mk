SYSTEM_INCLUDES += $(LIBRARY_DIR)/third_party/fatfs/source/

LIBRARY_FATFS += $(LIBRARY_DIR)/third_party/fatfs/source/default_diskio.cpp
LIBRARY_FATFS += $(LIBRARY_DIR)/third_party/fatfs/source/ff.c
LIBRARY_FATFS += $(LIBRARY_DIR)/third_party/fatfs/source/ffsystem.c
LIBRARY_FATFS += $(LIBRARY_DIR)/third_party/fatfs/source/ffunicode.c

$(eval $(call BUILD_LIRBARY,libfatfs,LIBRARY_FATFS))
