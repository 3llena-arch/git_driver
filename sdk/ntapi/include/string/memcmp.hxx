#pragma once

namespace nt {
   [[ nodiscard ]]
   const std::uint8_t memcmp(
      const auto string,
      const auto substring,
      const std::size_t size
   ) {
      if ( !string || !substring )
         return 0;

      auto src{ ptr< std::uint8_t* >( string ) };
      auto dst{ ptr< std::uint8_t* >( substring ) };

      for ( auto ctx{ size }; ctx; ctx-- )
         if ( *src++ != *dst++ )
            return 0;

      return 1;
   }
}
