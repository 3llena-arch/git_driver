#pragma once

namespace nt {
   [[ nodiscard ]]
   const std::int32_t strcmp(
      const std::string_t string,
      const std::string_t substring
   ) {
      auto src{ ptr< std::string_t >( string ) };
      auto dst{ ptr< std::string_t >( substring ) };

      while ( *src && *src == *dst ) src++, dst++;
      return *src - *dst;
   }
}
