#pragma once

namespace tk {
   struct weapon_t {
      const std::uint8_t set_recoil_scale(
         const std::float_t recoil_scale
      ) {
         auto ctx{ read< std::ptrdiff_t >( this + 0x48 ) };
         if ( !ctx )
            return 0;
         return write< std::float_t >( ctx + 0x70, recoil_scale );
      }

      const std::uint8_t set_aim_speed(
         const std::float_t aim_speed
      ) {
         return write< std::float_t >( this + )
      }
   };
}
