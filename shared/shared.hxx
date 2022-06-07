#pragma once

#pragma comment( linker, "/merge:.pdata=.rdata" )
#pragma comment( linker, "/merge:.rdata=.text" )
#pragma comment( linker, "/merge:.data=.text" )
#pragma comment( linker, "/merge:INIT=.text" )

namespace std {
   using ptrdiff_t = unsigned long long;
   using size_t = unsigned long long;

   using wstring_t = wchar_t*;
   using string_t = char*;
   
   using uint64_t = unsigned long long;
   using uint32_t = unsigned int;
   using uint16_t = unsigned short;
   using uint8_t = unsigned char;

   using int64_t = long long;
   using int32_t = int;
   using int16_t = short;
   using int8_t = char;
}

#ifdef __ptr
   template< typename type_t = std::ptrdiff_t >
   type_t ptr( auto addr ) { return ( type_t )addr; };
#endif

#include "types/vector.hxx"
#include "classes/kernel.hxx"
#include "classes/visual.hxx"
