#pragma once

namespace nt {
   [[ nodiscard ]]
   const std::int32_t wcscmp(
      const std::wstring_t string,
      const std::wstring_t substring
   ) {
      auto src{ ptr< std::wstring_t >( string ) };
      auto dst{ ptr< std::wstring_t >( substring ) };

      while ( *src && *src == *dst ) src++, dst++;
      return *src - *dst;
   }
}
