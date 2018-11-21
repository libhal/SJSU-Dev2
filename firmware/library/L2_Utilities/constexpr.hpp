#pragma once

#include <cstdlib>

// Constructing this structure with a file path, will generate a structure with
// a character array containing just the bare basename.
template <size_t kPathLength, size_t kBasenameLength>
struct FileBasename_t
{
  constexpr explicit FileBasename_t(const char (&path)[kPathLength])
      : basename{}
  {
    size_t base_position = 0;
    for (size_t i = kPathLength - kBasenameLength; i < kPathLength; i++)
    {
      basename[base_position] = path[i];
      base_position++;
    }
    basename[base_position - 1] = '\0';
  }
  char basename[kBasenameLength];
};

constexpr size_t StringLength(const char * str)
{
  size_t result = 0;
  for (result = 0; str[result] != '\0'; result++)
  {
    continue;
  }
  return result + 1;
}

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
