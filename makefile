# ==============================================================================
#
#
# Command line Arguments
#
#
# ==============================================================================
# NOTE: See "make help" for details about each argument
PLATFORM            ?= lpc40xx
OPTIMIZE            ?= $(or $(OPT), 0)
DEVICE              ?=
JTAG                ?= $(or $(DEBUG_DEVICE),)
TEST_ARGS           ?=
LINK_FLAGS          ?=
USER_TESTS          ?=
COMMON_FLAGS        ?=
LINT_FILTER         ?=
OPENOCD_CONFIG      ?=
TESTS               ?=
NO_TEST_NEEDED      ?=
WARNINGS_ARE_ERRORS ?=
# ==============================================================================
#
#
# Utility Variables
#
#
# ==============================================================================
# Variables containing the ANSI Color codes
# ==============================================================================
# Defines a newline
define newline


endef
# NOTE: shell echo must be used to ensure that the \x1B makes it into the
# variable simply doing YELLOW=\x1B[33;1m works on Linux but the forward slash
# is omitted on mac.
YELLOW=$(shell echo "\x1B[33;1m")
RED=$(shell echo "\x1B[31;1m")
MAGENTA=$(shell echo "\x1B[35;1m")
BLUE=$(shell echo "\x1B[34;1m")
GREEN=$(shell echo "\x1B[32;1m")
WHITE=$(shell echo "\x1B[37;1m")
RESET=$(shell echo "\x1B[0m")
# ==============================================================================
#
#
# Make Option Configuration
#
#
# ==============================================================================
# Report an error if SJSU_DEV2_BASE does not exist
# ==============================================================================
ifeq ($(SJSU_DEV2_BASE),)
$(info $(shell printf '$(RED)'))
$(info +------ Invalid Project Folder Not Found Error ------+)
$(info | Run make from within a SJSU-Dev2 project folder    |)
$(info +----------------------------------------------------+)
$(info $(shell printf '$(RESET)'))
$(error )
endif
# ==============================================================================
# Tell make to delete built files it has created if an error occurs
# ==============================================================================
.DELETE_ON_ERROR:
.DEFAULT_GOAL := help
print-%  : ; @echo $* = $($*)
# ==============================================================================
# The following list of targets that opt-out of output sync
# ==============================================================================
ifeq ($(MAKECMDGOALS), $(filter $(MAKECMDGOALS), test application library-test))
MAKEFLAGS += --output-sync=target
endif
#
# presubmit openocd debug debug-test flash jtag-flash platform-flash \
# platform-jtag-flash lint program
# ==============================================================================
# Present setup out of date message
# ==============================================================================
SETUP_VERSION_PATH    = $(SJSU_DEV2_BASE)/setup_version.txt
CURRENT_SETUP_VERSION = $(shell cat $(SETUP_VERSION_PATH) 2> /dev/null)

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

# ==============================================================================
# Setting the number of threads
# ==============================================================================
DO_NOT_MULTITHREAD = presubmit all-projects execute program
ifneq ($(MAKECMDGOALS), $(filter $(MAKECMDGOALS), $(DO_NOT_MULTITHREAD)))
  NPROCS := 1
  OS := $(shell uname -s)

  ifeq ($(OS), Linux)
    NPROCS := $(shell grep -c ^processor /proc/cpuinfo)
  endif

  ifeq ($(OS), Darwin) # Assume Mac OS X
    NPROCS := $(shell sysctl -n hw.ncpu | grep -o "[0-9]\+")
  endif

  MAKEFLAGS += --jobs=$(NPROCS)
endif
# ==============================================================================
# Detect Missing make Arguments
# ==============================================================================
ifeq ($(MAKECMDGOALS), $(filter $(MAKECMDGOALS), debug program jtag-flash))
ifneq ($(PLATFORM), linux)
ifndef JTAG
  $(info $(shell printf '$(RED)'))
  $(info +----------------- Missing command line arguments -------------------+)
  $(info | In order to run the targets 'debug' or 'program' you need to       |)
  $(info | supply the following command line variables:                       |)
  $(info |                                                                    |)
  $(info | 1. JTAG     = the name of the debugging interface in the command   |)
  $(info |               line variable JTAG                                   |)
  $(info | 2. PLATFORM = name of the platform you want to target              |)
  $(info |                                                                    |)
  $(info |                                                                    |)
  $(info | Example usage:                                                     |)
  $(info |                                                                    |)
  $(info |    make debug JTAG=stlink PLATFORM=lpc17xx                         |)
  $(info |                                                                    |)
  $(info +--------------------------------------------------------------------+)
  $(info $(shell printf '$(RESET)'))
  $(error )
endif
endif
endif
# ==============================================================================
# Transform platform -> "host" for testing targets
# ==============================================================================
ifeq ($(MAKECMDGOALS), $(filter $(MAKECMDGOALS), test library-test))
PLATFORM := host
endif
# ==============================================================================
#
#
# SJSU-Dev2 tool chains and parameters
#
#
# ==============================================================================
# Toolpaths
# ==============================================================================
SJCLANG_PATH   = $(SJSU_DEV2_BASE)/tools/clang+llvm-*/
SJCLANG        = $(shell cd $(SJCLANG_PATH) ; pwd)
SJARMGCC_PATH  = $(SJSU_DEV2_BASE)/tools/gcc-arm-none-eabi-*/
SJARMGCC       = $(shell cd $(SJARMGCC_PATH) ; pwd)
OPENOCD_DIR    = $(shell grep -q Microsoft /proc/version && \
                   echo "$(SJSU_DEV2_BASE)/tools/openocd-wsl" || \
                   echo "$(SJSU_DEV2_BASE)/tools/openocd")
OPENOCD_EXE    = $(shell grep -q Microsoft /proc/version && \
                   echo "openocd.exe" || echo "openocd")
GDBINIT_PATH   = $(SJSU_DEV2_BASE)/tools/gdb_dashboard/gdbinit
# ==============================================================================
# Tool Definitions
# ==============================================================================
CODE_COVERAGE_TOOL = gcov-8
CLANG_TIDY         = $(SJCLANG)/bin/clang-tidy
HOST_SYMBOLIZER    = $(SJCLANG)/bin/llvm-symbolizer
# ==============================================================================
# Compilation Flags
# ==============================================================================
C_OPTIMIZE    = -O$(OPTIMIZE) -fmessage-length=0 -ffunction-sections \
                -fdata-sections -fno-exceptions -fno-omit-frame-pointer
CPP_OPTIMIZE  = -fno-rtti -fno-threadsafe-statics
DEBUG_FLAG    = -g
WARNINGS      = -Wall -Wextra -Wshadow -Wfloat-equal -Wundef \
                -Wno-format-nonliteral -Wconversion -Wdouble-promotion \
                -Wswitch -Wnull-dereference -Wformat=2
CPP_WARNINGS  = -Wold-style-cast -Woverloaded-virtual -Wsuggest-override \
                -Wsuggest-final-types -Wsuggest-final-methods
DEFINES       = -D ELF_FILE=\"$(EXECUTABLE)\"
DISABLED_WARNINGS = -Wno-main -Wno-variadic-macros
# ==============================================================================
# Combining Compilation Flags
# ==============================================================================
COMMON_FLAGS  += $(C_OPTIMIZE) $(DEBUG_FLAG) $(DEFINES) $(DISABLED_WARNINGS) \
                 $(WARNINGS_ARE_ERRORS) -fdiagnostics-color
C_FLAGS_COMMON = $(COMMON_FLAGS) $(INCLUDES) $(SYSTEM_INCLUDES) -MMD -MP -c
# ==============================================================================
#
#
# Directory Flags and Build Folder Organization
#
#
# ==============================================================================
# Fixed names for source directories
BUILD_DIRECTORY_NAME        = build
SOURCE_DIR                  = source
CURRENT_DIRECTORY           = $(shell pwd)
LIBRARY_DIR                 = $(SJSU_DEV2_BASE)/library
PROJECTS_DIR                = $(SJSU_DEV2_BASE)/projects
DEMOS_DIR                   = $(SJSU_DEV2_BASE)/demos
TOOLS_DIR                   = $(SJSU_DEV2_BASE)/tools
COVERAGE_DIR                = $(BUILD_DIR)/coverage
# Build file locations
BUILD_DIR                   = $(BUILD_DIRECTORY_NAME)/$(PLATFORM)
OBJECT_DIR                  = $(BUILD_DIR)/compiled
STATIC_LIBRARY_DIR          = $(LIBRARY_DIR)/static_libraries
PLATFORM_STATIC_LIBRARY_DIR = $(STATIC_LIBRARY_DIR)/$(PLATFORM)
COVERAGE_FILES              = $(shell find build -name "*.gcda")
# ==============================================================================
#
#
# Gathering Source Files
#
#
# ==============================================================================
# Pre-compiled LIBRARY files to link into project
CORE_STATIC_LIBRARY = $(OBJECT_DIR)/libsjsudev2.a
LIBRARIES          ?=
SYSTEM_INCLUDES    ?=
INCLUDES           += $(CURRENT_DIRECTORY) $(SOURCE_DIR)
# Start by gather source files within the source/ directory
SOURCES = $(shell find source -name "*.c" -o -name "*.cpp" -print 2> /dev/null)
# ==============================================================================
# Macro for building static library files
# ==============================================================================
define BUILD_LIBRARY

LIBRARIES += $(PLATFORM_STATIC_LIBRARY_DIR)/$(1).a

$(1)_OBJECTS = $$(addprefix $(OBJECT_DIR)/, $$($(2):=.o))

.SECONDARY: $$($(1)_OBJECTS)

-include    $$($(1)_OBJECTS:.o=.d) # DEPENDENCIES

$(PLATFORM_STATIC_LIBRARY_DIR)/$(1).a: $$($(1)_OBJECTS)
	@mkdir -p "$(PLATFORM_STATIC_LIBRARY_DIR)"
	@rm -f "$@"
	@$$(DEVICE_AR) rcs "$$@" $$^
	@$$(DEVICE_RANLIB) "$$@"
	@printf '$(YELLOW)Library file ( A )$(RESET)  : $$@ \n'

endef
#===============================================================================
# Include a project specific makefile.
#
# Using -include to keep make form exiting if the project.mk file does not
# exist.
#===============================================================================
-include project.mk
ifeq ($(MAKECMDGOALS), $(filter $(MAKECMDGOALS), test library-test))
PLATFORM := host
endif
#===============================================================================
# Include all source files within the SJSU-Dev2 library directory
#
# This is where a lot of the magic happens. This makefile will call more sub
# makefiles until all of the included library source files have been found.
#===============================================================================
include $(LIBRARY_DIR)/library.mk
# ==============================================================================
# A bit of post processing on the source variables
# ==============================================================================
ifeq ($(MAKECMDGOALS), library-test)
  CC          := gcc-8
  CPPC        := g++-8
  OBJDUMP     := objdump
  SIZEC       := size
  OBJCOPY     := objcopy
  NM          := nm
  COMPILABLES := $(TESTS)
  TEST_SOURCE_DIRECTORIES = --filter="$(LIBRARY_DIR)"
else ifeq ($(MAKECMDGOALS), test)
  CC          := gcc-8
  CPPC        := g++-8
  OBJDUMP     := objdump
  SIZEC       := size
  OBJCOPY     := objcopy
  NM          := nm
  COMPILABLES := $(USER_TESTS)
  TEST_SOURCE_DIRECTORIES = --filter="$(LIBRARY_DIR)" \
      $(addsuffix ", $(addprefix --filter=", $(USER_TESTS)))
else
  CC          := $(DEVICE_CC)
  CPPC        := $(DEVICE_CPPC)
  OBJDUMP     := $(DEVICE_OBJDUMP)
  SIZEC       := $(DEVICE_SIZEC)
  OBJCOPY     := $(DEVICE_OBJCOPY)
  NM          := $(DEVICE_NM)
  COMPILABLES := $(SOURCES)
endif
# ==============================================================================
# Final Flag Compositions
# ==============================================================================
INCLUDES         := $(addsuffix ", $(addprefix -I", $(INCLUDES)))
SYSTEM_INCLUDES  := $(addsuffix ", $(addprefix -idirafter", $(SYSTEM_INCLUDES)))
OBJECTS          := $(addprefix $(OBJECT_DIR)/, $(COMPILABLES:=.o))

ifeq ($(MAKECMDGOALS), $(filter $(MAKECMDGOALS), test library-test))
CPP_FLAGS := -fprofile-arcs -fPIC -fexceptions -fno-inline -fno-builtin \
             -fprofile-generate -ftest-coverage -fbranch-probabilities \
             -fsanitize=address -fdiagnostics-color -fprofile-correction \
             $(WARNINGS) $(WARNINGS_ARE_ERRORS) \
             -Wsuggest-override -Wno-sign-conversion \
             -Wno-sign-compare -Wno-conversion -Wno-format-nonliteral \
             -Wno-missing-field-initializers \
             -D SJ2_BACKTRACE_DEPTH=1024 -D CATCH_CONFIG_FAST_COMPILE \
             -D HOST_TEST=1 -D PLATFORM=$(PLATFORM) \
             $(INCLUDES) $(SYSTEM_INCLUDES) $(DEFINES) $(DEBUG_FLAG) \
             $(DISABLED_WARNINGS) \
             -O0 -MMD -MP -c
C_FLAGS   := $(CPP_FLAGS)
else
C_FLAGS   := $(C_FLAGS_COMMON) -D PLATFORM=$(PLATFORM) \
             -DTRACE -DOS_USE_TRACE_SEMIHOSTING_STDOUT
CPP_FLAGS := $(C_FLAGS) $(CPP_WARNINGS) $(CPP_OPTIMIZE) $(WARNINGS)
endif
# ==============================================================================
# Final products
# ==============================================================================
EXECUTABLE     = $(BUILD_DIR)/firmware.elf
BINARY         = $(EXECUTABLE:.elf=.bin)
HEX            = $(EXECUTABLE:.elf=.hex)
LIST           = $(EXECUTABLE:.elf=.lst)
SIZE           = $(EXECUTABLE:.elf=.siz)
MAP            = $(EXECUTABLE:.elf=.map)
TEST_EXEC      = $(BUILD_DIRECTORY_NAME)/tests.exe
TEST_FRAMEWORK = $(LIBRARY_DIR)/L4_Testing/testing_frameworks.hpp.gch
# ==============================================================================
# Defines Linker flags
# ==============================================================================
DEFAULT_LINK_FLAGS = $(COMMON_FLAGS) -Wl,--gc-sections -Wl,-Map,"$(MAP)" \
                   --specs=nano.specs --specs=rdimon.specs \
                   -T $(LIBRARY_DIR)/L0_Platform/$(PLATFORM)/linker.ld
LINK_FLAGS := $(or $(LINK_FLAGS), $(DEFAULT_LINK_FLAGS))
# ==============================================================================
#
#
# Recipes
#
#
# ==============================================================================
# Set the recipes without end products
# ==============================================================================
.PHONY: flash telemetry show-lists clean library-clean purge  \
        telemetry presubmit openocd debug clean-coverage-files test \
        library-test $(SIZE)
# ==============================================================================
# Rebuild source files if header file dependencies changes
# ==============================================================================
-include       $(OBJECTS:.o=.d)


help:
	@cat $(TOOLS_DIR)/makefile_help_menu.txt | \
	GREP_COLOR='1;31' grep --color=always -e " [-]*"  -e '**' | \
	GREP_COLOR='1;34' grep --color=always -e "==" -e '**'


application: $(LIST) $(HEX) $(BINARY) $(SIZE)


execute: flash
flash:
	+@$(MAKE) --quiet application
	@printf \
	'$(MAGENTA)Programming chip via In-System Programming (ISP)...$(RESET)\n'
	+@$(MAKE) --quiet platform-flash


debug:
	@$(info $(shell printf '$(MAGENTA)Starting firmware debug...$(RESET)\n'))
	@$(TOOLS_DIR)/launch_openocd_gdb.sh \
			$(DEVICE_GDB) \
			$(GDBINIT_PATH) \
			$(PLATFORM) \
			$(CURRENT_DIRECTORY)/$(EXECUTABLE) \
			$(OPENOCD_DIR) \
			$(JTAG) \
			$(OPENOCD_CONFIG) \
			$(OPENOCD_EXE)


debug-test:
	gdb build/tests.exe


jtag-flash: program
program:
	+@$(MAKE) --quiet application
	@printf '$(MAGENTA)Programming chip via debug device...$(RESET)\n'
	@$(OPENOCD_DIR)/bin/$(OPENOCD_EXE) -s $(OPENOCD_DIR)/scripts/ \
			-c "source [find interface/$(JTAG).cfg]" -f $(OPENOCD_CONFIG) \
			-c "program \"$(EXECUTABLE)\" reset exit"


library-test: test $(TEST_EXEC)
# NOTE: From issue #374, we found that we need to remove the old gcda files
# otherwise if the test has been recompiled between executions of run-test, the
# executable will complain that the coverage files are out of date or corrupted.
test: $(TEST_EXEC)
	@rm -f $(COVERAGE_FILES) 2> /dev/null
	@export ASAN_SYMBOLIZER_PATH=$(HOST_SYMBOLIZER) && \
	 ASAN_OPTIONS="symbolize=1 color=always" \
	 $(TEST_EXEC) $(TEST_ARGS) --use-colour="yes"
	@mkdir -p "$(COVERAGE_DIR)"
	@gcovr $(TEST_SOURCE_DIRECTORIES) \
		--object-directory="$(BUILD_DIRECTORY_NAME)/" \
		-e "$(LIBRARY_DIR)/newlib" \
		-e "$(LIBRARY_DIR)/third_party" \
		-e "$(LIBRARY_DIR)/L4_Testing" \
		--html --html-details --gcov-executable="$(CODE_COVERAGE_TOOL)" \
		-o $(COVERAGE_DIR)/coverage.html


clean:
	@rm -fR $(BUILD_DIRECTORY_NAME)
	@printf '$(MAGENTA)Build directory deleted$(RESET)\n'


library-clean:
	@rm -f $(LIBRARIES) $(CORE_STATIC_LIBRARY)
	@printf '$(MAGENTA)Libraries cleared$(RESET)\n'


purge: clean
	@rm -rf $(STATIC_LIBRARY_DIR)
	@printf '$(MAGENTA)Cleared All Libraries$(RESET)\n'


telemetry:
	google-chrome https://kammce.github.io/Telemetry


format-code:
	@$(SJCLANG)/bin/git-clang-format --binary="$(SJCLANG)/bin/clang-format" \
	  --force


FILE_EXCLUDES = grep -v $(addprefix -e ,$(LINT_FILTER))
LINT_FILES  = $(shell find $(PROJECTS_DIR)/hello_world \
                           $(PROJECTS_DIR)/starter \
                           $(PROJECTS_DIR)/barebones \
                           $(LIBRARY_DIR) \
                           $(DEMOS_DIR) \
                      -name "*.h"   -o \
                      -name "*.hpp" -o \
                      -name "*.c"   -o \
                      -name "*.cpp" |  \
                      $(FILE_EXCLUDES) \
                      2> /dev/null)
lint:
	@python3 $(TOOLS_DIR)/cpplint/cpplint.py $(LINT_FILES)


spellcheck:
	@$(TOOLS_DIR)/spell_checker.sh $(LINT_FILES)


all-projects:
	+@$(TOOLS_DIR)/build_all_projects.sh


FILES_WITH_TESTS=$(filter-out $(NO_TEST_NEEDED), $(LINT_FILES))
find-missing-tests:
	@$(TOOLS_DIR)/find_sources_without_tests.sh $(FILES_WITH_TESTS)


FIND_ALL_TEST_FILES = $(shell find $(LIBRARY_DIR) -name "*_test.cpp")
UNUSED_TEST_FILES = $(filter-out $(TESTS), $(FIND_ALL_TEST_FILES))
find-unused-tests:
	@$(TOOLS_DIR)/print_unused_test_files.sh $(UNUSED_TEST_FILES)


TIDY_FILES_PHONY = $(addprefix $(OBJECT_DIR)/, $(LINT_FILES:=.tidy))
tidy: $(TIDY_FILES_PHONY)
	@printf '$(GREEN)Tidy Evaluation Complete. Everything clear!$(RESET)\n'


presubmit:
	@$(TOOLS_DIR)/presubmit.sh


stacktrace:
	@$(DEVICE_ADDR2LINE) -e $(EXECUTABLE) $(TRACES)


show-lists:
	@$(foreach V,$(sort $(.VARIABLES)), \
		$(if $(filter-out environment% default automatic, $(origin $V)),\
			$(warning $V=$($V) ($(value $V))$newline)))

clean-coverage-files:
	@rm -f $(COVERAGE_FILES) 2> /dev/null

# ====================================================================
#
#
# Source Code Compilation Recipes
#
#
# ====================================================================
$(HEX): $(EXECUTABLE)
	@$(OBJCOPY) -O ihex "$<" "$@"
	@printf '$(YELLOW)Generated Hex Image $(RESET)   : $@\n'


$(BINARY): $(EXECUTABLE)
	@$(OBJCOPY) -O binary "$<" "$@"
	@printf '$(YELLOW)Generated Binary Image $(RESET): $@\n'


$(SIZE): $(EXECUTABLE)
	@echo
	@printf '$(WHITE)   Memory region:     Used Size  Region Size     %% Used\n'
	@printf '$(RESET)'
	@export GREP_COLOR='1;34' ; cat '$(SIZE)' | grep --color=always ".*: " || true
	@echo
	@printf '$(WHITE)Section Memory Usage$(RESET)\n'
	@$(SIZEC) --format=berkeley "$<"
	@echo


$(LIST): $(EXECUTABLE)
	@$(OBJDUMP) --disassemble --all-headers --source --demangle "$<" > "$@"
	@printf '$(YELLOW)Disassembly Generated!$(RESET)  : $@\n'


$(CORE_STATIC_LIBRARY): $(LIBRARIES)
	@rm -f "$@"
	@$(DEVICE_AR) rcT "$@" $^
	@$(DEVICE_RANLIB) "$@"
	@printf '$(YELLOW)Final Library file ( A ) $(RESET): $@\n'


$(EXECUTABLE): $(OBJECTS) $(CORE_STATIC_LIBRARY)
	@printf '$(YELLOW)Linking Executable$(RESET)    : $@\n'
	@mkdir -p "$(dir $@)"
	@$(CPPC) -Wl,--print-memory-usage $(LINK_FLAGS) -o "$@" \
			$(OBJECTS) $(CORE_STATIC_LIBRARY) 1> "$(SIZE)"


$(OBJECT_DIR)/%.c.o: %.c
	@mkdir -p "$(dir $@)"
	@$(CC) $(C_FLAGS) -std=gnu11 -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@printf '$(YELLOW)Built file ( C ) $(RESET): $<\n'


$(OBJECT_DIR)/%.o: %
	@mkdir -p "$(dir $@)"
	@$(CPPC) $(CPP_FLAGS) -std=c++2a -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@printf '$(YELLOW)Built file (C++) $(RESET): $<\n'


$(TEST_EXEC): clean-coverage-files $(OBJECTS)
	@printf '$(YELLOW)Linking Test Executable $(RESET) : $@\n'
	@mkdir -p "$(dir $@)"
	@$(CPPC) -fprofile-arcs -fPIC -fexceptions -fno-inline \
					 -fno-inline-small-functions -fno-default-inline \
					 -fkeep-inline-functions -fno-elide-constructors  \
					 -ftest-coverage -fsanitize=address -O0 -std=c++2a \
					 -o $(TEST_EXEC) $(OBJECTS)
	@printf '$(GREEN)Test Executable Generated!$(RESET)\n'


$(OBJECT_DIR)/%.tidy: %
	@mkdir -p "$(dir $@)"
	@$(CLANG_TIDY) $(if $(or $(findstring .hpp,$<), $(findstring .cpp,$<)), \
		-extra-arg="-std=c++2a") "$<"  -- \
		-D PLATFORM=host -D HOST_TEST=1 \
		-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk \
		$(INCLUDES) $(SYSTEM_INCLUDES) 2> $@
	@printf '$(GREEN)Evaluated file: $(RESET)$< \n'
