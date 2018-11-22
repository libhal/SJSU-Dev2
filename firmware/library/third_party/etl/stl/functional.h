// SJSU-Dev2: Adding system_header
#pragma GCC system_header


#ifndef ETL_STL_FUNCTIONAL_INCLUDED
#define ETL_STL_FUNCTIONAL_INCLUDED

#include "../platform.h"

#if defined(ETL_NO_STL)
  #include "alternate/functional.h"
#else
  #include <functional>
#endif

#endif
