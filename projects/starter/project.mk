# ======================
# SPECIFYING A PLATFORM
# ======================
# If you want your project compile for a specific platform, you can define it
# here. Defaults to lpc40xx (SJTwo).
# This is usefule because you can go from writting:
#
#   make application PLATFORM=lpc40xx
#      TO
#   make application
#
# NOTE: lpc40xx is for the SJTwo board
PLATFORM = lpc40xx

# =============
# UNIT TESTING
# =============
# Add the source files of unit tests from your project.
# Invoke "make user-test" in order to compile project tests.
# Invoke "make run-test" to run that test program.
#
# You need to add source and test file that is required for your test. If you
# added a test for the gps module, you need to include the .cpp file if it
# exists.
# DO NOT include a file with "int main()" in it. An main() is added for you
# automatically when you run "make user-test"
#
# Example of adding your own tests:
#
#    USER_TESTS += source/gps_test.cpp
#    USER_TESTS += source/gps.cpp
#    USER_TESTS += source/sonar_test.cpp
#    USER_TESTS += source/sonar.cpp
#
# Example of adding a test from the library folder:
#
#    USER_TESTS += $(LIBRARY_DIR)/L1_Peripheral/test/adc_test.cpp
#
USER_TESTS +=

# =============================
# ADDING FILES TO INCLUDE PATH
# =============================
# Add files or folders to include path using this
# Example:
#
#    INCLUDES += source/plugin_folder/
#
INCLUDES +=

# =========================================
# INCLUDING PATHs for DOWNLOADED LIBRARIES
# =========================================
# Works the same as the above, but files included in this variable will not
# cause the compile flag warnings to go off. This is very useful when using a
# library from online.
SYSTEM_INCLUDES +=

# ======================
# ADDING COMPILER FLAGS
# ======================
# Add additional compilation flags to your build.
# Example:
#
#    COMMON_FLAGS += -Weverything
#
COMMON_FLAGS +=

# =========================
# IGNORING FILES FROM LINT
# =========================
# List of folder or files that should be excluded from lint analysis
# Example
#
#    LINT_FILTER += source/downloaded_library_file.hpp
#
LINT_FILTER +=

# ==========================
# OVERRIDING OPENOCD CONFIG
# ==========================
# Override the path to the Openocd debugger configuration file
# Example:
#
#    OPENOCD_CONFIG = $(LIBRARY_DIR)/L0_Platform/lpc40xx/sjone.cfg
#
OPENOCD_CONFIG =
