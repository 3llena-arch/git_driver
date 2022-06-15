#pragma once

namespace nt {
   [[ nodiscard ]]
   const std::uint8_t strcmp(
      const std::string_t string,
      const std::string_t substring
   ) {
      auto len{ get_strlen( string ) };
      if ( !len )
         return 0;

      return memcmp( string, substring, len );
   }
}
