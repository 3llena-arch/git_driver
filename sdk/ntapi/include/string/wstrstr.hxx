#pragma once

namespace nt {
   [[ nodiscard ]]
   const std::uint8_t wstrstr(
      const std::wstring_t string,
      const std::wstring_t substring
   ) {
      if ( !string || !substring )
         return 0;

      auto str{ get_wstrlen( string ) * sizeof( wchar_t ) };
      auto sub{ get_wstrlen( substring ) * sizeof( wchar_t ) };

      auto len{ str > sub ? sub : str };
      if ( !len )
         return 0;

      return memcmp( string, substring, len );
   }
}
