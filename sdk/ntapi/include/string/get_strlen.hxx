#pragma once

namespace nt {
   [[ nodiscard ]]
   const std::size_t get_strlen(
      const std::string_t string
   ) {
      std::size_t size{ };
      for ( auto ctx{ string }; *ctx != '\0'; ctx++, size++ );
      return size;
   }
}
