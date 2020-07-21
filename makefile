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
# Default to help when
# ==============================================================================

.DEFAULT_GOAL := help

# ==============================================================================
# Enable Undefined Variable Use Warnings
# ==============================================================================

MAKEFLAGS += --warn-undefined-variables

# Make sure MAKECMDGOALS is defined, so it doesn't cause an error itself
ifndef MAKECMDGOALS
MAKECMDGOALS = help
endif

# ==============================================================================
# Directories
# ==============================================================================

SJ2_BUILD_DIRECTORY_NAME = build
SJ2_SOURCE_DIR           = source
SJ2_LOCAL_LIBRARY_DIR    = library
SJ2_CURRENT_DIRECTORY    = $(shell pwd)
SJ2_TOOLS_DIR            = $(SJSU_DEV2_BASE)/tools
SJ2_BUILD_DIR            = $(SJ2_BUILD_DIRECTORY_NAME)/$(PLATFORM)
SJ2_OBJECT_DIR           = $(SJ2_BUILD_DIR)/objects
SJ2_TEST_EXECUTABLE_DIR  = $(SJ2_BUILD_DIRECTORY_NAME)/test
SJ2_TEST_OBJECT_DIR      = $(SJ2_TEST_EXECUTABLE_DIR)/objects
SJ2_COVERAGE_DIR         = $(SJ2_TEST_EXECUTABLE_DIR)/coverage
LIBRARY_DIR              = $(SJSU_DEV2_BASE)/library

# ==============================================================================
# Build Products and Artifacts
# ==============================================================================

EXECUTABLE = $(SJ2_BUILD_DIR)/firmware.elf
BINARY     = $(EXECUTABLE:.elf=.bin)
HEX        = $(EXECUTABLE:.elf=.hex)
LIST       = $(EXECUTABLE:.elf=.lst)
SIZE       = $(EXECUTABLE:.elf=.siz)
MAP        = $(EXECUTABLE:.elf=.map)

TEST_EXECUTABLE = $(SJ2_TEST_EXECUTABLE_DIR)/test.exe

# ==============================================================================
# ANSI Color Codes Constants
# ==============================================================================

RED     = $(shell echo "\x1B[31;1m")
GREEN   = $(shell echo "\x1B[32;1m")
YELLOW  = $(shell echo "\x1B[33;1m")
BLUE    = $(shell echo "\x1B[34;1m")
MAGENTA = $(shell echo "\x1B[35;1m")
CYAN    = $(shell echo "\x1B[36;1m")
WHITE   = $(shell echo "\x1B[37;1m")
RESET   = $(shell echo "\x1B[0m")

# ==============================================================================
# Default Flags for Firmware
# ==============================================================================

SJ2_DEFAULT_PLATFORM = lpc40xx

SJ2_DEFAULT_INCLUDES = $(SJ2_CURRENT_DIRECTORY) $(SJ2_SOURCE_DIR) \
                       $(SJ2_LOCAL_LIBRARY_DIR)

SJ2_DEFAULT_CFLAGS   = -g -fmessage-length=0 -ffunction-sections \
                       -fdata-sections -fno-exceptions -fno-omit-frame-pointer \
                       -Wno-main -Wno-variadic-macros -Wall -Wextra -Wshadow \
                       -Wfloat-equal -Wundef -Wno-format-nonliteral \
                       -Wconversion -Wdouble-promotion -Wswitch -Wformat=2 \
                       -Wno-uninitialized -Wnull-dereference \
											 -fdiagnostics-color -MMD -MP

SJ2_DEFAULT_CPPFLAGS = -std=c++2a -fno-rtti -fno-threadsafe-statics \
                       -Wold-style-cast -Woverloaded-virtual -Wsuggest-override

SJ2_DEFAULT_LDFLAGS  = -Wl,--gc-sections -Wl,-Map,"$(MAP)" \
                       --specs=nano.specs --specs=rdimon.specs

SJ2_DEFAULT_SOURCES  = source/main.cpp

# ==============================================================================
# Default Flags for Testing
# ==============================================================================

SJ2_DEFAULT_TESTS       = # test/unit_test.cpp
SJ2_DEFAULT_TEST_FLAGS := \
                -g --coverage -fPIC -fexceptions -fno-inline -fno-builtin \
                -fno-inline-small-functions -fno-default-inline \
                -fkeep-inline-functions -fno-elide-constructors  \
                -fdiagnostics-color \
                -Wall -Wno-variadic-macros -Wextra -Wshadow -Wno-main \
                -Wno-missing-field-initializers \
                -Wfloat-equal -Wundef -Wno-format-nonliteral \
                -Wdouble-promotion -Wswitch -Wnull-dereference -Wformat=2 \
                -D HOST_TEST=1 -D PLATFORM=host -O0 -std=c++2a -MMD -MP \
                -pthread

#===============================================================================
# Include a project specific makefile.
#
# Using -include to keep make form exiting if the project.mk file does not
# exist.
#===============================================================================

-include project.mk

# ==============================================================================
# Arguments
#
# Set to their defaults if project.mk does not set them first.
# ==============================================================================
WARNING_BECOME_ERRORS ?=

PLATFORM        ?= $(SJ2_DEFAULT_PLATFORM)
OPTIMIZE        ?= g
INCLUDES        ?= $(SJ2_DEFAULT_INCLUDES)
SYSTEM_INCLUDES ?=
SOURCES         ?= $(SJ2_DEFAULT_SOURCES)
CFLAGS          ?= $(SJ2_DEFAULT_CFLAGS)
CPPFLAGS        ?= $(SJ2_DEFAULT_CPPFLAGS)
LDFLAGS         ?= $(SJ2_DEFAULT_LDFLAGS)
SOURCES         ?= $(SJ2_DEFAULT_SOURCES)
TESTS           ?= $(SJ2_DEFAULT_TESTS)
TEST_ARGUMENTS  ?=


# ==============================================================================
# User Defined Arguments
#
# These are not set by default and must be set by the user.
# The makefile will error if a target needs them.
# ==============================================================================
# PORT
# OPENOCD_CONFIG
# JTAG

# ==============================================================================
# Tool chain paths
# ==============================================================================

SJCLANG      = $(shell cd $(SJSU_DEV2_BASE)/tools/clang+llvm-*/ ; pwd)
SJARMGCC     = $(shell cd $(SJSU_DEV2_BASE)/tools/gcc-arm-none-eabi-*/ ; pwd)
SJ2_OPENOCD_DIR  = $(shell grep -q Microsoft /proc/version && \
                           echo "$(SJSU_DEV2_BASE)/tools/openocd-wsl" || \
                           echo "$(SJSU_DEV2_BASE)/tools/openocd")
SJ2_OPENOCD_EXE  = $(shell grep -q Microsoft /proc/version && \
                           echo "openocd.exe" || echo "openocd")
OPENOCD          = $(SJ2_OPENOCD_DIR)/bin/$(SJ2_OPENOCD_EXE)
GDBINIT_PATH     = $(SJSU_DEV2_BASE)/tools/gdb_dashboard/gdbinit

# ==============================================================================
# Macro for building static library files
# ==============================================================================

SJ2_CORE_STATIC_LIBRARY = $(SJ2_OBJECT_DIR)/libsjsudev2.a
SJ2_STATIC_LIBRARY_ROOT = $(LIBRARY_DIR)/static_libraries
SJ2_STATIC_LIBRARY_DIR  = $(LIBRARY_DIR)/static_libraries/$(PLATFORM)

define BUILD_LIBRARY

SJ2_LIBRARIES += $(SJ2_STATIC_LIBRARY_DIR)/$(1).a

$(1)_OBJECTS = $$(addprefix $(SJ2_OBJECT_DIR)/, $$($(2):=.o))

.SECONDARY: $$($(1)_OBJECTS)

-include    $$($(1)_OBJECTS:.o=.d) # DEPENDENCIES


$(SJ2_STATIC_LIBRARY_DIR)/$(1).a: $$($(1)_OBJECTS)
	@mkdir -p "$(SJ2_STATIC_LIBRARY_DIR)"
	@rm -f "$$@"
	@$$(DEVICE_AR) rcs --plugin="$$(PLUGIN)" "$$@" $$^
	@$$(DEVICE_RANLIB) --plugin="$$(PLUGIN)" "$$@"
	@printf '$(CYAN)Built Library ( A )$(RESET) : $$@ \n'

endef

#===============================================================================
# Include sub makefiles within the SJSU-Dev2 library directory
#
# This is where a lot of the magic happens. This makefile will call more sub
# makefiles until all of the included library source files have been found.
#===============================================================================

include $(LIBRARY_DIR)/library.mk

# ==============================================================================
# Build Tools
# ==============================================================================

CC          = $(DEVICE_CC)
CPPC        = $(DEVICE_CPPC)
OBJDUMP     = $(DEVICE_OBJDUMP)
SIZEC       = $(DEVICE_SIZEC)
OBJCOPY     = $(DEVICE_OBJCOPY)
NM          = $(DEVICE_NM)
PLUGIN      = $(shell $(DEVICE_CC) --print-file-name=liblto_plugin.so)

TEST_CPPC          = g++-9
TEST_CC            = gcc-9
CODE_COVERAGE_TOOL = gcov-9
CLANG_TIDY         = $(SJCLANG)/bin/clang-tidy
HOST_SYMBOLIZER    = $(SJCLANG)/bin/llvm-symbolizer

# ==============================================================================
# Post Makefile Inclusion
# ==============================================================================

-include post_library.mk

# ==============================================================================
# Final Flag Compositions
# ==============================================================================

LINKER_SCRIPT   ?= $(LIBRARY_DIR)/L0_Platform/$(PLATFORM)/linker.ld

INCLUDES        := $(addsuffix ", $(addprefix -I", $(INCLUDES)))
SYSTEM_INCLUDES := $(addsuffix ", $(addprefix -idirafter", $(SYSTEM_INCLUDES)))
OBJECTS         := $(addprefix $(SJ2_OBJECT_DIR)/, $(SOURCES:=.o))

CFLAGS          := -O$(OPTIMIZE) -D PLATFORM=$(PLATFORM) \
                   $(SYSTEM_INCLUDES) $(INCLUDES) $(CFLAGS)  \
                   $(WARNING_BECOME_ERRORS)
CPPFLAGS        := $(SYSTEM_INCLUDES) $(INCLUDES) $(CPPFLAGS) $(CFLAGS)
LDFLAGS         := $(CFLAGS) $(LDFLAGS) $(addprefix -T ,$(LINKER_SCRIPT))
TEST_FLAGS      := $(SYSTEM_INCLUDES) $(INCLUDES) $(SJ2_DEFAULT_TEST_FLAGS) \
                   $(WARNING_BECOME_ERRORS)

SJ2_TEST_OBJECTS   := $(addprefix $(SJ2_TEST_OBJECT_DIR)/, $(TESTS:=.o))
SJ2_COVERAGE_FILES := $(shell find $(SJ2_BUILD_DIRECTORY_NAME) -name "*.gcda" \
                              2> /dev/null)

# ==============================================================================
# Rebuild source files if header file dependencies changes
# ==============================================================================

-include       $(OBJECTS:.o=.d)
-include       $(SJ2_TEST_OBJECTS:.o=.d)
-include       $(LINKER_SCRIPT:.ld=.d)

# ==============================================================================
# Phony Targets Declaration
# ==============================================================================


.PHONY: application flash clean library-clean purge $(SIZE) clean-coverage \
        run-test coverage clean-coverage run-test test help

# ==============================================================================
# Application Build Targets
# ==============================================================================


help:
	@cat $(SJ2_TOOLS_DIR)/makefile_help_menu.txt | \
	GREP_COLOR='1;31' grep --color=always -e " [-]*"  -e '**' | \
	GREP_COLOR='1;34' grep --color=always -e "==" -e '**'


# ==============================================================================
# Application Build Targets
# ==============================================================================


application: | $(LIST) $(HEX) $(BINARY) $(SIZE)


# ==============================================================================
# Firmware programming & debugging targets
# ==============================================================================


program: application
	@printf '$(MAGENTA)Programming chip via debug device...$(RESET)\n'
	@$(OPENOCD) -s $(SJ2_OPENOCD_DIR)/scripts/ \
			-c "source [find interface/$(JTAG).cfg]" -f $(OPENOCD_CONFIG) \
			-c "program \"$(EXECUTABLE)\" reset exit"


debug:
	@printf '$(MAGENTA)Starting firmware debug...$(RESET)\n'
	@$(SJ2_TOOLS_DIR)/launch_openocd_gdb.sh \
			"$(DEVICE_GDB)" \
			"$(GDBINIT_PATH)" \
			"$(PLATFORM)" \
			"$(SJ2_CURRENT_DIRECTORY)/$(EXECUTABLE)" \
			"$(SJ2_OPENOCD_DIR)" \
			"$(JTAG)" \
			"$(OPENOCD_CONFIG)" \
			"$(SJ2_OPENOCD_EXE)"


debug-test:
	gdb build/tests.exe


flash: | application platform-flash
execute: flash


# ==============================================================================
# Project cleaning targets
# ==============================================================================


clean:
	rm -fr $(SJ2_BUILD_DIRECTORY_NAME)
	@mkdir -p $(SJ2_BUILD_DIRECTORY_NAME)


library-clean:
	rm -fr $(SJ2_STATIC_LIBRARY_ROOT)
	@mkdir -p $(SJ2_STATIC_LIBRARY_ROOT)


purge: | clean library-clean


# ==============================================================================
# Testing Targets
# ==============================================================================


clean-coverage:
	@rm -f $(SJ2_COVERAGE_FILES) 2> /dev/null


coverage:
	@printf '$(YELLOW)Generating Coverage Files $(RESET) : '

	@mkdir -p "$(SJ2_COVERAGE_DIR)"

	@gcovr \
		--filter="$(LIBRARY_DIR)" \
		-e "$(LIBRARY_DIR)/newlib" \
		-e "$(LIBRARY_DIR)/third_party" \
		-e "$(LIBRARY_DIR)/L4_Testing" \
		--html --html-details --gcov-executable="$(CODE_COVERAGE_TOOL)" \
		-o $(SJ2_COVERAGE_DIR)/coverage.html

	@printf '$(GREEN)DONE!$(RESET)\n'

run-test:
	@export ASAN_SYMBOLIZER_PATH=$(HOST_SYMBOLIZER) && \
	 ASAN_OPTIONS="symbolize=1 color=always" $(TEST_EXECUTABLE) $(TEST_ARGUMENTS)


test: | clean-coverage $(TEST_EXECUTABLE)
	+@$(MAKE) run-test --no-print-directory
	+@$(MAKE) coverage --no-print-directory


# ==============================================================================
# Source Code Compilation Recipes
# ==============================================================================


$(HEX): $(EXECUTABLE)
	@$(OBJCOPY) -O ihex "$<" "$@"
	@printf '$(YELLOW)Generated Hex Image $(RESET)   : $@\n'


$(BINARY): $(EXECUTABLE)
	@$(OBJCOPY) -O binary "$<" "$@"
	@printf '$(YELLOW)Generated Binary Image $(RESET): $@\n'


$(SIZE): $(EXECUTABLE)
	@printf '$(GREEN)==================================================$(RESET)\n'
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
	@printf '$(YELLOW)Disassembly Generated$(RESET)  : $@\n'


$(SJ2_CORE_STATIC_LIBRARY): $(SJ2_LIBRARIES)
	@rm -rf "$@"
	@mkdir -p "$(dir $@)"
	@$(DEVICE_AR) rcT --plugin="$(PLUGIN)" "$@" $^
	@$(DEVICE_RANLIB) --plugin="$(PLUGIN)" "$@"
	@printf '$(CYAN)Final Library ( A ) $(RESET): $@\n'


$(EXECUTABLE): $(OBJECTS) $(SJ2_CORE_STATIC_LIBRARY) $(LINKER_SCRIPT)
	@printf '$(GREEN)==================================================$(RESET)\n'
	@printf '$(YELLOW)Linking Executable$(RESET)     : $@\n'
	@mkdir -p "$(dir $@)"
	@$(CPPC) -Wl,--print-memory-usage $(LDFLAGS) -o "$@" \
						$(OBJECTS) $(SJ2_CORE_STATIC_LIBRARY) 1> "$(SIZE)"
	@printf '$(GREEN)==================================================$(RESET)\n'


$(SJ2_OBJECT_DIR)/%.c.o: %.c
	@mkdir -p "$(dir $@)"
	@$(CC) $(CFLAGS) -MF"$(@:%.o=%.d)" -std=gnu11 -MT"$(@)" -c -o \
					"$@" "$<"
	@printf '$(YELLOW)Built Source  ( C ) $(RESET): $<\n'


$(SJ2_OBJECT_DIR)/%.o: %
	@mkdir -p "$(dir $@)"
	@$(CPPC) $(CPPFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o \
						"$@" "$<"
	@printf '$(YELLOW)Built Source  (C++) $(RESET): $<\n'


# ==============================================================================
# Testing Code Compilation Recipes
# ==============================================================================


$(TEST_EXECUTABLE): $(SJ2_TEST_OBJECTS)
	@printf '$(GREEN)==================================================$(RESET)\n'
	@printf '$(YELLOW)Linking Test Executable $(RESET) : $@\n'
	@mkdir -p "$(dir $@)"
	@$(TEST_CPPC) $(TEST_FLAGS) -o $(TEST_EXECUTABLE) $(SJ2_TEST_OBJECTS)
	@printf '$(GREEN)==================================================$(RESET)\n'
	@printf '$(GREEN)Test Executable Generated!$(RESET)\n'


$(SJ2_TEST_OBJECT_DIR)/%.c.o: %.c
	@mkdir -p "$(dir $@)"
	@$(TEST_CPPC) $(TEST_FLAGS) \
					-MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@printf '$(YELLOW)Built Source  ( C ) $(RESET): $<\n'


$(SJ2_TEST_OBJECT_DIR)/%.o: %
	@mkdir -p "$(dir $@)"
	@$(TEST_CPPC) $(TEST_FLAGS) \
					 -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@printf '$(YELLOW)Built Source  (C++) $(RESET): $<\n'

