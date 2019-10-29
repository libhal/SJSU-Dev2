# ============================
# Misc Flags
# ============================
# shell echo must be used to ensure that the \x1B makes it into the variable
# simply doing YELLOW=\x1B[33;1m works on Linux but the forward slash is omitted
# on mac.
YELLOW=$(shell echo "\x1B[33;1m")
RED=$(shell echo "\x1B[31;1m")
MAGENTA=$(shell echo "\x1B[35;1m")
BLUE=$(shell echo "\x1B[34;1m")
GREEN=$(shell echo "\x1B[32;1m")
WHITE=$(shell echo "\x1B[37;1m")
RESET=$(shell echo "\x1B[0m")
CURRENT_SETUP_VERSION=$(shell cat $(SJSU_DEV2_BASE)/setup_version.txt)
# ============================
# Modifying make Flags
# ============================
# The following list of targets that opt-out of output sync
ifneq ($(MAKECMDGOALS), \
       $(filter $(MAKECMDGOALS), \
        presubmit openocd debug debug-test flash jtag-flash platform-flash \
        platform-jtag-flash lint ))
MAKEFLAGS += --output-sync=target
endif
#
# Setting the number of threads
#
NPROCS := 1
OS := $(shell uname -s)

ifneq ($(PREVIOUS_SETUP_VERSION), $(CURRENT_SETUP_VERSION))
$(info $(shell printf '$(YELLOW)'))
$(info +---------------- Project Setup Not Up to Date -----------------+)
$(info | The setup file version in the location file and this project  |)
$(info | are not equal.                                                |)
$(info |                                                               |)
$(info \  PREVIOUS_SETUP_VERSION = $(PREVIOUS_SETUP_VERSION)            )
$(info \  CURRENT_SETUP_VERSION  = $(CURRENT_SETUP_VERSION)             )
$(info |                                                               |)
$(info | Please run ./setup again                                      |)
$(info +---------------------------------------------------------------+)
$(info $(shell printf '$(RESET)'))
$(warning )
endif

ifeq ($(OS), Linux)
NPROCS := $(shell grep -c ^processor /proc/cpuinfo)
endif

ifeq ($(OS), Darwin) # Assume Mac OS X
NPROCS := $(shell sysctl -n hw.ncpu | grep -o "[0-9]\+")
endif

ifneq ($(MAKECMDGOALS), presubmit)
MAKEFLAGS += --jobs=$(NPROCS)
endif

ifeq ($(MAKECMDGOALS), $(filter $(MAKECMDGOALS), debug jtag-flash))
ifneq ($(PLATFORM), linux)
ifndef DEBUG_DEVICE
$(info $(shell printf '$(RED)'))
$(info +--------------- Missing command line arguments ----------------+)
$(info | In order to run the targets 'debug' or 'jtag-flash' you need  |)
$(info | to supply the following comand line variables:                |)
$(info |                                                               |)
$(info | 1. DEBUG_DEVICE = the name of the debugging interface in the  |)
$(info |                   command line variable DEBUG_DEVICE          |)
$(info | 2. PLATFORM     = name of the platform you want to target     |)
$(info |                                                               |)
$(info |                                                               |)
$(info | Example usage:                                                |)
$(info |                                                               |)
$(info |    make debug DEBUG_DEVICE=stlink PLATFORM=lpc17xx            |)
$(info |                                                               |)
$(info +---------------------------------------------------------------+)
$(info $(shell printf '$(RESET)'))
$(error )
endif
endif
endif

ifneq ($(MAKECMDGOALS), presubmit)
endif


# ============================
# SJSU-Dev2 Toolchain Paths
# ============================
# Path to CLANG compiler
SJCLANG_PATH      = $(SJSU_DEV2_BASE)/tools/clang+llvm-*/
SJCLANG           = $(shell cd $(SJCLANG_PATH) ; pwd)
# Path to ARM GCC compiler
SJARMGCC_PATH     = $(SJSU_DEV2_BASE)/tools/gcc-arm-none-eabi-*/
SJARMGCC          = $(shell cd $(SJARMGCC_PATH) ; pwd)
# Path to Openocd compiler
OPENOCD_DIR       = $(SJSU_DEV2_BASE)/tools/openocd
# Path to Openocd compiler
GDBINIT_PATH      = $(SJSU_DEV2_BASE)/tools/gdb_dashboard/gdbinit
# Compiler and library settings:
SJLIBDIR          = $(SJSU_DEV2_BASE)/firmware/library

# =================================
# Updating the LD_LIBRARY_PATH
# =================================
# used to run executables using the clang libc++
# linked library which are creatd via the "test" target
LD_LIBRARY_PATH := $(LD_LIBRARY_PATH):$(SJCLANG)/lib/
# ============================
# User Flags
# ============================
# Allow setting a project name from the environment, default to firmware.
# Only affects the name of the generated binary.
# TODO(#82): Set this from the directory this makefile is stored in
PROJ     ?= firmware
# Allow user to change which platform they are building for
PLATFORM ?= lpc40xx
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
#         make test TEST_ARGS="-s [i2c,adc]"
#         make library-test TEST_ARGS="-s [i2c,adc]"
#
TEST_ARGS ?=
# ============================
# Compilation Tools
# ============================
# Cause compiler warnings to become errors.
# Used in presubmit checks to make sure that the codebase does not include
# warnings
WARNINGS_ARE_ERRORS ?=
# IMPORTANT: GCC must be accessible via the PATH environment variable
HOST_CC         = $(SJCLANG)/bin/clang
HOST_CPPC       = $(SJCLANG)/bin/clang++
HOST_OBJDUMP    = $(SJCLANG)/bin/llvm-objdump
HOST_SIZEC      = $(SJCLANG)/bin/llvm-size
HOST_OBJCOPY    = $(SJCLANG)/bin/llvm-objcopy
HOST_NM         = $(SJCLANG)/bin/llvm-nm
HOST_COV        = $(SJCLANG)/bin/llvm-cov
CLANG_TIDY      = $(SJCLANG)/bin/clang-tidy
HOST_SYMBOLIZER = $(SJCLANG)/bin/llvm-symbolizer
# Mux between using the firmware compiler executables or the host compiler
ifeq ($(MAKECMDGOALS), $(filter $(MAKECMDGOALS), test library-test))
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

ifeq ($(MAKECMDGOALS), application)
$(info $(shell printf '$(MAGENTA)Building application firmware...$(RESET)'))
endif

# "make application"'s build directory becomes "build/application"
ifeq ($(MAKECMDGOALS), $(filter $(MAKECMDGOALS), application flash jtag-flash \
      platform-flash platform-jtag-flash stacktrace-application \
			multi-debug debug))
BUILD_SUBDIRECTORY_NAME = application
else
BUILD_SUBDIRECTORY_NAME = $(MAKECMDGOALS)
endif

BUILD_DIR       = $(BUILD_DIRECTORY_NAME)/$(BUILD_SUBDIRECTORY_NAME)/$(PLATFORM)
OBJECT_DIR                  = $(BUILD_DIR)/compiled
DBC_DIR                     = $(BUILD_DIR)/can-dbc
COVERAGE_DIR                = $(BUILD_DIR)/coverage
LIBRARY_DIR                 = $(SJSU_DEV2_BASE)/library
PROJECTS_DIR                = $(SJSU_DEV2_BASE)/projects
DEMOS_DIR                   = $(SJSU_DEV2_BASE)/demos
STATIC_LIBRARY_DIR          = $(LIBRARY_DIR)/static_libraries
PLATFORM_STATIC_LIBRARY_DIR = $(STATIC_LIBRARY_DIR)/$(PLATFORM)
TOOLS_DIR                   = $(SJSU_DEV2_BASE)/tools
SOURCE_DIR                  = source
COMPILED_HEADERS_DIR        = $(BUILD_DIR)/headers # NOTE: Actually use this!
CURRENT_DIRECTORY	          = $(shell pwd)
COVERAGE_FILES              = $(shell find build -name "*.gcda")
# ===========================
# Gathering Source Files
# ===========================
DBC_BUILD = $(DBC_DIR)/generated_can.h
# Set of directories to be added to the direct include list
INCLUDES += $(CURRENT_DIRECTORY) $(SOURCE_DIR)
# System includes are files that we do not want to show diagnostic information
# about. This is typically used for third party code.
SYSTEM_INCLUDES ?=
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

# Precompiled LIBARY files to link into project
CORE_STATIC_LIBRARY = $(OBJECT_DIR)/libsjsudev2.a
LIBRARIES ?=

define BUILD_LIRBARY

LIBRARIES += $(PLATFORM_STATIC_LIBRARY_DIR)/$(1).a

$(1)_OBJECTS = $$(addprefix $(OBJECT_DIR)/, $$($(2):=.o))

.SECONDARY: $$($(1)_OBJECTS)

-include    $$($(1)_OBJECTS:.o=.d) # DEPENDENCIES

$(PLATFORM_STATIC_LIBRARY_DIR)/$(1).a: $$($(1)_OBJECTS)
	@mkdir -p "$(PLATFORM_STATIC_LIBRARY_DIR)"
	@rm -f "$@"
	@$$(DEVICE_AR) rcs "$$@" $$^
	@$$(DEVICE_RANLIB) "$$@"
	@echo -e '$(YELLOW)Library file ( A )$(RESET)  : $$@ '

endef
# Set of ALL compilable test files in the current library.
# This also includes any source AND test to be tested.
# MUST NOT contain source files that contain a "main()" implementation
TESTS ?=
# Set of compilable test files specified by the user.
# This also includes any source AND test to be tested.
# MUST NOT contain source files that contain a "main()" implementation
USER_TESTS ?=
# Set of common flags to build software
COMMON_FLAGS ?=
# List of folder or files that should be excluded from lint analysis
LINT_FILTER ?=
# Openocd configuration file
OPENOCD_CONFIG ?=
# Include a project specific makefile. Using -include to keep make form exiting
# if the project.mk file does not exist.
-include project.mk
# Include all source files within the SJSU-Dev2 library directory by running the
# the library/library.mk makefile.
# This is where a lot of the magic happens. This makefile will call more sub
# makefiles until all of the included library source files have been found.
include $(LIBRARY_DIR)/library.mk
# A bit of post processing on the source variables
ifeq ($(MAKECMDGOALS), library-test)
COMPILABLES = $(TESTS)
TEST_SOURCE_DIRECTORIES = --filter="$(LIBRARY_DIR)"
else ifeq ($(MAKECMDGOALS), test)
COMPILABLES = $(USER_TESTS)
TEST_SOURCE_DIRECTORIES = --filter="$(LIBRARY_DIR)" \
    $(addsuffix ", $(addprefix --filter=", $(USER_TESTS)))
else
COMPILABLES = $(SOURCES)
endif

INCLUDES         := $(addsuffix ", $(addprefix -I", $(INCLUDES)))
SYSTEM_INCLUDES  := $(addsuffix ", $(addprefix -idirafter", $(SYSTEM_INCLUDES)))
OBJECTS           = $(addprefix $(OBJECT_DIR)/, $(COMPILABLES:=.o))

# ===========================
# Compilation Flags
# ===========================
OPTIMIZE  = -O$(OPT) -fmessage-length=0 -ffunction-sections -fdata-sections \
            -fno-exceptions -fno-omit-frame-pointer
            # -fasynchronous-unwind-tables
CPPOPTIMIZE = -fno-rtti -fno-threadsafe-statics
DEBUG_FLAG  = -g
WARNINGS  = -Wall -Wextra -Wshadow -Wlogical-op -Wfloat-equal \
            -Wdouble-promotion -Wduplicated-cond -Wswitch \
            -Wnull-dereference -Wformat=2 \
            -Wundef -Wconversion -Wsuggest-final-types \
            -Wsuggest-final-methods $(WARNINGS_ARE_ERRORS)
CPPWARNINGS = -Wold-style-cast -Woverloaded-virtual -Wsuggest-override \
              -Wuseless-cast $(WARNINGS_ARE_ERRORS)
DEFINES   = -D ELF_FILE=\"$(EXECUTABLE)\" -D PLATFORM=$(PLATFORM)
DISABLED_WARNINGS = -Wno-main -Wno-variadic-macros
# Combine all of the flags together
COMMON_FLAGS += $(OPTIMIZE) $(DEBUG_FLAG) $(DEFINES) \
                $(DISABLED_WARNINGS) -fdiagnostics-color
# Add the last touch for object files
CFLAGS_COMMON = $(COMMON_FLAGS) $(INCLUDES) $(SYSTEM_INCLUDES) -MMD -MP -c

ifndef LINKFLAGS
LINKFLAGS = $(COMMON_FLAGS) -Wl,--gc-sections -Wl,-Map,"$(MAP)" \
            --specs=nano.specs --specs=rdimon.specs \
            -T $(LIBRARY_DIR)/L0_Platform/$(PLATFORM)/linker.ld
endif

# Enable a whole different set of exceptions, checks, coverage tools and more
# with the test target
ifeq ($(MAKECMDGOALS), $(filter $(MAKECMDGOALS), test library-test))
CPPFLAGS = -fprofile-arcs -fPIC -fexceptions -fno-inline -fno-builtin \
         -fprofile-instr-generate -fcoverage-mapping \
         -fno-elide-constructors -ftest-coverage -fno-omit-frame-pointer \
         -fsanitize=address -stdlib=libc++ \
         -fdiagnostics-color \
         -Wconversion -Wextra -Wall \
         -Wno-sign-conversion -Wno-format-nonliteral \
         -Winconsistent-missing-override -Wshadow -Wfloat-equal \
         -Wdouble-promotion -Wswitch -Wnull-dereference -Wformat=2 \
         -Wundef -Wold-style-cast -Woverloaded-virtual \
          $(WARNINGS_ARE_ERRORS) \
         -D HOST_TEST=1 -D TARGET=HostTest -D SJ2_BACKTRACE_DEPTH=1024 \
         -D CATCH_CONFIG_FAST_COMPILE \
         $(INCLUDES) $(SYSTEM_INCLUDES) $(DEFINES) $(DEBUG_FLAG) \
         $(DISABLED_WARNINGS) \
         -O0 -MMD -MP -c
CFLAGS = $(CPPFLAGS)
else
CFLAGS = $(CFLAGS_COMMON) -D TARGET=Application -DTRACE -DOS_USE_TRACE_SEMIHOSTING_STDOUT
CPPFLAGS = $(CFLAGS) $(CPPWARNINGS) $(CPPOPTIMIZE) $(WARNINGS)
endif

# ===========================
# Lint variables
# ===========================
# Files to ignore in the linting and tidy process
# FILE_EXCLUDES entry resembles:
#   grep -v -e path/to/excluded/file
#   grep -v -e path/to/excluded/directory/
FILE_EXCLUDES = grep -v $(addprefix -e ,$(LINT_FILTER))

# Find all files within the firmware directory to be evaluated
LINT_FILES  = $(shell find $(PROJECTS_DIR)/hello_world \
                      $(PROJECTS_DIR)/starter \
                      $(LIBRARY_DIR) $(DEMOS_DIR) \
                      -name "*.h"   -o \
                      -name "*.hpp" -o \
                      -name "*.c"   -o \
                      -name "*.cpp" |  \
                      $(FILE_EXCLUDES) \
                      2> /dev/null)

# TODO(kammce): Add these phony files back to make linting and tiding up
# remember which files have already been linted/tidied.
LINT_FILES_PHONY = $(LINT_FILES:=.lint)
TIDY_FILES_PHONY = $(addprefix $(OBJECT_DIR)/, $(LINT_FILES:=.tidy))
# ===========================
# Firmware final products
# ===========================
EXECUTABLE = $(BUILD_DIR)/$(PROJ).elf
BINARY     = $(EXECUTABLE:.elf=.bin)
HEX        = $(EXECUTABLE:.elf=.hex)
LIST       = $(EXECUTABLE:.elf=.lst)
SIZE       = $(EXECUTABLE:.elf=.siz)
MAP        = $(EXECUTABLE:.elf=.map)
TEST_EXEC  = $(BUILD_DIRECTORY_NAME)/tests.exe
# TODO(kammce): Add header file precompilation back later
# TEST_FRAMEWORK = $(LIBRARY_DIR)/L4_Testing/testing_frameworks.hpp.gch

# This line allows the make to rebuild if header file changes.
# This is feature and not a bug, otherwise updates to header files do not
# register as an update to the source code.
-include       $(OBJECTS:.o=.d) # DEPENDENCIES
# Tell make to delete built files it has created if an error occurs
.DELETE_ON_ERROR:
# Tell make that the default recipe is the default
.DEFAULT_GOAL := default
# Tell make that these recipes don't have a end product
.PHONY: default build cleaninstall telemetry monitor show-lists clean flash \
        telemetry presubmit openocd debug library-clean purge test \
        library-test $(SIZE)
print-%  : ; @echo $* = $($*)
# ====================================================================
# When the user types just "make" or "help" this should appear to them
# ====================================================================
default: help
help:
	@echo "List of available targets:"
	@echo
	@echo "General Commands:"
	@echo
	@echo "  application  - Builds firmware project as an application"
	@echo "  flash        - Installs firmware on to a device with the Hyperload"
	@echo "                 bootloader installed."
	@echo "  clean        - deletes build folder contents"
	@echo "  cleaninstall - cleans, builds, and installs application firmware on "
	@echo "                 device."
	@echo "  library-clean - cleans static libraries files"
	@echo "  purge        - remove local build files and static libraries "
	@echo "  telemetry    - Launch telemetry web interface on platform"
	@echo
	@echo "SJSU-Dev2 Developer Commands: "
	@echo
	@echo "  presubmit    - run presubmit checks script"
	@echo "  lint         - Check that source files abide by the SJSU-Dev2 coding"
	@echo "                 standard."
	@echo "  tidy         - Check that source file fit the SJSU-Dev2 naming "
	@echo "                 convention "
	@echo "  help         - Shows this menu"
	@echo
	@echo "SJSU-Dev2 Testing Commands: "
	@echo
	@echo "  test          - build all library tests"
	@echo "  library-test     - build all tests as defined in USER_TESTS"
	@echo
	@echo "Debugging Commands: "
	@echo
	@echo "  openocd      - run openocd with the sjtwo.cfg file"
	@echo "  debug        - run arm gdb with current projects .elf file"
	@echo "  show-lists   - Makefile debugging target that displays the contents"
	@echo "                 of make variables"

# ====================================================================
# Build firmware
# ====================================================================
application: build
build: $(LIST) $(HEX) $(BINARY) $(SIZE)
# ====================================================================
# Flash/Program microcontroller using In-system programming (ISP)
# ====================================================================
flash:
	@$(MAKE) --quiet application
	@printf \
	'$(MAGENTA)Programming chip via In-system programming (ISP)...$(RESET)\n'
	@$(MAKE) --quiet platform-flash
# ====================================================================
# Flash/Program microcontroller using a debug port like jtag or swd
# ====================================================================
jtag-flash:
	@$(MAKE) --quiet application
	@printf '$(MAGENTA)Programming chip via debug port...$(RESET)\n'
	@$(OPENOCD_DIR)/bin/openocd -s $(OPENOCD_DIR)/scripts/ \
	-c "source [find interface/$(DEBUG_DEVICE).cfg]" -f $(OPENOCD_CONFIG) \
	-c "program \"$(EXECUTABLE)\" reset exit 0x0"
# ====================================================================
# Clean working build directory by deleting the build folder
# ====================================================================
clean:
	@rm -fR $(BUILD_DIRECTORY_NAME)
	@printf '$(MAGENTA)Build directory deleted$(RESET)\n'
# ====================================================================
# Remove precompiled libraries
# ====================================================================
library-clean:
	@rm -f $(LIBRARIES) $(CORE_STATIC_LIBRARY)
	@printf '$(MAGENTA)Libraries cleared$(RESET)\n'
# ====================================================================
# Remove precompiled libraries and current build folder
# ====================================================================
purge: clean
	@rm -rf $(STATIC_LIBRARY_DIR)
	@printf '$(MAGENTA)Cleared All Libraries$(RESET)\n'
# ====================================================================
# Open Browser to Telemetry website
# ====================================================================
telemetry:
	google-chrome https://kammce.github.io/Telemetry
# ====================================================================
# Format Code
# ====================================================================
format-code:
	@$(SJCLANG)/bin/git-clang-format --binary="$(SJCLANG)/bin/clang-format" \
	  --force
# ====================================================================
# Build Test Executable, Run test and generate code coverage
# ====================================================================
# In reference to issue #374, we need to remove the old gcda files otherwise
# if the test has been recompiled between executions of run-test, the
# executable will complain that the coverage files are out of date or
# corrupted
library-test: test $(TEST_EXEC)
test: $(TEST_EXEC)
	@rm -f $(COVERAGE_FILES) 2> /dev/null
	@export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) && \
	 export ASAN_SYMBOLIZER_PATH=$(HOST_SYMBOLIZER) && \
	 ASAN_OPTIONS="symbolize=1 color=always" \
	 $(TEST_EXEC) $(TEST_ARGS) --use-colour="yes"
	@mkdir -p "$(COVERAGE_DIR)"
	@gcovr $(TEST_SOURCE_DIRECTORIES) \
		--object-directory="$(BUILD_DIRECTORY_NAME)/" \
		-e "$(LIBRARY_DIR)/newlib" \
		-e "$(LIBRARY_DIR)/third_party" \
		-e "$(LIBRARY_DIR)/L4_Testing" \
		--html --html-details --gcov-executable="$(HOST_COV) gcov" \
		-o $(COVERAGE_DIR)/coverage.html
# ====================================================================
# Source Code Linting
# ====================================================================
# Evaluate library files and check them for linting errors.
lint:
	@python3 $(TOOLS_DIR)/cpplint/cpplint.py $(LINT_FILES)
# Evaluate library files for proper code naming conventions
tidy: $(TIDY_FILES_PHONY)
	@printf '$(GREEN)Tidy Evaluation Complete. Everything clear!$(RESET)\n'
# Run presumbission tests
presubmit:
	@$(TOOLS_DIR)/presubmit.sh
# ====================================================================
# Microcontroller Debugging
# ====================================================================
stacktrace-application:
	@$(DEVICE_ADDR2LINE) -e $(EXECUTABLE) $(TRACES)
# Start gdb and connect to openocd jtag debugging session

debug:
	@$(info $(shell printf '$(MAGENTA)Starting firmware debug...$(RESET)\n'))
	@$(TOOLS_DIR)/launch_openocd_gdb.sh \
			$(DEVICE_GDB) \
			$(GDBINIT_PATH) \
			$(PLATFORM) \
			$(CURRENT_DIRECTORY)/$(EXECUTABLE) \
			$(OPENOCD_DIR) \
			$(DEBUG_DEVICE) \
			$(OPENOCD_CONFIG)

debug-test:
	export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) && gdb build/tests.exe
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
	@echo "=========== COVERAGE FILES =============="
	@echo $(COVERAGE_FILES)
	@echo "=========== LIBRARIES =============="
	@echo $(LIBRARIES)
	@echo "=========== LINT =============="
	@echo $(LINT_FILES)

# ====================================================================
# Recipes to Compile Source Code
# ====================================================================

$(HEX): $(EXECUTABLE)
	@$(OBJCOPY) -O ihex "$<" "$@"
	@echo -e '$(YELLOW)Generated Hex Image $(RESET)   : $@'

$(BINARY): $(EXECUTABLE)
	@$(OBJCOPY) -O binary "$<" "$@"
	@echo -e '$(YELLOW)Generated Binary Image $(RESET): $@'


$(SIZE): $(EXECUTABLE)
	@echo
	@echo -e '$(WHITE)   Memory region:     Used Size  Region Size  %age Used'
	@echo -ne '$(RESET)'
	@export GREP_COLOR='1;34' ; cat '$(SIZE)' | grep --color=always ".*: " || true
	@echo
	@echo -e '$(WHITE)Section Memory Usage$(RESET)'
	@$(SIZEC) --format=berkeley "$<"
	@echo

$(LIST): $(EXECUTABLE)
	@$(OBJDUMP) --disassemble --all-headers --source --demangle --wide "$<" > "$@"
	@echo -e '$(YELLOW)Disassembly Generated!$(RESET)  : $@'

$(CORE_STATIC_LIBRARY): $(LIBRARIES)
	@rm -f "$@"
	@$(DEVICE_AR) -rcT "$@" $^
	@$(DEVICE_RANLIB) "$@"
	@echo -e '$(YELLOW)Final Library file ( A ) $(RESET): $@'

$(EXECUTABLE): $(OBJECTS) $(CORE_STATIC_LIBRARY)
	@echo -e '$(YELLOW)Linking Executable$(RESET)    : $@'
	@mkdir -p "$(dir $@)"
	@$(CPPC) -Wl,--print-memory-usage $(LINKFLAGS) -o "$@" \
			$(OBJECTS) $(CORE_STATIC_LIBRARY) 1> "$(SIZE)"

$(OBJECT_DIR)/%.c.o: %.c
	@mkdir -p "$(dir $@)"
	@$(CC) $(CFLAGS) -std=gnu11 -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo -e '$(YELLOW)Built file ( C ) $(RESET): $<'

$(OBJECT_DIR)/%.o: %
	@mkdir -p "$(dir $@)"
	@$(CPPC) $(CPPFLAGS) -std=c++17 -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo -e '$(YELLOW)Built file (C++) $(RESET): $<'

$(DBC_BUILD):
	@mkdir -p "$(dir $@)"
	python3 "$(LIBRARY_DIR)/$(DBC_DIR)/dbc_parse.py" \
		-i "$(LIBRARY_DIR)/$(DBC_DIR)/243.dbc" -s $(ENTITY) > $(DBC_BUILD)

$(TEST_EXEC): $(OBJECTS)
	@echo -e '$(YELLOW)Linking Test Executable $(RESET) : $@'
	@mkdir -p "$(dir $@)"
	@$(CPPC) -fprofile-arcs -fPIC -fexceptions -fno-inline \
					 -fno-inline-small-functions -fno-default-inline \
					 -fkeep-inline-functions -fno-elide-constructors  \
					 -ftest-coverage -O0 -fsanitize=address \
					 -std=c++17 -stdlib=libc++ -lc++ -lc++abi \
					 -o $(TEST_EXEC) $(OBJECTS)
	@echo -e '$(GREEN)Test Executable Generated!$(RESET)'

$(OBJECT_DIR)/%.tidy: %
	@mkdir -p "$(dir $@)"
	@$(CLANG_TIDY) $(if $(or $(findstring .hpp,$<), $(findstring .cpp,$<)), \
		-extra-arg="-std=c++17") "$<"  -- \
		-D TARGET=HostTest -D HOST_TEST=1 \
		-isystem"$(SJCLANG)/include/c++/v1/" \
		-stdlib=libc++ $(INCLUDES) $(SYSTEM_INCLUDES) 2> $@
	@echo -e '$(GREEN)Evaluated file: $(RESET)$< '
