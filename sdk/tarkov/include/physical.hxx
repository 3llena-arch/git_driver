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

         return write< std::float_t >( body_stamina + 0x48, stamina )
             && write< std::float_t >( hand_stamina + 0x48, stamina );
      }
   };
}
