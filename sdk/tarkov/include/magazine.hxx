#pragma once

namespace tk {
   struct magazine_t {
      const std::uint8_t set_reload_rate(
         const std::float_t reload_rate
      ) {
         auto v1{ read< std::ptrdiff_t >( this + 0x170 ) };
         auto v2{ read< std::ptrdiff_t >( this + 0x178 ) };
         auto v3{ read< std::ptrdiff_t >( this + 0x198 ) };

         return write< std::float_t >( v1 + 0x28, reload_rate )
             && write< std::float_t >( v2 + 0x28, reload_rate )
             && write< std::float_t >( v3 + 0x28, reload_rate );
      }
   };
}
