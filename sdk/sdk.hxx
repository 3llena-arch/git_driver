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

   using double_t = double;
   using float_t = float;
}

#ifdef __ptr
   template< typename type_t >
   type_t ptr( auto obj ) { return ( type_t )obj; }
#endif

extern "C" std::int32_t _fltused( ) { return 0; }
extern "C" std::int32_t __chkstk( ) { return 0; }

#include "ntapi/ntapi.hxx"
#include "tarkov/tarkov.hxx"
