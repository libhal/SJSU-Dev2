# Allow settiing a project name from the environment, default to firmware.
# Only affects the name of the generated binary.
# TODO(#82): Set this from the directory this makefile is stored in
PROJ    ?= firmware
# Affects what DBC is generated for SJSUOne board
ENTITY  ?= DBG
# Optimization level
OPT=0
# Cause compiler warnings to become errors.
# Used in presubmit checks to make sure that the codebase does not include
# warnings
WARNINGS_ARE_ERRORS ?=
# IMPORTANT: Be sure to source env.sh to access these via the PATH variable.
DEVICE_CC      = arm-none-eabi-gcc
DEVICE_CPPC    = arm-none-eabi-g++
DEVICE_OBJDUMP = arm-none-eabi-objdump
DEVICE_SIZEC   = arm-none-eabi-size
DEVICE_OBJCOPY = arm-none-eabi-objcopy
DEVICE_NM      = arm-none-eabi-nm
# Set of tests you would like to run. Text must be surrounded by [] and be a set
# comma deliminated.
#
#   Example of running only i2c and adc tests with the -s flag to show
#   successful assertions:
#
#         make run-test TEST_ARGS="-s [i2c,adc]"
#
TEST_ARGS ?=
# IMPORTANT: GCC must be accessible via the PATH environment variable
HOST_CC        ?= gcc-7
HOST_CPPC      ?= g++-7
HOST_OBJDUMP   ?= objdump-7
HOST_SIZEC     ?= size-7
HOST_OBJCOPY   ?= objcopy-7
HOST_NM        ?= nm-7

ifeq ($(MAKECMDGOALS), test)
CC      = $(HOST_CC)
CPPC    = $(HOST_CPPC)
OBJDUMP = $(HOST_OBJDUMP)
SIZEC   = $(HOST_SIZEC)
OBJCOPY = $(HOST_OBJCOPY)
NM      = $(HOST_NM)
else
CC      = $(DEVICE_CC)
CPPC    = $(DEVICE_CPPC)
OBJDUMP = $(DEVICE_OBJDUMP)
SIZEC   = $(DEVICE_SIZEC)
OBJCOPY = $(DEVICE_OBJCOPY)
NM      = $(DEVICE_NM)
endif

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
CLANG_TIDY   = $(SJCLANG)/clang-tidy
endif
ifeq ($(UNAME_S),Darwin)
CLANG_TIDY   = /usr/local/opt/llvm@6/bin/clang-tidy
endif

# Internal build directories
BUILD_DIR = build
TEST_DIR  = $(BUILD_DIR)/test

ifeq ($(MAKECMDGOALS), bootloader)
BIN_DIR   = $(BUILD_DIR)/bootloader
else
BIN_DIR   = $(BUILD_DIR)/application
endif

ifeq ($(MAKECMDGOALS), test)
OBJ_DIR   = $(TEST_DIR)/compiled
else
OBJ_DIR   = $(BIN_DIR)/compiled
endif

DBC_DIR   = $(BUILD_DIR)/can-dbc
COVERAGE  = $(BUILD_DIR)/coverage
LIB_DIR   = $(SJLIBDIR)
FIRMWARE  = $(SJBASE)/firmware
TOOLS     = $(SJBASE)/tools
COMPILED_HEADERS  = $(BUILD_DIR)/headers
CURRENT_DIRECTORY	= $(shell pwd)
# Source files folder
SOURCE    = source
define n


endef

ifndef SJDEV
$(error $n$n=============================================$nSJSU-Dev2 environment variables not set.$nPLEASE run "source env.sh"$n=============================================$n$n)
endif

#########
# FLAGS #
#########
CORTEX_M4F = -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
			       -fabi-version=0 -finstrument-functions \
			       -finstrument-functions-exclude-file-list=L0_LowLevel
OPTIMIZE  = -O$(OPT) -fmessage-length=0 -ffunction-sections -fdata-sections \
            -fno-exceptions -fomit-frame-pointer
CPPOPTIMIZE = -fno-rtti
DEBUG     = -g
WARNINGS  = -Wall -Wextra -Wshadow -Wlogical-op -Wfloat-equal \
            -Wdouble-promotion -Wduplicated-cond -Wlogical-op -Wswitch \
            -Wnull-dereference -Wformat=2 \
            -Wundef -Wconversion -Wsuggest-final-types \
            -Wsuggest-final-methods $(WARNINGS_ARE_ERRORS)
CPPWARNINGS = -Wold-style-cast -Woverloaded-virtual -Wsuggest-override \
              -Wuseless-cast $(WARNINGS_ARE_ERRORS)
DEFINES   = -DARM_MATH_CM4=1 -D__FPU_PRESENT=1U
DISABLED_WARNINGS = -Wno-main -Wno-variadic-macros
INCLUDES  = -I"$(CURRENT_DIRECTORY)/" \
			-I"$(LIB_DIR)/" \
			-isystem"$(LIB_DIR)/L0_LowLevel/SystemFiles" \
			-isystem"$(LIB_DIR)/third_party/" \
			-isystem"$(LIB_DIR)/third_party/printf" \
			-isystem"$(LIB_DIR)/third_party/FreeRTOS/Source" \
			-isystem"$(LIB_DIR)/third_party/FreeRTOS/Source/trace" \
			-isystem"$(LIB_DIR)/third_party/FreeRTOS/Source/include" \
			-isystem"$(LIB_DIR)/third_party/FreeRTOS/Source/portable" \
			-isystem"$(LIB_DIR)/third_party/FreeRTOS/Source/portable/GCC/ARM_CM4F"
COMMON_FLAGS = $(CORTEX_M4F) $(OPTIMIZE) $(DEBUG) $(WARNINGS)  $(DEFINES) \
               $(DISABLED_WARNINGS)

CFLAGS_COMMON = $(COMMON_FLAGS) $(INCLUDES) -MMD -MP -c

ifeq ($(MAKECMDGOALS), test)
CFLAGS = -fprofile-arcs -fPIC -fexceptions -fno-inline \
         -fno-inline-small-functions -fno-default-inline -fno-builtin \
         -ftest-coverage --coverage \
				 -Wno-unused -fno-elide-constructors -D HOST_TEST=1 \
         $(filter-out $(CORTEX_M4F) $(OPTIMIZE), $(CFLAGS_COMMON)) \
         -O0 -g
CPPFLAGS = $(CFLAGS)
else
CFLAGS = $(CFLAGS_COMMON)
CPPFLAGS = $(CFLAGS) $(CPPWARNINGS) $(CPPOPTIMIZE)
endif

ifeq ($(MAKECMDGOALS), bootloader)
LINKER = $(LIB_DIR)/LPC4078_bootloader.ld
CFLAGS += -D BOOTLOADER=1
else
LINKER = $(LIB_DIR)/LPC4078_application.ld
CFLAGS += -D APPLICATION=1
endif

LINKFLAGS = $(COMMON_FLAGS) \
    -T $(LINKER) \
    -Wl,--gc-sections \
		-Wl,-Map,"$(MAP)" \
    -specs=nano.specs
##############
# Test files #
##############
FILE_EXCLUDES = grep -v  \
				-e "$(LIB_DIR)/third_party/" \
				-e "$(LIB_DIR)/L0_LowLevel/SystemFiles" \
				-e "$(LIB_DIR)/L0_LowLevel/LPC40xx.h" \
				-e "$(LIB_DIR)/L0_LowLevel/FreeRTOSConfig.h"
# Find all files that end with "_test.cpp"
SOURCE_TESTS  = $(shell find $(SOURCE) \
                         -name "*_test.cpp" \
                         2> /dev/null)
# Find all library that end with "_test.cpp"
LIBRARY_TESTS = $(shell find "$(LIB_DIR)" -name "*_test.cpp" | \
						    $(FILE_EXCLUDES))
TESTS = $(SOURCE_TESTS) $(LIBRARY_TESTS)
OMIT_LIBRARIES = $(shell find "$(LIB_DIR)" \
                         -name "startup.cpp" -o \
                         -name "*.cpp" \
                         -path "$(LIB_DIR)/third_party/*" -o \
						             -path "$(LIB_DIR)/third_party/*")
OMIT_SOURCES   = $(shell find $(SOURCE) -name "main.cpp")
OMIT = $(OMIT_LIBRARIES) $(OMIT_SOURCES)
################
# Source files #
################
DBC_BUILD     = $(DBC_DIR)/generated_can.h
LIBRARY_FILES = $(shell find "$(LIB_DIR)" \
                         -name "*.c" -o \
                         -name "*.cpp")
# Remove all test files from LIBRARY_FILES
LIBRARIES     = $(filter-out $(LIBRARY_TESTS), $(LIBRARY_FILES))
SOURCE_FILES  = $(shell find $(SOURCE) \
                         -name "*.c" -o \
                         -name "*.s" -o \
                         -name "*.S" -o \
                         -name "*.cpp" \
                         2> /dev/null)
SOURCE_HEADERS  = $(shell find $(SOURCE) \
                         -name "*.h" -o \
                         -name "*.hpp" \
                         2> /dev/null)
PRINTF_3P_LIBRARY = $(shell find "$(LIB_DIR)/third_party/printf" \
                         -name "*.cpp" 2> /dev/null)
##############
# Lint files #
##############
LINT_FILES      = $(shell find $(FIRMWARE) \
                         -name "*.h"   -o \
                         -name "*.hpp" -o \
                         -name "*.c"   -o \
                         -name "*.cpp" | \
						             $(FILE_EXCLUDES) \
                         2> /dev/null)
# Remove all test files from SOURCE_FILES
SOURCES     = $(filter-out $(SOURCE_TESTS), $(SOURCE_FILES))
ifeq ($(MAKECMDGOALS), test)
COMPILABLES = $(filter-out $(OMIT), $(LIBRARIES) $(SOURCES) $(TESTS)) \
              $(PRINTF_3P_LIBRARY)
else
COMPILABLES = $(LIBRARIES) $(SOURCES)
endif
###############
# Ouput Files #
###############
# $(patsubst %.cpp,%.o, LIST)    : Replace .cpp -> .o
# $(patsubst %.c,%.o, LIST)      : Replace .c -> .o
# $(patsubst src/%,%, LIST)      : Replace src/path/file.o -> path/file.o
# $(addprefix $(OBJ_DIR)/, LIST) : Add OBJ DIR to path
#                                  (path/file.o -> obj/path/file.o)
# NOTE: the extra / for obj_dir is necessary to fully quaify the path from root.
OBJECT_FILES = $(addprefix $(OBJ_DIR)/, \
                    $(patsubst %.S,%.o, \
                        $(patsubst %.s,%.o, \
                            $(patsubst %.c,%.o, \
                                $(patsubst %.cpp,%.o, \
                                    $(COMPILABLES) \
                                ) \
                            ) \
                        ) \
                    ) \
                )
EXECUTABLE = $(BIN_DIR)/$(PROJ).elf
BINARY     = $(EXECUTABLE:.elf=.bin)
HEX        = $(EXECUTABLE:.elf=.hex)
LIST       = $(EXECUTABLE:.elf=.lst)
SIZE       = $(EXECUTABLE:.elf=.siz)
MAP        = $(EXECUTABLE:.elf=.map)
TEST_EXEC  = $(TEST_DIR)/tests.exe
TEST_FRAMEWORK = $(LIB_DIR)/L5_Testing/testing_frameworks.hpp.gch

# This line allows the make to rebuild if header file changes.
# This is feature and not a bug, otherwise updates to header files do not
# register as an update to the source code.
DEPENDENCIES = $(OBJECT_FILES:.o=.d)
-include       $(DEPENDENCIES)
# Tell make to delete built files it has created if an error occurs
.DELETE_ON_ERROR:
# Tell make that the default recipe is the default
.DEFAULT_GOAL := default
# Tell make that these recipes don't have a end product
.PHONY: build cleaninstall telemetry monitor show-lists clean flash telemetry \
        presubmit openocd debug multi-debug default
print-%  : ; @echo $* = $($*)

# When the user types just "make" this should appear to them
help:
	@echo "List of available targets:"
	@echo
	@echo "    build        - builds firmware project"
	@echo "    bootloader   - builds firmware using bootloader linker"
	@echo "    help         - shows this menu"
	@echo "    flash        - builds and installs firmware on to SJOne board"
	@echo "    telemetry    - will launch telemetry interface"
	@echo "    clean        - cleans project folder"
	@echo "    cleaninstall - cleans, builds, and installs firmware"
	@echo "    show-lists   - Shows all object files that will be compiled"
	@echo "    presubmit    - run presubmit checks script"
	@echo "    openocd      - run openocd with the sjtwo.cfg file"
	@echo "    debug        - run arm gdb with current projects .elf file"
	@echo "    multi-debug  - run multiarch gdb with current projects .elf file"
	@echo

default: help
    # Just shows the help menu
bootloader: build
# Build recipe
application: build
build: $(DBC_DIR) $(OBJ_DIR) $(BIN_DIR) $(SIZE) $(LIST) $(HEX) $(BINARY)
# Complete rebuild and flash installation#
cleaninstall: clean build flash
# Debug recipe to show internal list contents
show-lists:
	@echo "=========== OBJECT FILES ============"
	@echo $(OBJECT_FILES)
	@echo "===========  TEST FILES  ============"
	@echo $(TESTS)
	@echo "===========  LIBRARIES   ============"
	@echo $(LIBRARIES)
	@echo "===========   SOURCES   ============"
	@echo $(SOURCES)
	@echo "=========== COMPILABLES ============"
	@echo $(COMPILABLES)
	@echo "=========== OMIT ============"
	@echo $(OMIT)
	@echo "===========   FLAGS   =============="
	@echo $(CFLAGS)
	@echo "=========== TEST FLAGS =============="
	@echo $(TEST_CFLAGS)
	@echo "=========== CLANG TIDY BIN PATH =============="
	@echo $(CLANG_TIDY)
	@echo "=========== OMIT_LIBRARIES =============="
	@echo $(OMIT_LIBRARIES)

$(HEX): $(EXECUTABLE)
	@echo ' '
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	@$(OBJCOPY) -O ihex "$<" "$@"
	@echo 'Finished building: $@'
	@echo ' '

$(BINARY): $(EXECUTABLE)
	@echo ' '
	@echo 'Invoking: Cross ARM GNU Create Flash Binary Image'
	@$(OBJCOPY) -O binary "$<" "$@"
	@echo 'Finished building: $@'
	@echo ' '

$(SIZE): $(EXECUTABLE)
	@echo ' '
	@echo 'Invoking: Cross ARM GNU Print Size'
	@$(SIZEC) --format=berkeley "$<"
	@echo 'Finished building: $@'
	@echo ' '

#--line-numbers --disassemble --source
$(LIST): $(EXECUTABLE)
	@echo ' '
	@echo 'Invoking: Cross ARM GNU Create Assembly Listing'
	@$(OBJDUMP) --disassemble --all-headers --source --demangle --wide "$<" > "$@"
	@echo 'Finished building: $@'
	@echo ' '

$(EXECUTABLE): $(OBJECT_FILES)
	@echo 'Invoking: Cross ARM C++ Linker'
	@mkdir -p "$(dir $@)"
	@$(CPPC) $(LINKFLAGS) -o "$@" $(OBJECT_FILES)
	@echo 'Finished building target: $@'

$(OBJ_DIR)/%.o: %.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	@mkdir -p "$(dir $@)"
	@$(CPPC) $(CPPFLAGS) -std=c++17 -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

$(OBJ_DIR)/%.o: %.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	@mkdir -p "$(dir $@)"
	@$(CC) $(CFLAGS) -std=gnu11 -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

$(OBJ_DIR)/%.o: %.s
	@echo 'Building Assembly file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	@mkdir -p "$(dir $@)"
	@$(CC) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

$(OBJ_DIR)/%.o: %.S
	@echo 'Building Assembly file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	@mkdir -p "$(dir $@)"
	@$(CC) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

$(OBJ_DIR)/%.o: $(LIB_DIR)/%.cpp
	@echo 'Building C++ file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	@mkdir -p "$(dir $@)"
	@$(CPPC) $(CPPFLAGS) -std=c++17 -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

$(OBJ_DIR)/%.o: $(LIB_DIR)/%.c
	@echo 'Building C file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	@mkdir -p "$(dir $@)"
	@$(CC) $(CFLAGS) -std=gnu11 -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

$(DBC_BUILD):
	python2.7 "$(LIB_DIR)/$(DBC_DIR)/dbc_parse.py" -i "$(LIB_DIR)/$(DBC_DIR)/243.dbc" -s $(ENTITY) > $(DBC_BUILD)

$(DBC_DIR):
	mkdir -p $(DBC_DIR)

$(OBJ_DIR):
	@echo 'Creating Objects Folder: $<'
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	@echo 'Creating Binary Folder: $<'
	mkdir -p $(BIN_DIR)

clean:
	rm -fR $(BUILD_DIR)

flash: build
	@bash -c "\
	source $(TOOLS)/Hyperload/modules/bin/activate && \
	python $(TOOLS)/Hyperload/hyperload.py -b 576000 -c 48000000 -a clocks -d $(SJDEV) $(HEX)"

telemetry:
	@bash -c "\
	source $(TOOLS)/Telemetry/modules/bin/activate && \
	python2.7 $(TOOLS)/Telemetry/telemetry.py"

test: $(COVERAGE) $(TEST_EXEC)

run-test:
	@valgrind --leak-check=full --track-origins=yes -v $(TEST_EXEC) $(TEST_ARGS)
	@gcovr --root $(FIRMWARE) --keep --object-directory $(BUILD_DIR) \
		-e "$(LIB_DIR)/newlib" \
		-e "$(LIB_DIR)/third_party" \
		--html --html-details -o $(COVERAGE)/coverage.html

$(COVERAGE):
	mkdir -p $(COVERAGE)

$(TEST_EXEC): $(TEST_FRAMEWORK) $(OBJECT_FILES)
	@mkdir -p "$(dir $@)"
	@echo 'Finished building target: $@'
	@$(CPPC) -fprofile-arcs -fPIC -fexceptions -fno-inline \
         -fno-inline-small-functions -fno-default-inline \
         -ftest-coverage --coverage \
         -fno-elide-constructors -lgcov \
         -fprofile-arcs -ftest-coverage -fPIC -O0 \
         -o $(TEST_EXEC) $(OBJECT_FILES)

%.hpp.gch: %.hpp
	@echo 'Precompiling HPP file: $<'
	@echo 'Invoking: C Compiler'
	@mkdir -p "$(dir $@)"
	@$(CPPC) $(CFLAGS) -std=c++17 -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" \
        $(LIB_DIR)/L5_Testing/testing_frameworks.hpp
	@echo 'Finished building: $<'
	@echo ' '

lint:
	@python2.7 $(TOOLS)/cpplint/cpplint.py $(LINT_FILES)

tidy:
	@$(CLANG_TIDY) -extra-arg=-std=c++17 $(LINT_FILES) -- -std=c++17 \
	$(INCLUDES) -D CLANG_TIDY=1 -D HOST_TEST=1

presubmit:
	@$(TOOLS)/presubmit.sh

openocd:
	openocd -f $(TOOLS)/OpenOCD/sjtwo.cfg

debug:
	arm-none-eabi-gdb -ex "target remote :3333" $(EXECUTABLE)

multi-debug:
	gdb-multiarch -ex "target remote :3333" $(EXECUTABLE)
