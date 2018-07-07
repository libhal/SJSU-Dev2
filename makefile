# Allow settiing a project name from the environment, default to firmware.
# Only affects the name of the generated binary.
# TODO(#82): Set this from the directory this makefile is stored in
PROJ    ?= firmware
# Affects what DBC is generated for SJSUOne board
ENTITY  ?= DBG

# IMPORTANT: Be sure to source env.sh to access these via the PATH variable.
DEVICE_CC      = arm-none-eabi-gcc
DEVICE_CPPC    = arm-none-eabi-g++
DEVICE_OBJDUMP = arm-none-eabi-objdump
DEVICE_SIZEC   = arm-none-eabi-size
DEVICE_OBJCOPY = arm-none-eabi-objcopy
DEVICE_NM      = arm-none-eabi-nm
# IMPORTANT: Must be accessible via the PATH variable!!!
HOST_CC        ?= gcc
HOST_CPPC      ?= g++
HOST_OBJDUMP   ?= objdump
HOST_SIZEC     ?= size
HOST_OBJCOPY   ?= objcopy
HOST_NM        ?= nm

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

# Internal build directories
BUILD_DIR = build

TEST_DIR  = $(BUILD_DIR)/test
ifeq ($(MAKECMDGOALS), test)
OBJ_DIR   = $(TEST_DIR)/compiled
else
OBJ_DIR   = $(BUILD_DIR)/compiled
endif
BIN_DIR   = $(BUILD_DIR)/binaries
DBC_DIR   = $(BUILD_DIR)/can-dbc
COVERAGE  = $(BUILD_DIR)/coverage
LIB_DIR   = $(SJLIBDIR)
FIRMWARE  = $(SJBASE)/firmware
TOOLS     = $(SJBASE)/tools
COMPILED_HEADERS    = $(BUILD_DIR)/headers
CURRENT_DIRECTORY	= $(shell pwd)
# Source files folder
SOURCE    = source
define n


endef

ifndef SJDEV
$(error $n$n=============================================$nSJSUOne environment variables not set.$nPLEASE run "source env.sh"$n=============================================$n$n)
endif

#########
# FLAGS #
#########
CORTEX_M4F = -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
			 -fabi-version=0
# CORTEX_M4F  = -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb
OPTIMIZE  = -O3 -fmessage-length=0 -ffunction-sections -fdata-sections -fno-exceptions \
               -fsingle-precision-constant -fno-rtti
DEBUG     = -g
WARNINGS  = -Wall -Wextra -Wshadow -Wlogical-op -Wfloat-equal \
            -Wdouble-promotion -Wduplicated-cond -Wlogical-op -Wswitch \
            -Wnull-dereference -Wold-style-cast -Wuseless-cast -Wformat=2 \
            -Wundef -Wconversion -Woverloaded-virtual -Wsuggest-final-types \
            -Wsuggest-final-methods -Wsuggest-override \
            -Wframe-larger-than=1024
DEFINES   = -DARM_MATH_CM4=1 -D__FPU_PRESENT=1U
DISABLED_WARNINGS = -Wno-main -Wno-variadic-macros
COMMON_FLAGS = $(CORTEX_M4F) $(OPTIMIZE) $(DEBUG) $(WARNINGS)  $(DEFINES) \
               $(DISABLED_WARNINGS)
CFLAGS_COMMON = $(COMMON_FLAGS) \
    -I"$(CURRENT_DIRECTORY)/" \
    -I"$(LIB_DIR)/" \
    -I"$(LIB_DIR)/newlib" \
    -I"$(LIB_DIR)/third_party/" \
    -I"$(LIB_DIR)/third_party/FreeRTOS" \
    -I"$(LIB_DIR)/third_party/FreeRTOS/trace" \
    -I"$(LIB_DIR)/third_party/FreeRTOS/include" \
    -I"$(LIB_DIR)/third_party/FreeRTOS/portable" \
    -I"$(LIB_DIR)/third_party/FreeRTOS/portable/no_mpu" \
    -I"$(DBC_DIR)" \
    -MMD -MP -c

ifeq ($(MAKECMDGOALS), test)
CFLAGS = -fprofile-arcs -fPIC -fexceptions -fno-inline \
         -fno-inline-small-functions -fno-default-inline \
         -ftest-coverage --coverage \
         -fno-elide-constructors \
         $(filter-out $(CORTEX_M4F) $(OPTIMIZE), $(CFLAGS_COMMON)) \
         -O3
else
CFLAGS = $(CFLAGS_COMMON)
endif

LINKFLAGS = $(COMMON_FLAGS) \
    -T $(LIB_DIR)/LPC4078.ld \
    -Xlinker \
    --gc-sections -Wl,-Map,"$(MAP)" \
    -specs=nano.specs
##############
# Test files #
##############
# Find all files that end with "_test.cpp"
SOURCE_TESTS  = $(shell find $(SOURCE) \
                         -name "*_test.cpp" \
                         -not -path "$(SOURCE)/third_party/*" \
                         2> /dev/null)
# Find all library that end with "_test.cpp"
LIBRARY_TESTS = $(shell find "$(LIB_DIR)" -name "*_test.cpp" \
                         -not -path "$(LIB_DIR)/third_party/*")
TESTS = $(SOURCE_TESTS) $(LIBRARY_TESTS)
OMIT_LIBRARIES = $(shell find "$(LIB_DIR)" \
                         -name "startup.cpp" -o \
                         -name "*.cpp" \
                         -path "$(LIB_DIR)/newlib/*" -o \
                         -path "$(LIB_DIR)/third_party/*")
OMIT_SOURCES   = $(shell find $(SOURCE) \
                         -name "main.cpp" \
                         -not -path "$(LIB_DIR)/third_party/*")
OMIT = $(OMIT_LIBRARIES) $(OMIT_SOURCES)
################
# Source files #
################
DBC_BUILD     = $(DBC_DIR)/generated_can.h
LIBRARY_FILES = $(shell find "$(LIB_DIR)" \
                         -name "*.c" -o \
                         -name "*.cpp" \
                         -not -path "$(LIB_DIR)/third_party/*")
# Remove all test files from LIBRARY_FILES
LIBRARIES     = $(filter-out $(LIBRARY_TESTS), $(LIBRARY_FILES))
SOURCE_FILES  = $(shell find $(SOURCE) \
                          -name "*.c" -o \
                         -name "*.s" -o \
                         -name "*.S" -o \
                         -name "*.cpp" \
                         2> /dev/null)
# Remove all test files from SOURCE_FILES
SOURCES     = $(filter-out $(SOURCE_TESTS), $(SOURCE_FILES))
ifeq ($(MAKECMDGOALS), test)
COMPILABLES = $(filter-out $(OMIT), $(LIBRARIES) $(SOURCES) $(TESTS))
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

# This line allows the make to rebuild if header file #changes
# This is feature and not a bug.
# Otherwise updates to header files do not
# register as an update to the source code.
DEPENDENCIES = $(OBJECT_FILES:.o=.d)
-include       $(DEPENDENCIES)#
# Tell make to delete built files it has created if an error occurs
.DELETE_ON_ERROR:
# Tell make that the default recipe is the default
.DEFAULT_GOAL := default
# Tell make that these recipes don't have a end product
.PHONY: build cleaninstall telemetry monitor show-lists clean flash telemetry \
        presubmit default
print-%  : ; @echo $* = $($*)

# When the user types just "make" this should appear to them
default:
	@echo "List of available targets:"
	@echo "    build         - builds firmware project"
	@echo "    flash         - builds and installs firmware on to SJOne board"
	@echo "    telemetry     - will launch telemetry interface"
	@echo "    clean         - cleans project folder"
	@echo "    cleaninstall  - cleans, builds and installs firmware"
	@echo "    show-lists    - Shows all object files that will be compiled"
	@echo "    presubmit     - run presubmit script and "
# Build recipe
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

$(LIST): $(EXECUTABLE)
	@echo ' '
	@echo 'Invoking: Cross ARM GNU Create Assembly Listing'
	@$(OBJDUMP) --source --all-headers --demangle --line-numbers --wide "$<" > "$@"
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
	@$(CPPC) $(CFLAGS) -std=c++17 -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
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
	@$(CPPC) $(CFLAGS) -std=c++17 -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
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
	python "$(LIB_DIR)/$(DBC_DIR)/dbc_parse.py" -i "$(LIB_DIR)/$(DBC_DIR)/243.dbc" -s $(ENTITY) > $(DBC_BUILD)

$(DBC_DIR):
	mkdir -p $(DBC_DIR)

$(OBJ_DIR):
	@echo 'Creating Objects Folder: $<'
	mkdir $(OBJ_DIR)

$(BIN_DIR):
	@echo 'Creating Binary Folder: $<'
	mkdir $(BIN_DIR)

clean:
	rm -fR $(BUILD_DIR)

flash: build
	@bash -c "\
	source $(TOOLS)/Hyperload/modules/bin/activate && \
	python $(TOOLS)/Hyperload/hyperload.py $(SJDEV) $(SYMBOLS_HEX)"

telemetry:
	@bash -c "\
	source $(TOOLS)/Telemetry/modules/bin/activate && \
	python $(TOOLS)/Telemetry/telemetry.py"

lint:
	python $(TOOLS)/cpplint/cpplint.py $(LIBRARIES) $(SOURCES)

test: $(COVERAGE) $(TEST_EXEC)
	@valgrind --leak-check=full --track-origins=yes -v $(TEST_EXEC) -s
	# @./$(TEST_EXEC)
	@gcovr --root $(FIRMWARE) --object-directory $(BUILD_DIR) \
	-e "$(LIB_DIR)/newlib" \
	-e "$(LIB_DIR)/third_party" \
	--html --html-details -o $(COVERAGE)/coverage.html

test-all: $(COVERAGE) $(TEST_EXEC)

$(COVERAGE):
	mkdir -p $(COVERAGE)

# $(TEST_EXEC):
$(TEST_EXEC): $(TEST_FRAMEWORK) $(OBJECT_FILES)
	@echo " \\──────────────────────────────/"
	@echo "  \\ Generating test executable /"
	@mkdir -p "$(dir $@)"
	@echo 'Finished building target: $@'
	@$(CPPC) -fprofile-arcs -fPIC -fexceptions -fno-inline \
         -fno-inline-small-functions -fno-default-inline \
         -ftest-coverage --coverage \
         -fno-elide-constructors \
         -fprofile-arcs -ftest-coverage -fPIC -O0 \
         -o $(TEST_EXEC) $(OBJECT_FILES)
	@echo "   \\──────────────────────────/"
	@echo "    \\       Finished         /"
	@echo "     \\──────────────────────/"
	@echo "      \\    Running Test    /"
	@echo "       \\──────────────────/"

%.hpp.gch: %.hpp
	@echo 'Precompiling HPP file: $<'
	@echo 'Invoking: C Compiler'
	@mkdir -p "$(dir $@)"
	@$(CPPC) $(CFLAGS) -std=c++17 -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" \
        $(LIB_DIR)/L5_Testing/testing_frameworks.hpp
	@echo 'Finished building: $<'
	@echo ' '

presubmit:
	$(TOOLS)/presubmit.sh