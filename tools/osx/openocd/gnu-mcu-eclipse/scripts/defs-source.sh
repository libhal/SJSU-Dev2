# -----------------------------------------------------------------------------

# Helper script used in the second edition of the build scripts.
# As the name implies, it should contain only definitions and should
# be included with 'source' by the host and container scripts.

# Warning: MUST NOT depend on $HOME or other environment variables.

# -----------------------------------------------------------------------------

# Used to display the application name.
APP_NAME=${APP_NAME:-"OpenOCD"}

# Used as part of file/folder paths.
APP_UC_NAME=${APP_UC_NAME:-"OpenOCD"}
APP_LC_NAME=${APP_LC_NAME:-"openocd"}

DISTRO_UC_NAME=${DISTRO_UC_NAME:-"GNU MCU Eclipse"}
DISTRO_LC_NAME=${DISTRO_LC_NAME:-"gnu-mcu-eclipse"}

BRANDING=${BRANDING:-"${DISTRO_UC_NAME} OpenOCD"}

# GCC_TARGET=${GCC_TARGET:-"riscv-none-embed"}
# GCC_ARCH=${GCC_ARCH:-"rv64imafdc"}
# GCC_ABI=${GCC_ABI:-"lp64d"}

CONTAINER_SCRIPT_NAME=${CONTAINER_SCRIPT_NAME:-"container-build.sh"}
CONTAINER_LIB_FUNCTIONS_SCRIPT_NAME=${CONTAINER_LIB_FUNCTIONS_SCRIPT_NAME:-"container-lib-functions-source.sh"}
CONTAINER_APP_FUNCTIONS_SCRIPT_NAME=${CONTAINER_APP_FUNCTIONS_SCRIPT_NAME:-"container-openocd-functions-source.sh"}

# -----------------------------------------------------------------------------
