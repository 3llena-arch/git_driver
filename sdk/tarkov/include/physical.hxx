#pragma once

namespace tk {
   struct physical_t {
      const std::uint8_t set_stamina(
         const std::float_t stamina
      ) {
         auto body_stamina{ read< std::ptrdiff_t >( this + 0x38 ) };
         auto hand_stamina{ read< std::ptrdiff_t >( this + 0x40 ) };

         if ( !body_stamina || !hand_stamina )
            return 0;

         return write< float >( body_stamina + 0x48, stamina )
             && write< float >( hand_stamina + 0x48, stamina );
      }

      const std::uint8_t set_fall_damage(
         const std::float_t fall_damage
      ) {
         return write< std::float_t >( this + 0xb4, fall_damage );
      }
   };
}
