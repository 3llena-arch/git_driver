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
         return write< std::float_t >( this + 0x198, aim_speed );
      }

      const std::uint8_t set_anim_mask(
         const std::int32_t anim_mask
      ) {
         return write< std::int32_t >( this + 0x100, anim_mask );
      }
      
      firearm_t* get_firearm( ) {
         return read< firearm_t* >( this + 0x80 );
      }

      const std::uint8_t set_should_retract(
         const std::uint8_t should_retract
      ) {
         return write< std::uint8_t >( this + 0x17b, should_retract );
      }
   };
}
