# ============================
# Misc Flags
# ============================
# shell echo must be used to ensure that the \x1B makes it into the variable
# simply doing YELLOW=\x1B[33;1m works on Linux but the forward slash is omitted
# on mac.
YELLOW=$(shell echo "\x1B[33;1m")
RED=$(shell echo "\x1B[31;1m")
RESET=$(shell echo "\x1B[0m")
GREEN=$(shell echo "\x1B[32;1m")
# ============================
# Environment Flags
# ============================
include env.mk
# ============================
# Modifying make Flags
# ============================
# The following list of target opt-out of output sync
ifneq ($(MAKECMDGOALS), \
       $(filter $(MAKECMDGOALS), \
			 presubmit run-test openocd debug lint multi-debug flash burn))
MAKEFLAGS += --output-sync
endif
#
# Setting the number of threads
#
NPROCS := 1
OS := $(shell uname -s)

ifeq ($(OS), Linux)
NPROCS := $(shell grep -c ^processor /proc/cpuinfo)
endif

ifeq ($(OS), Darwin) # Assume Mac OS X
NPROCS := $(shell sysctl -n hw.ncpu | grep -o "[0-9]\+")
endif

ifneq ($(MAKECMDGOALS), presubmit)
MAKEFLAGS += --jobs=$(NPROCS)
endif

#
# Updating the LD_LIBRARY_PATH used to run executables using the clang libc++
# linked library which are creatd via the "test" target
#
LD_LIBRARY_PATH := $(LD_LIBRARY_PATH):$(SJCLANG)/../lib/
# ============================
# User Flags
# ============================
# Allow settiing a project name from the environment, default to firmware.
# Only affects the name of the generated binary.
# TODO(#82): Set this from the directory this makefile is stored in
PROJ    ?= firmware
# Specifies which entity in the DBC your application will represent.
# The example below will generate a DBC where this particular application is
# the NAVIGATION entity on the CANBUS.
#
#			make application ENTITY="NAVIGATION"
#
ENTITY  ?= DBG
# Used by "make flash" to specify a direct path to a serial port connected to a
# device running the Hyperload bootloader
#
#     make flash DEVICE="/dev/ttyUSB0"
#
DEVICE ?=
# Set the optimization level of the compiler. Default is optimization level 0.
# Available optimization levels for GCC are:
#     0: Low to no optimization. Only trivial and quick optimizations will be
#        considered
#     1: Level 1 optimization. Optimizes the executable, compilation time will
#        increase.
#     2: Level 2 optimization. Optimizes the executable further. Performs all
#        optimizations that do not sacrafice memory to increase runtime
#        performance.
#     3: Highest level of optimization. Typically increases binary size
#        significatly.
#     s: Optimize for size. Will perform all optimizations that reduce the size
#        of the binary.
OPT    ?= 0
# Set of tests you would like to run. Text must be surrounded by [] and be a set
# comma deliminated.
#
#   Example of running only i2c and adc tests with the -s flag to show
#   successful assertions:
#
#         make run-test TEST_ARGS="-s [i2c,adc]"
#
TEST_ARGS ?=
# ============================
# Compilation Flags
# ============================
# IMPORTANT: Be sure to source env.sh to access these via the PATH variable.
DEVICE_CC      = arm-none-eabi-gcc
DEVICE_CPPC    = arm-none-eabi-g++
DEVICE_OBJDUMP = arm-none-eabi-objdump
DEVICE_SIZEC   = arm-none-eabi-size
DEVICE_OBJCOPY = arm-none-eabi-objcopy
DEVICE_NM      = arm-none-eabi-nm
# Cause compiler warnings to become errors.
# Used in presubmit checks to make sure that the codebase does not include
# warnings
WARNINGS_ARE_ERRORS ?=
# IMPORTANT: GCC must be accessible via the PATH environment variable
HOST_CC        = clang
HOST_CPPC      = clang++
HOST_OBJDUMP   = llvm-objdump
HOST_SIZEC     = llvm-size
HOST_OBJCOPY   = llvm-objcopy
HOST_NM        = llvm-nm
# Mux between using the firmware compiler executables or the host compiler
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
# =============================================
# Directory Flags and Build Folder Organization
# =============================================
# Name of the folder where all of the object and intermediate compilable files
# will be stored
BUILD_DIRECTORY_NAME = build
# "make application"'s build directory becomes "build/application"
# "make test"'s build directory becomes "build/test"
ifeq ($(MAKECMDGOALS), flash)
BUILD_SUBDIRECTORY_NAME = application
else ifeq ($(MAKECMDGOALS), burn)
BUILD_SUBDIRECTORY_NAME = bootloader
else
BUILD_SUBDIRECTORY_NAME = $(MAKECMDGOALS)
endif

BUILD_DIR     = $(BUILD_DIRECTORY_NAME)/$(BUILD_SUBDIRECTORY_NAME)
OBJECT_DIR    = $(BUILD_DIR)/compiled
DBC_DIR       = $(BUILD_DIR)/can-dbc
COVERAGE_DIR  = $(BUILD_DIR)/coverage
FIRMWARE_DIR  = $(SJBASE)/firmware
LIB_DIR       = $(FIRMWARE_DIR)/library
TOOLS_DIR     = $(SJBASE)/tools
SOURCE_DIR    = source
COMPILED_HEADERS_DIR  = $(BUILD_DIR)/headers # NOTE: Actually use this!
CURRENT_DIRECTORY	= $(shell pwd)
# ===========================
# Gathering Source Files
# ===========================
DBC_BUILD = $(DBC_DIR)/generated_can.h
# Set of directories to be added to the direct include list
INCLUDES = $(CURRENT_DIRECTORY) $(SOURCE_DIR)
# System includes are files that we do not want to show diagnostic information
# about. This is typically used for third party code.
SYSTEM_INCLUDES =
# Set of all source files to compile (.c, .cpp)
# By default, all source files included within the source directory of a
# project are included. The search below is recursive, so subdirectories within
# the source directory are supported.
SOURCES = $(shell find source -name "*.c" -o -name "*.cpp" -print 2> /dev/null)
# Report an error if the SOURCES variable is empty
ifeq ($(SOURCES),)
$(info $(shell printf '$(RED)'))
$(info +------------ Project Source Files Not Found Error -------------+)
$(info | Couldn't find any source files within the "source" directory! |)
$(info |                                                               |)
$(info | This is the case if you are not in a project folder, or your  |)
$(info | project folder lacks a "sources" folder.                      |)
$(info |                                                               |)
$(info | Either move into a project directory or add a source folder   |)
$(info | with source files in it.                                      |)
$(info +---------------------------------------------------------------+)
$(info $(shell printf '$(RESET)'))
$(error )
endif
# Set of all compilable test files.
# This also includes any source AND test to be tested.
# MUST NOT contain source files that contain a "main()" implementation
# TODO(#296): Add a seperation between library and user tests
TESTS ?=
# Include a project specific makefile. Using -include to keep make form exiting
# if the project.mk file does not exist.
-include project.mk
# Include all source files within the SJSU-Dev2 library directory by running the
# the library/library.mk makefile.
# This is where a lot of the magic happens. This makefile will call more sub
# makefiles until all of the included library source files have been found.
include $(FIRMWARE_DIR)/library/library.mk
# A bit of post processing on the source variables
INCLUDES        := $(addsuffix ", $(addprefix -I", $(INCLUDES)))
SYSTEM_INCLUDES := $(addsuffix ", $(addprefix -idirafter", $(SYSTEM_INCLUDES)))
OBJECTS          = $(addprefix $(OBJECT_DIR)/, $(SOURCES:=.o))
TEST_OBJECTS     = $(addprefix $(OBJECT_DIR)/, $(TESTS:=.o))

ifeq ($(MAKECMDGOALS), test)
COMPILABLES = $(TESTS)
else
COMPILABLES = $(SOURCES)
endif
# ===========================
# Compilation Flags
# ===========================
CORTEX_M4F = -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
			       -fabi-version=0 \
			       -finstrument-functions-exclude-file-list=L0_LowLevel
OPTIMIZE  = -O$(OPT) -fmessage-length=0 -ffunction-sections -fdata-sections \
            -fno-exceptions -fomit-frame-pointer
CPPOPTIMIZE = -fno-rtti
DEBUG     = -g
WARNINGS  = -Wall -Wextra -Wshadow -Wlogical-op -Wfloat-equal \
            -Wdouble-promotion -Wduplicated-cond -Wswitch \
            -Wnull-dereference -Wformat=2 \
            -Wundef -Wconversion -Wsuggest-final-types \
            -Wsuggest-final-methods $(WARNINGS_ARE_ERRORS)
CPPWARNINGS = -Wold-style-cast -Woverloaded-virtual -Wsuggest-override \
              -Wuseless-cast $(WARNINGS_ARE_ERRORS)
DEFINES   = -DARM_MATH_CM4=1 -D__FPU_PRESENT=1U
DISABLED_WARNINGS = -Wno-main -Wno-variadic-macros
# Combine all of the flags together
COMMON_FLAGS = $(CORTEX_M4F) $(OPTIMIZE) $(DEBUG) $(WARNINGS) $(DEFINES) \
               $(DISABLED_WARNINGS) -fdiagnostics-color
# Add the last touch for object files
CFLAGS_COMMON = $(COMMON_FLAGS) $(INCLUDES) $(SYSTEM_INCLUDES) -MMD -MP -c
LINKFLAGS = $(COMMON_FLAGS) -T $(LINKER) -specs=nano.specs \
						-Wl,--gc-sections -Wl,-Map,"$(MAP)" \

# Enable specific flags for building a bootloader
ifeq ($(MAKECMDGOALS), bootloader)
LINKER = $(LIB_DIR)/LPC4078_bootloader.ld
CFLAGS_COMMON += -D BOOTLOADER=1
endif
# NOTE: DO NOT LINK -finstrument-functions into test build when using clang and
# clang std libs (libc++) or it will result in a metric ton of undefined linker
# errors.
# The filte command checks if any of the make targets are application, flash,
# etc and if so, this ifeq will become true
ifeq ($(MAKECMDGOALS), $(filter \
			$(MAKECMDGOALS), application flash build cleaninstall))
LINKER = $(LIB_DIR)/LPC4078_application.ld
CFLAGS_COMMON += -D APPLICATION=1
CFLAGS_COMMON += -finstrument-functions \
	-finstrument-functions-exclude-file-list=third_party/FreeRTOS/Source
endif

# Enable a whole different set of exceptions, checks, coverage tools and more
# with the test target
ifeq ($(MAKECMDGOALS), test)
CPPFLAGS = -fprofile-arcs -fPIC -fexceptions -fno-inline -fno-builtin \
				 -fprofile-instr-generate -fcoverage-mapping \
         -fno-elide-constructors -ftest-coverage -fno-omit-frame-pointer \
				 -fsanitize=address -stdlib=libc++ \
				 -Wconversion -Wextra -Wall \
				 -Wno-sign-conversion -Wno-format-nonliteral \
				 -Winconsistent-missing-override -Wshadow -Wfloat-equal \
         -Wdouble-promotion -Wswitch -Wnull-dereference -Wformat=2 \
         -Wundef -Wold-style-cast -Woverloaded-virtual \
				 $(WARNINGS_ARE_ERRORS) \
				 -D HOST_TEST=1 -D SJ2_BACKTRACE_DEPTH=1024 \
				 -D CATCH_CONFIG_FAST_COMPILE \
				 $(INCLUDES) $(SYSTEM_INCLUDES) $(DEFINES) $(DEBUG) \
				 $(DISABLED_WARNINGS) \
				 -O0 -MMD -MP -c
CFLAGS = $(CPPFLAGS)
else
CFLAGS = $(CFLAGS_COMMON)
CPPFLAGS = $(CFLAGS) $(CPPWARNINGS) $(CPPOPTIMIZE)
endif

# ===========================
# Lint variables
# ===========================
# Files to ignore in the linting and tidy process
FILE_EXCLUDES = grep -v  \
				-e "$(LIB_DIR)/third_party/" \
				-e "$(LIB_DIR)/L0_LowLevel/SystemFiles" \
				-e "$(LIB_DIR)/L0_LowLevel/LPC40xx.h" \
				-e "$(LIB_DIR)/L0_LowLevel/FreeRTOSConfig.h"
# Find all files within the firmware directory to be evaluated
LINT_FILES  = $(shell find $(FIRMWARE_DIR) \
                      -name "*.h"   -o \
                      -name "*.hpp" -o \
                      -name "*.c"   -o \
                      -name "*.cpp" | \
					            $(FILE_EXCLUDES) \
                      2> /dev/null)
# TODO(kammce): Add these phony files back to make linting and tiding up
# remember which files have already been linted/tidied.
LINT_FILES_PHONY = $(LINT_FILES:=.lint)
TIDY_FILES_PHONY = $(LINT_FILES:=.tidy)
# ===========================
# Firmware final products
# ===========================
EXECUTABLE = $(BUILD_DIR)/$(PROJ).elf
BINARY     = $(EXECUTABLE:.elf=.bin)
HEX        = $(EXECUTABLE:.elf=.hex)
LIST       = $(EXECUTABLE:.elf=.lst)
SIZE       = $(EXECUTABLE:.elf=.siz)
MAP        = $(EXECUTABLE:.elf=.map)
TEST_EXEC  = $(BUILD_DIRECTORY_NAME)/test/tests.exe
# TODO(kammce): Add header file precompilation back later
# TEST_FRAMEWORK = $(LIB_DIR)/L4_Testing/testing_frameworks.hpp.gch

# This line allows the make to rebuild if header file changes.
# This is feature and not a bug, otherwise updates to header files do not
# register as an update to the source code.
-include       $(OBJECTS:.o=.d) # DEPENDENCIES
# Tell make to delete built files it has created if an error occurs
.DELETE_ON_ERROR:
# Tell make that the default recipe is the default
.DEFAULT_GOAL := default
# Tell make that these recipes don't have a end product
.PHONY: build cleaninstall telemetry monitor show-lists clean flash telemetry \
        presubmit openocd debug multi-debug default
print-%  : ; @echo $* = $($*)
# ====================================================================
# When the user types just "make" or "help" this should appear to them
# ====================================================================
default: help
help:
	@echo "List of available targets:"
	@echo "General Commands:"
	@echo "  application  - Builds firmware project as an application"
	@echo "  flash        - Installs firmware on to a device with the Hyperload"
	@echo "                 bootloader installed."
	@echo "  bootloader   - Builds firmware using bootloader linker"
	@echo "  burn         - Installs bootloader onto device [NOT OPERATIONAL]"
	@echo "                 (LPC40xx & LPC17xx)"
	@echo "  clean        - deletes build folder contents"
	@echo "  cleaninstall - cleans, builds, and installs application firmware on "
	@echo "                 device."
	@echo "  telemetry    - Launch telemetry web interface on platform"
	@echo "  show-lists   - Makefile debugging target that displays the contents"
	@echo "                 of make variables"
	@echo "SJSU-Dev2 Developer Commands: "
	@echo "  presubmit    - run presubmit checks script"
	@echo "  lint         - Check that source files abide by the SJSU-Dev2 coding"
	@echo "                 standard."
	@echo "  tidy         - Check that source file fit the SJSU-Dev2 naming "
	@echo "                 convention "
	@echo "  help         - Shows this menu"
	@echo "Debugging Commands: "
	@echo "  openocd      - run openocd with the sjtwo.cfg file"
	@echo "  debug        - run arm gdb with current projects .elf file"
	@echo "  multi-debug  - run multiarch gdb with current projects .elf file"
	@echo
	@echo
# ====================================================================
# Build firmware
# ====================================================================
bootloader: build
application: build
build: $(LIST) $(HEX) $(BINARY) $(SIZE)
# ====================================================================
# Flash board
# ====================================================================
flash:
	@bash -c "\
	source $(TOOLS_DIR)/Hyperload/modules/bin/activate && \
	python $(TOOLS_DIR)/Hyperload/hyperload.py \
	--baud=576000 --animation=clocks --clockspeed=48000000 \
	--device=\"$(SJDEV)\" \"$(BINARY)\""
# ====================================================================
# Clean working build directory by deleting the build folder
# ====================================================================
clean:
	rm -fR $(BUILD_DIRECTORY_NAME)
# ====================================================================
# Run telemetry
# ====================================================================
telemetry:
	@bash -c "\
	source $(TOOLS_DIR)/Telemetry/modules/bin/activate && \
	python2.7 $(TOOLS_DIR)/Telemetry/telemetry.py"
# ====================================================================
# Build Test Executable
# ====================================================================
test: $(TEST_EXEC)
# ====================================================================
# Run Test Executable
# ====================================================================
run-test:
	@export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) && \
	  $(TEST_EXEC) $(TEST_ARGS) --use-colour="yes"
	@mkdir -p "$(COVERAGE_DIR)"
	@gcovr --root="$(FIRMWARE_DIR)/" --keep --object-directory="$(BUILD_DIR)/" \
		-e "$(LIB_DIR)/newlib" \
		-e "$(LIB_DIR)/third_party" \
		-e "$(LIB_DIR)/L4_Testing" \
		--html --html-details --gcov-executable="llvm-cov gcov" \
		-o $(COVERAGE_DIR)/coverage.html
# ====================================================================
# Source Code Linting
# ====================================================================
# Evaluate library files and check them for linting errors.
lint:
	@python2.7 $(TOOLS_DIR)/cpplint/cpplint.py $(LINT_FILES)
# Evaluate library files for proper code naming conventions
tidy: $(TIDY_FILES_PHONY)
	@printf '$(GREEN)Tidy Evaluation Complete. Everything clear!$(RESET)\n'
# Run presumbission tests
presubmit:
	@$(TOOLS_DIR)/presubmit.sh
# ====================================================================
# Microcontroller Debugging
# ====================================================================
# Start an openocd jtag debug session for the sjtwo development board
openocd:
	openocd -f $(FIRMWARE_DIR)/debug/sjtwo.cfg
# Start gdb for arm and connect to openocd jtag debugging session
debug:
	arm-none-eabi-gdb -ex "target remote :3333" $(EXECUTABLE)
# Start gdb just like the debug target, but using gdb-multiarch
# gdb-multiarch is perferable since it supports python in its .gdbinit file
multi-debug:
	gdb-multiarch -ex "target remote :3333" $(EXECUTABLE)
# ====================================================================
# Makefile debug
# ====================================================================
show-lists:
	@echo "===========  TEST FILES  ============"
	@echo $(TESTS)
	@echo "=========== OBJECT FILES ============"
	@echo $(TEST_OBJECTS)
	@echo "===========   SOURCES   ============"
	@echo $(SOURCES)
	@echo "=========== OBJECT FILES ============"
	@echo $(OBJECTS)
	@echo "=========== INCLUDES FILES ============"
	@echo $(INCLUDES)
	@echo "=========== SYSTEM INCLUDES FILES ============"
	@echo $(SYSTEM_INCLUDES)
	@echo "===========   FLAGS   =============="
	@echo $(CFLAGS)
	@echo "=========== TEST FLAGS =============="
	@echo $(TEST_CFLAGS)

# ====================================================================
# Recipes to Compile Source Code
# ====================================================================

$(HEX): $(EXECUTABLE)
	@printf '$(YELLOW)Generating Hex Image $(RESET)   : $@ '
	@$(OBJCOPY) -O ihex "$<" "$@"
	@printf '$(GREEN)Hex Generated!$(RESET)\n'

$(BINARY): $(EXECUTABLE)
	@printf '$(YELLOW)Generating Binary Image $(RESET): $@ '
	@$(OBJCOPY) -O binary "$<" "$@"
	@printf '$(GREEN)Binary Generated!$(RESET)\n'

$(SIZE): $(EXECUTABLE)
	@echo ' '
	@echo 'Showing Image Size Information: '
	@$(SIZEC) --format=berkeley "$<"
	@echo ' '

$(LIST): $(EXECUTABLE)
	@printf '$(YELLOW)Generating Disassembly$(RESET)  : $@ '
	@$(OBJDUMP) --disassemble --all-headers --source --demangle --wide "$<" > "$@"
	@printf '$(GREEN)Disassembly Generated!$(RESET)\n'

$(EXECUTABLE): $(OBJECTS)
	@printf '$(YELLOW)Linking Executable $(RESET)     : $@ '
	@mkdir -p "$(dir $@)"
	@$(CPPC) $(LINKFLAGS) -o "$@" $(OBJECTS)
	@printf '$(GREEN)Executable Generated!$(RESET)\n'

$(OBJECT_DIR)/%.c.o: %.c
	@printf '$(YELLOW)Building file ( C ) $(RESET): $< '
	@mkdir -p "$(dir $@)"
	@$(CC) $(CFLAGS) -std=gnu11 -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@printf '$(GREEN)DONE!$(RESET)\n'

$(OBJECT_DIR)/%.o: %
	@printf '$(YELLOW)Building file (C++) $(RESET): $< '
	@mkdir -p "$(dir $@)"
	@$(CPPC) $(CPPFLAGS) -std=c++17 -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@printf '$(GREEN)DONE!$(RESET)\n'

$(DBC_BUILD):
	@mkdir -p "$(dir $@)"
	python2.7 "$(LIB_DIR)/$(DBC_DIR)/dbc_parse.py" -i "$(LIB_DIR)/$(DBC_DIR)/243.dbc" -s $(ENTITY) > $(DBC_BUILD)

$(TEST_EXEC): $(TEST_OBJECTS)
	@printf '$(YELLOW)Linking Test Executable $(RESET) : $@ '
	@mkdir -p "$(dir $@)"
	@$(CPPC) -fprofile-arcs -fPIC -fexceptions -fno-inline \
					 -fno-inline-small-functions -fno-default-inline \
					 -fkeep-inline-functions -fno-elide-constructors  \
					 -ftest-coverage -O0 -fsanitize=address \
					 -std=c++17 -stdlib=libc++ -lc++ -lc++abi \
					 -o $(TEST_EXEC) $(TEST_OBJECTS)
	@printf '$(GREEN)Test Executable Generated!$(RESET)\n'

%.tidy: %
	@printf '$(YELLOW)Evaluating file: $(RESET)$< '
	@clang-tidy $(if $(or $(findstring .hpp,$<), $(findstring .cpp,$<)), \
	  -extra-arg="-std=c++17") "$<"  -- \
		-D CLANG_TIDY=1 -D HOST_TEST=1 \
		-isystem"$(SJCLANG)/../include/c++/v1/" \
		-stdlib=libc++ $(INCLUDES) $(SYSTEM_INCLUDES) 2> /dev/null
	@printf '$(GREEN)DONE!$(RESET)\n'
