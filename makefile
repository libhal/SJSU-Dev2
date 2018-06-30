# Allow settiing a project name from the environment, default to firmware.
# Only affects the name of the generated binary.
# TODO: Set this from the directory this makefile is stored in
PROJ    ?= firmware
# Affects what DBC is generated for SJSUOne board
ENTITY  ?= DBG

# IMPORTANT: Must be accessible via the PATH variable!!!
DEVICE_CC      = arm-none-eabi-gcc
DEVICE_CPPC    = arm-none-eabi-g++
DEVICE_OBJDUMP = arm-none-eabi-objdump
DEVICE_SIZEC   = arm-none-eabi-size
DEVICE_OBJCOPY = arm-none-eabi-objcopy
DEVICE_NM      = arm-none-eabi-nm

CC      = $(DEVICE_CC)
CPPC    = $(DEVICE_CPPC)
OBJDUMP = $(DEVICE_OBJDUMP)
SIZEC   = $(DEVICE_SIZEC)
OBJCOPY = $(DEVICE_OBJCOPY)
NM      = $(DEVICE_NM)

# Internal build directories
BUILD_DIR = build
TEST_DIR  = $(BUILD_DIR)/test
OBJ_DIR   = $(BUILD_DIR)/compiled
BIN_DIR   = $(BUILD_DIR)/binaries
DBC_DIR   = $(BUILD_DIR)/can-dbc
LIB_DIR   = $(SJLIBDIR)
FIRMWARE  = $(SJBASE)/firmware
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
CORTEX_M4F = -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16
# CORTEX_M4F  = -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb
OPTIMIZE  = -O3 -fmessage-length=0 -ffunction-sections -fdata-sections -fno-exceptions \
               -fsingle-precision-constant -fno-rtti
DEBUG     = -g
WARNINGS  = -Wall -Wextra -Wpedantic -Wshadow -Wlogical-op -Wfloat-equal \
            -Wdouble-promotion -Wduplicated-cond -Wlogical-op -Wswitch \
            -Wnull-dereference -Wold-style-cast -Wuseless-cast -Wformat=2 \
            -Wundef -Wconversion -Woverloaded-virtual -Wsuggest-final-types \
            -Wsuggest-final-methods -Wsuggest-override \
            -Wframe-larger-than=1024
            #-Walloc-zero -Walloc-size-larger-than=8kB -Walloca-larger-than=1

DEFINES   = -DARM_MATH_CM4=1 -D__FPU_PRESENT=1U
DISABLED_WARNINGS = -Wno-main
COMMON_FLAGS = $(CORTEX_M4F) $(OPTIMIZE) $(DEBUG) $(WARNINGS)  $(DEFINES) \
               $(DISABLED_WARNINGS)

CFLAGS_COMMON = $(COMMON_FLAGS) \
    -fabi-version=0 \
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

CFLAGS = $(CFLAGS_COMMON)

LINKFLAGS = $(COMMON_FLAGS) \
    -T $(LIB_DIR)/LPC4078.ld \
    -Xlinker \
    --gc-sections -Wl,-Map,"$(MAP)" \
    -specs=nano.specs
################
# Source files #
################
DBC_BUILD     = $(DBC_DIR)/generated_can.h
LIBRARIES     = $(shell find "$(LIB_DIR)" \
                         -name "*.c" -o \
                         -name "*.cpp" \
                         -not -path "$(LIB_DIR)/third_party/*")
SOURCES       = $(shell find $(SOURCE) \
                          -name "*.c" -o \
                         -name "*.s" -o \
                         -name "*.S" -o \
                         -name "*.cpp" \
                         2> /dev/null)
COMPILABLES = $(LIBRARIES) $(SOURCES)
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
# This line allows the make to rebuild if header file #changes
# This is feature and not a bug. 
# Otherwise updates to header files do not
# register as an update to the source code.
DEPENDENCIES = $(OBJECT_FILES:.o=.d)
-include       $(DEPENDENCIES)
# Tell make to delete built files it has created if an error occurs
.DELETE_ON_ERROR:
# Tell make that the default recipe is the default
.DEFAULT_GOAL := default
# Tell make that these recipes don't have a end product
.PHONY: build cleaninstall telemetry monitor show-lists clean flash telemetry \
        default
print-%  : ; @echo $* = $($*)

# When the user types just "make" this should appear to them
default:
	@echo "List of available targets:"
	@echo "    build         - builds firmware project"
	@echo "    flash         - builds and installs firmware on to SJOne board"
	@echo "    telemetry     - will launch telemetry interface"
	@echo "    clean         - cleans project folder"
	@echo "    cleaninstall  - cleans, builds and installs firmware"
	@echo "    show-lists - Shows all object files that will be compiled"
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
	python "$(LIB_DIR)/$(DBC_DIR)/dbc_parse.py" -i \
	"$(LIB_DIR)/$(DBC_DIR)/243.dbc" -s $(ENTITY) > $(DBC_BUILD)

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
	source $(SJBASE)/tools/Hyperload/modules/bin/activate && \
	python $(SJBASE)/tools/Hyperload/hyperload.py $(SJDEV) $(SYMBOLS_HEX)"

telemetry:
	@bash -c "\
	source $(SJBASE)/tools/Telemetry/modules/bin/activate && \
	python $(SJBASE)/tools/Telemetry/telemetry.py"
	