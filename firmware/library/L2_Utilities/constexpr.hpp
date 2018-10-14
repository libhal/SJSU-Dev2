#pragma once

#include <cstdlib>

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
