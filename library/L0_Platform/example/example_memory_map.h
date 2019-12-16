
// 1. Use the include guard that is given by the source code
#pragma once
// 2. Add this to the header file to keep GCC and the code analysis tools from
//    check this file.
#pragma GCC system_header

// 3. Wrap contents of the memory map in the sjsu::<insert_platform_name_here>
//    as to make sure that there are not any name conflicts when developers are
//    building multiplatform applications.
#if defined(__cplusplus)
namespace sjsu::example  // rename to name of platform
{
extern "C"
{
#endif

  // 2. Replace this file with the memory map definitions file for your
  //    microcontroller.

#if defined(__cplusplus)
}  // extern "C"
}  // namespace sjsu::example
#endif
