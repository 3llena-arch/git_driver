#pragma once

namespace tk {
   struct player_t {
      [[ nodiscard ]]
      const std::uint8_t is_local( ) {
         return !!read< std::uint8_t >( this + 0x807 );
      }

      [[ nodiscard ]]
      const std::uint8_t is_dead( ) {
         return !!read< std::uint8_t >( this + 0x6e0 );
      }

      [[ nodiscard ]]
      physical_t* get_physical( ) {
         return read< physical_t* >( this + 0x500 );
      }

      [[ nodiscard ]]
      profile_t* get_profile( ) {
         return read< profile_t* >( this + 0x4f0 );
      }

      [[ nodiscard ]]
      weapon_t* get_weapon( ) {
         return read< weapon_t* >( this + 0x198 );
      }

      [[ nodiscard ]]
      movement_t* get_movement( ) {
         return read< movement_t* >( this + 0x40 );
      }

      [[ nodiscard ]]
      const std::uint8_t is_player( ) {
         return !!get_profile( )->get_user_age( );
      }
   };
}
