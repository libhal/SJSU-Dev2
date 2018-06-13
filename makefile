# Allow settiing a project name from the environment, default to firmware.
# Only affects the name of the generated binary.
# TODO: Set this from the directory this makefile is stored in
PROJ 			?= firmware
# Affects what DBC is generated for SJSUOne board
ENTITY 			?= DBG

# IMPORTANT: Must be accessible via the PATH variable!!!
CC              = arm-none-eabi-gcc
CPPC            = arm-none-eabi-g++
OBJDUMP         = arm-none-eabi-objdump
SIZEC           = arm-none-eabi-size
OBJCOPY         = arm-none-eabi-objcopy
NM 		        = arm-none-eabi-nm

# Internal build directories
OBJ_DIR			= compiled
BIN_DIR			= binaries
DBC_DIR			= can-dbc
LIB_DIR 		= $(SJLIBDIR)

define n


endef

ifndef SJDEV
$(error $n$n=============================================$nSJSUOne environment variables not set.$nPLEASE run "source env.sh"$n=============================================$n$n)
endif

# FLAGS
CORTEX_M4F	= -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
OPTIMIZE 	= -O3 -fmessage-length=0 -ffunction-sections -fdata-sections -fno-exceptions \
			   -fsingle-precision-constant -fno-rtti
DEBUG 		= -g
WARNINGS 	= -Wall -Wextra -Wpedantic -Wshadow -Wlogical-op -Wfloat-equal \
			  -Wdouble-promotion -Wduplicated-cond -Wlogical-op -Wswitch \
			  -Wnull-dereference -Wold-style-cast -Wuseless-cast -Wformat=2 \
			  -Wundef -Wconversion -Wsign-conversion -Woverloaded-virtual \
			  -Wsuggest-attribute=const -Wsuggest-final-types -Wsuggest-final-methods -Wsuggest-override \
			  -Wframe-larger-than=1024
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
    -MMD -MP -c

#-nostartfiles

LINKFLAGS = $(COMMON_FLAGS) \
	-T $(LIB_DIR)/loader.ld \
	-Xlinker \
	--gc-sections -Wl,-Map,"$(MAP)" \
	-specs=nano.specs

DBC_BUILD        	= $(DBC_DIR)/generated_can.h
LIBRARIES			= $(shell find "$(LIB_DIR)" -name '*.c' -o -name '*.cpp')
SOURCES				= $(shell find source \
 						 -name '*.c' -o\
						 -name '*.s' -o \
						 -name '*.S' -o \
						 -name '*.cpp' \
						 -not -path './test/*' \
						 2> /dev/null)
COMPILABLES 		= $(LIBRARIES) $(SOURCES)

# $(patsubst %.cpp,%.o, LIST) 		: Replace .cpp -> .o
# $(patsubst %.c,%.o, LIST)			: Replace .c -> .o
# $(patsubst src/%,%, LIST) 		: Replace src/path/file.o -> path/file.o
# $(addprefix $(OBJ_DIR)/, LIST) 	: Add OBJ DIR to path (path/file.o -> obj/path/file.o)
OBJECT_FILES 		= $(addprefix $(OBJ_DIR)/, \
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
EXECUTABLE			= $(BIN_DIR)/$(PROJ).elf
SYMBOL_TABLE 		= $(BIN_DIR)/symbol-table.c
BINARY				= $(EXECUTABLE:.elf=.bin)
HEX					= $(EXECUTABLE:.elf=.hex)
SYMBOLS_HEX			= $(EXECUTABLE:.elf=.symbols.hex)
LIST				= $(EXECUTABLE:.elf=.lst)
SYMBOLS_LIST		= $(EXECUTABLE:.elf=.symbols.lst)
SIZE				= $(EXECUTABLE:.elf=.siz)
SYMBOLS_SIZE		= $(EXECUTABLE:.elf=.symbols.siz)
MAP					= $(EXECUTABLE:.elf=.map)
SYMBOLS_MAP			= $(EXECUTABLE:.elf=.symbols.map)
SYMBOLS				= $(EXECUTABLE:.elf=.sym)
SYMBOLS_EXECUTABLE	= $(EXECUTABLE:.elf=.symbols.elf)
SYMBOLS_OBJECT 		= $(SYMBOLS).o

# Will cause dependency rebuild if header file changes
DEPENDENCIES 		= $(OBJECT_FILES:.o=.d)
-include 			  $(DEPENDENCIES)

.DELETE_ON_ERROR:
.PHONY: sym-build build cleaninstall telemetry monitor show-obj-list clean sym-flash flash telemetry

default:
	@echo "List of available targets:"
	@echo "    build         - builds firmware project"
	@echo "    sym-build     - builds firmware project with embeddeding symbol table"
	@echo "    flash         - builds and installs firmware on to SJOne board"
	@echo "    sym-flash     - builds and installs firmware on to SJOne board without embeddeding symbol table"
	@echo "    telemetry     - will launch telemetry interface"
	@echo "    clean         - cleans project folder"
	@echo "    cleaninstall  - cleans, builds and installs firmware"
	@echo "    show-obj-list - Shows all object files that will be compiled"

build: $(DBC_DIR) $(OBJ_DIR) $(BIN_DIR) $(SIZE) $(LIST) $(HEX) $(BINARY)

sym-build: $(DBC_DIR) $(OBJ_DIR) $(BIN_DIR) $(SYMBOLS_SIZE) $(SYMBOLS_LIST) $(SYMBOLS_HEX)

cleaninstall: clean build flash

show-obj-list:
	@echo $(OBJECT_FILES)

print-%  : ; @echo $* = $($*)

$(SYMBOLS_HEX): $(SYMBOLS_EXECUTABLE)
	@echo ' '
	@echo 'Invoking: Cross ARM GNU Create Symbol Linked Flash Image'
	@$(OBJCOPY) -O ihex "$<" "$@"
	@echo 'Finished building: $@'
	@echo ' '

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

$(SYMBOLS_SIZE): $(SYMBOLS_EXECUTABLE)
	@echo ' '
	@echo 'Invoking: Cross ARM GNU Print Size'
	@$(SIZEC) --format=berkeley "$<"
	@echo 'Finished building: $@'
	@echo ' '

$(SIZE): $(EXECUTABLE)
	@echo ' '
	@echo 'Invoking: Cross ARM GNU Print Size'
	@$(SIZEC) --format=berkeley "$<"
	@echo 'Finished building: $@'
	@echo ' '

$(SYMBOLS_LIST): $(SYMBOLS_EXECUTABLE)
	@echo ' '
	@echo 'Invoking: Cross ARM GNU Create Assembly Listing'
	@$(OBJDUMP) --source --all-headers --demangle --line-numbers --wide "$<" > "$@"
	@echo 'Finished building: $@'
	@echo ' '

$(LIST): $(EXECUTABLE)
	@echo ' '
	@echo 'Invoking: Cross ARM GNU Create Assembly Listing'
	@$(OBJDUMP) --source --all-headers --demangle --line-numbers --wide "$<" > "$@"
	@echo 'Finished building: $@'
	@echo ' '

$(SYMBOLS_EXECUTABLE): $(SYMBOLS_OBJECT)
	@echo ' '
	@echo 'Linking: FINAL Symbol Table Linked EXECUTABLE'
	@mkdir -p "$(dir $@)"
	@$(CPPC) $(LINKFLAGS) -o "$@" $(SYMBOLS_OBJECT) $(OBJECT_FILES)
	@echo 'Finished building target: $@'
	@echo ' '

$(SYMBOLS_OBJECT): $(SYMBOL_TABLE)
	@echo 'Invoking: Cross ARM GNU Generating Symbol Table Object File'
	@$(CC) $(CFLAGS) -std=gnu11 -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $@'
	@echo ' '

$(SYMBOL_TABLE): $(SYMBOLS)
	@echo 'Generating: Symbol Table C file'
	# Copying firmware.sym to .c file
	@echo '________' > "$@"
	@cat "$<" >> "$@"
	# Remove everything that is not a function (text/code) symbols
	@perl -p -i -e 's;^.* [^T] .*\n;;' "$@"
	@perl -p -i -e 's;^.* T __.*\n;;' "$@"
	@perl -p -i -e 's;^.* T _.*\n;;' "$@"
	@perl -p -i -e 's;^.* T operator .*\n;;' "$@"
	@perl -p -i -e 's;^.* T typeinfo for.*\n;;' "$@"
	@perl -p -i -e 's;^.* T typeinfo name for .*\n;;' "$@"
	@perl -p -i -e 's;^.* T typeinfo name for .*\n;;' "$@"
	@perl -p -i -e 's;^.* T vtable for .*\n;;' "$@"
	@perl -p -i -e 's;^.* T vtable for .*\n;;' "$@"
	# Prepend " to each line
	@perl -p -i -e 's;^;\t";' "$@"
	# Append " to each line
	@perl -p -i -e 's;$$;\\n\";' "$@"
	# Append variable declaration
	@perl -p -i -e 's;^.*________.*;__attribute__((section(".symbol_table"))) const char APP_SYM_TABLE[] =;' "$@"
	# append it with a curly brace and semicolon
	@echo ";" >> "$@"
	@echo ' '

$(SYMBOLS): $(EXECUTABLE)
	@echo 'Generating: Cross ARM GNU NM Generate Symbol Table'
	@$(NM) -C "$<" > "$@"
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
	@$(CPPC) $(CFLAGS) -std=gnu++17 -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
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
	@$(CPPC) $(CFLAGS) -std=gnu++17 -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
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
	rm -fR $(OBJ_DIR) $(BIN_DIR) $(DBC_DIR)

sym-flash: sym-build
	@bash -c "\
	source $(SJBASE)/tools/Hyperload/modules/bin/activate && \
	python $(SJBASE)/tools/Hyperload/hyperload.py $(SJDEV) $(HEX)"

flash: build
	@bash -c "\
	source $(SJBASE)/tools/Hyperload/modules/bin/activate && \
	python $(SJBASE)/tools/Hyperload/hyperload.py $(SJDEV) $(SYMBOLS_HEX)"

telemetry:
	@bash -c "\
	source $(SJBASE)/tools/Telemetry/modules/bin/activate && \
	python $(SJBASE)/tools/Telemetry/telemetry.py"