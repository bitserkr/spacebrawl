#ifndef _COMPILER_H
#define _COMPILER_H

#include <cstddef>
#include <cstdint>
#include <crtdbg.h> // Overloaded new for debugging

#ifndef _CRTDBG_MAP_ALLOC
  #define _CRTDBG_MAP_ALLOC
#endif

#ifdef DIS_DEBUG
  #ifdef new
    #undef new
  #endif
    #define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif // DIS_DEBUG

#endif /* _COMPILER_H */
