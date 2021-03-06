#pragma once

namespace nt {
   [[ nodiscard ]]
   const std::size_t wcslen(
      const std::wstring_t string
   ) {
      std::size_t size{ };
      for ( auto ctx{ string }; *ctx != L'\0'; ctx++, size++ );
      return size;
   }
}
