#pragma once

#include <iostream>

#define DEBUG_OBJ_LIFE

#if !defined(quote)
#define quote(x) #x
#endif

#if !defined(debug_construction)
#if defined(DEBUG_OBJ_LIFE)
#define debug_construction(x) std::cout << "+" << quote(x) << "()" << std::endl
#else
#define debug_construction(x)
#endif
#endif

#if !defined(debug_destruction)
#if defined(DEBUG_OBJ_LIFE)
#define debug_destruction(x) std::cout << "-" << quote(x) << "()" << std::endl
#else
#define debug_destruction(x)
#endif
#endif
