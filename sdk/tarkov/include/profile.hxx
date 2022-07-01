#pragma once

namespace tk {
   struct profile_t {
      [[ nodiscard ]]
      const std::int32_t get_user_age( ) {
         auto ctx{ read< std::ptrdiff_t >( this + 0x28 ) };
         if ( !ctx )
            return 0;
         return read< std::int32_t >( ctx + 0x6c );
      }

      [[ nodiscard ]]
      const std::int32_t get_user_side( ) {
         auto ctx{ read< std::ptrdiff_t >( this + 0x28 ) };
         if ( !ctx )
            return 0;
         return read< std::int32_t >( ctx + 0x68 );
      }
   };
}
