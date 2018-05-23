# Allow settiing a project name from the environment, default to firmware.
# Only affects the name of the generated binary.
# TODO: Set this from the directory this makefile is stored in
PROJ 			?= firmware
# Points where the SJSUOne libraries sources are located
SJDEV_LIB_DIR 	?= lib
# Affects what DBC is generated for SJSUOne board
ENTITY 			?= DBG

# IMPORTANT: Must be accessible via the PATH variable!!!
CPPC            = g++

# Internal build directories
OBJ_DIR			= obj
BIN_DIR			= bin
DBC_DIR			= _can_dbc

CFLAGS = -Wall -Wshadow -Wlogical-op -Wfloat-equal \
    -fabi-version=0 -fno-exceptions \
    -std=gnu++11 \
    -I"$(LIB_DIR)/" \
    -I"$(LIB_DIR)/newlib" \
    -I"$(LIB_DIR)/L0_LowLevel" \
    -I"$(LIB_DIR)/L1_FreeRTOS" \
    -I"$(LIB_DIR)/L1_FreeRTOS/include" \
    -I"$(LIB_DIR)/L1_FreeRTOS/portable" \
    -I"$(LIB_DIR)/L1_FreeRTOS/portable/no_mpu" \
    -I"$(LIB_DIR)/L2_Drivers" \
    -I"$(LIB_DIR)/L2_Drivers/base" \
    -I"$(LIB_DIR)/L3_Utils" \
    -I"$(LIB_DIR)/L3_Utils/tlm" \
    -I"$(LIB_DIR)/L4_IO" \
    -I"$(LIB_DIR)/L4_IO/fat" \
    -I"$(LIB_DIR)/L4_IO/wireless" \
    -I"$(LIB_DIR)/L5_Application" \
    -I"$(LIB_DIR)/L6_Testing" \
    -I"L2_Drivers" \
    -I"L3_Utils" \
    -I"L4_IO" \
    -I"../../L5_Application/" \
    -I"$(DBC_DIR)"

TESTFILES 			= $(shell cat "test-files.list")
COMPILABLES		 	= $(addprefix ../../, $(TESTFILES)) test.cpp
CGREEN_COMPILABLES	= $(addprefix ../../, $(TESTFILES)) cgreen-test.cpp
TEST_EXEC 			= ./test-suite
CGREEN_TEST_EXEC	= ./cgreen-test-suite

.PHONY: test build build-cgreen cgreen clean debug

test: $(TEST_EXEC)
	@$(TEST_EXEC) -s

cgreen: $(CGREEN_TEST_EXEC)
	@$(CGREEN_TEST_EXEC) -s

$(TEST_EXEC): clean
	@echo " \\───────────────────────────────────────/"
	@echo "  \\      Generating test executable     /"
	@$(CPPC) $(CFLAGS) -fexceptions -o $(TEST_EXEC) $(COMPILABLES)
	@echo "   \\───────────────────────────────────/"
	@echo "    \\            Finished             /"
	@sleep .25
	@echo "     \\───────────────────────────────/"
	@echo "      \\        Running Test         /"
	@echo "       \\───────────────────────────/"
	@sleep .25

$(CGREEN_TEST_EXEC): clean
	@echo -n 'Generating CGREEN test executable '
	@$(CPPC) $(CFLAGS) -fexceptions -o $(CGREEN_TEST_EXEC) $(CGREEN_COMPILABLES) -lcgreen
	@echo '--> Finished'

debug:
	@echo $(TESTFILES)
	@echo "=================="
	@echo $(COMPILABLES)
	@echo "=================="
	@echo $(TEST_EXEC)
	@echo "=================="
	@echo $(CGREEN_TEST_EXEC)
	@echo "=================="

clean:
	@rm -f $(TEST_EXEC) $(CGREEN_TEST_EXEC)
