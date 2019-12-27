#pragma once

#include <cstdlib>

namespace sjsu
{
/// @param str - a string to have its length computed.
/// @return The length of the input string.
constexpr size_t StringLength(const char * str)
{
  size_t result = 0;
  for (result = 0; str[result] != '\0'; result++)
  {
    continue;
  }
  return result + 1;
}

/// @param path - path to calculate the basename length for.
/// @return the number of characters from the end of a string that contains the
///         basename.
constexpr size_t BasenameLength(const char * path)
{
  size_t forward_slash = 0;
  size_t length        = StringLength(path);
  for (forward_slash = 0; forward_slash < length; forward_slash++)
  {
    if (path[(length - 1) - forward_slash] == '/')
    {
      break;
    }
  }
  return forward_slash;
}

/// Returns a pointer to path's basename in the basename pointer.
/// If one is not found then the whole path will be returned.
/// Can only be used on string literals.
///
/// @tparam kPathLength - length of the full string path
/// @param path - path to find the base name of.
template <size_t kPathLength>
constexpr const char * FileBasename(const char (&path)[kPathLength])
{
  const size_t kBasenameLength = BasenameLength(path);
  const char * basename        = &path[kPathLength - (kBasenameLength - 1)];
  return basename;
}

/// Returns a pointer to path's basename in the basename pointer.
/// If one is not found then the whole path will be returned.
/// This implementation, if used outside of a constexpr context will occur at
/// runtime.
///
/// @param path - path to find the base name of.
constexpr const char * FileBasename(const char * path)
{
  const size_t kBasenameLength = BasenameLength(path);
  const size_t kPathLength     = StringLength(path);
  const char * basename        = &path[kPathLength - kBasenameLength];
  return basename;
}
}  // namespace sjsu
