#include <cstdint>
#include <cstring>

#include "L4_Testing/testing_frameworks.hpp"
#include "utility/fatfs.hpp"

namespace sjsu
{
TEST_CASE("Testing FatFS Utility", "[fatfs-utility]")
{
  SECTION("Stringify")
  {
    CHECK(0 == strcmp(Stringify(FR_OK), "(0) Succeeded"));
    CHECK(0 ==
          strcmp(Stringify(FR_DISK_ERR),
                 "(1) A hard error occurred in the low level disk I/O layer"));
    CHECK(0 == strcmp(Stringify(FR_INT_ERR), "(2) Assertion failed"));
    CHECK(0 == strcmp(Stringify(FR_NOT_READY),
                      "(3) The physical drive cannot work"));
    CHECK(0 == strcmp(Stringify(FR_NO_FILE), "(4) Could not find the file"));
    CHECK(0 == strcmp(Stringify(FR_NO_PATH), "(5) Could not find the path"));
    CHECK(0 == strcmp(Stringify(FR_INVALID_NAME),
                      "(6) The path name format is invalid"));
    CHECK(
        0 ==
        strcmp(Stringify(FR_DENIED),
               "(7) Access denied due to prohibited access or directory full"));
    CHECK(0 == strcmp(Stringify(FR_EXIST),
                      "(8) Access denied due to prohibited access"));
    CHECK(0 == strcmp(Stringify(FR_INVALID_OBJECT),
                      "(9) The file/directory object is invalid"));
    CHECK(0 == strcmp(Stringify(FR_WRITE_PROTECTED),
                      "(10) The physical drive is write protected"));
    CHECK(0 == strcmp(Stringify(FR_INVALID_DRIVE),
                      "(11) The logical drive number is invalid"));
    CHECK(0 == strcmp(Stringify(FR_NOT_ENABLED),
                      "(12) The volume has no work area"));
    CHECK(0 == strcmp(Stringify(FR_NO_FILESYSTEM),
                      "(13) There is no valid FAT volume"));
    CHECK(0 == strcmp(Stringify(FR_MKFS_ABORTED),
                      "(14) The f_mkfs() aborted due to any problem"));
    CHECK(0 == strcmp(Stringify(FR_TIMEOUT),
                      "(15) Could not get a grant to access the volume within "
                      "defined period"));
    CHECK(0 == strcmp(Stringify(FR_LOCKED),
                      "(16) The operation is rejected according to the file "
                      "sharing policy"));
    CHECK(0 == strcmp(Stringify(FR_NOT_ENOUGH_CORE),
                      "(17) LFN working buffer could not be allocated"));
    CHECK(0 == strcmp(Stringify(FR_TOO_MANY_OPEN_FILES),
                      "(18) Number of open files > FF_FS_LOCK"));
    CHECK(0 == strcmp(Stringify(FR_INVALID_PARAMETER),
                      "(19) Given parameter is invalid"));
  }
}
}  // namespace sjsu
