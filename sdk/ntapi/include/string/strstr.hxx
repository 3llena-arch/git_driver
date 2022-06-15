#pragma once

namespace nt {
   [[ nodiscard ]]
   const std::uint8_t strstr(
      const std::string_t string,
      const std::string_t substring
   ) {
      if ( !string || !substring )
         return 0;

      auto str{ get_strlen( string ) };
      auto sub{ get_strlen( substring ) };

      auto len{ str > sub ? sub : str };
      if ( !len )
         return 0;

      return memcmp( string, substring, len );
   }
}
