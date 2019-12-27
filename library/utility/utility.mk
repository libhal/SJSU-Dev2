INCLUDES +=
SYSTEM_INCLUDES +=
SOURCES +=

TESTS += $(LIBRARY_DIR)/utility/test/allocator_test.cpp
TESTS += $(LIBRARY_DIR)/utility/test/bit_test.cpp
TESTS += $(LIBRARY_DIR)/utility/test/build_info_test.cpp
TESTS += $(LIBRARY_DIR)/utility/test/constexpr_test.cpp
TESTS += $(LIBRARY_DIR)/utility/test/crc_test.cpp
TESTS += $(LIBRARY_DIR)/utility/test/debug_test.cpp
TESTS += $(LIBRARY_DIR)/utility/test/enum_test.cpp
TESTS += $(LIBRARY_DIR)/utility/test/fatfs_test.cpp
TESTS += $(LIBRARY_DIR)/utility/test/infrared_algorithms_test.cpp
TESTS += $(LIBRARY_DIR)/utility/test/map_test.cpp
TESTS += $(LIBRARY_DIR)/utility/test/rtos_test.cpp
TESTS += $(LIBRARY_DIR)/utility/test/status_test.cpp
TESTS += $(LIBRARY_DIR)/utility/test/stopwatch_test.cpp
TESTS += $(LIBRARY_DIR)/utility/test/time_test.cpp
TESTS += $(LIBRARY_DIR)/utility/math/test/average_test.cpp
TESTS += $(LIBRARY_DIR)/utility/containers/test/string_test.cpp
TESTS += $(LIBRARY_DIR)/utility/containers/test/vector_test.cpp
TESTS += $(LIBRARY_DIR)/utility/containers/test/list_test.cpp
TESTS += $(LIBRARY_DIR)/utility/containers/test/deque_test.cpp

NO_TEST_NEEDED += $(LIBRARY_DIR)/utility/ansi_terminal_codes.hpp
NO_TEST_NEEDED += $(LIBRARY_DIR)/utility/macros.hpp
NO_TEST_NEEDED += $(LIBRARY_DIR)/utility/log.hpp
NO_TEST_NEEDED += $(LIBRARY_DIR)/utility/units.hpp
