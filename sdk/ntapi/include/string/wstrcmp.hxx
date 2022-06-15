#pragma once

namespace nt {
   [[ nodiscard ]]
   const std::uint8_t wstrcmp(
      const std::wstring_t string,
      const std::wstring_t substring
   ) {
      auto len{ get_wstrlen( string ) * sizeof( wchar_t ) };
      if ( !len )
         return 0;

      return memcmp( string, substring, len );
   }
}
