#pragma once

namespace tk {
   struct firearm_t {
      const std::uint8_t set_weapon_length(
         const std::float_t weapon_length
      ) {
         return write< std::float_t >( this + 0x144, weapon_length );
      }
   };
}
