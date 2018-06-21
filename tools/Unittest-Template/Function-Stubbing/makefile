# Allow settiing a project name from the environment, default to firmware.
# Only affects the name of the generated binary.
# TODO: Set this from the directory this makefile is stored in
PROJ 			?= firmware
# Affects what DBC is generated for SJSUOne board
ENTITY 			?= DBG

# IMPORTANT: Must be accessible via the PATH variable!!!
CC            = gcc
CPPC            = g++

# Internal build directories
OBJ_DIR			= obj
BIN_DIR			= bin
DBC_DIR			= _can_dbc
LIB_DIR 		= $(SJLIBDIR)

define n


endef

ifndef SJLIBDIR
$(error $n$n=============================================$nSJSUOne environment variables not set.$nPLEASE run "source env.sh"$n=============================================$n$n)
endif

# FLAGS
CORTEX_M4F	= # nothing because this is being compiled on the host machine
OPTIMIZE 	= -O0
DEBUG 		= -g
WARNINGS 	= -Wall -Wextra -Wshadow -Wlogical-op -Wfloat-equal \
			  -Wdouble-promotion -Wlogical-op -Wswitch \
			  -Wuseless-cast -Wformat=2 \
			  -Wundef -Wconversion -Wsign-conversion -Woverloaded-virtual \
			  -Wsuggest-attribute=const -Wsuggest-final-types -Wsuggest-final-methods \
			  -Wsuggest-override
			  #-Walloc-zero -Walloc-size-larger-than=8kB -Walloca-larger-than=1

DISABLED_WARNINGS = -Wno-main
DEFINES     = -DARM_MATH_CM4=1 -D__FPU_PRESENT=1U
# end FLAGS

COMMON_FLAGS = $(CORTEX_M4F) $(OPTIMIZE) $(DEBUG) $(WARNINGS) $(DISABLED_WARNINGS) $(DEFINES)

CFLAGS = $(COMMON_FLAGS) \
    -fabi-version=0 \
    -I"$(LIB_DIR)/" \
    -I"$(LIB_DIR)/newlib" \
    -I"$(LIB_DIR)/third-party/fat" \
    -I"$(LIB_DIR)/third-party/FreeRTOS" \
    -I"$(LIB_DIR)/third-party/FreeRTOS/trace" \
    -I"$(LIB_DIR)/third-party/FreeRTOS/include" \
    -I"$(LIB_DIR)/third-party/FreeRTOS/portable" \
    -I"$(LIB_DIR)/third-party/FreeRTOS/portable/no_mpu" \
    -I"$(DBC_DIR)" \
    -I"../../source/"

CURRENT_DIRECTORY	= $(shell pwd)
TESTFILES 			= $(shell cat "test-files.list")
COMPILABLES		 	= $(addprefix $(LIB_DIR)/, $(TESTFILES)) test.cpp
TEST_EXEC 			= ./test-suite


.PHONY: test build build-cgreen cgreen clean debug

COVERAGE_RESULTS=results

test: $(COVERAGE_RESULTS) $(TEST_EXEC)
	@valgrind --leak-check=full --track-origins=yes -v ./$(TEST_EXEC) -s
	@gcovr -f "($(LIB_DIR)/|$(CURRENT_DIRECTORY))" \
	-e "($(LIB_DIR)/newlib|$(LIB_DIR)/third-party)" \
<<<<<<< HEAD
	--delete --html --html-details -o $(COVERAGE_RESULTS)/code-coverage.html
	mv *.gcno $(COVERAGE_RESULTS) 2> /dev/null
=======
	--html --html-details -o $(COVERAGE_RESULTS)/code-coverage.html
	mv *.gcda *.gcno $(COVERAGE_RESULTS) 2> /dev/null
>>>>>>> 5693f3cb6feb4ffae724835de6f1f9f7356a804e

$(COVERAGE_RESULTS):
	mkdir -p $(COVERAGE_RESULTS)

# $(TEST_EXEC):
$(TEST_EXEC): clean
	@echo " \\──────────────────────────────/"
	@echo "  \\ Generating test executable /"
	@$(CPPC) $(CFLAGS) -std=c++17 -fprofile-arcs -ftest-coverage -fPIC -O0 -fexceptions -o $(TEST_EXEC) $(COMPILABLES)
	@echo "   \\──────────────────────────/"
	@echo "    \\       Finished         /"
	@sleep .25
	@echo "     \\──────────────────────/"
	@echo "      \\    Running Test    /"
	@echo "       \\──────────────────/"
	@sleep .25

debug:
	@echo $(TESTFILES)
	@echo "=================="
	@echo $(COMPILABLES)
	@echo "=================="
	@echo $(TEST_EXEC)
	@echo "=================="

clean:
	@rm -f $(TEST_EXEC)
