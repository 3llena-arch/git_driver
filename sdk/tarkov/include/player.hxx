#pragma once

namespace tk {
   struct player_t {
      [[ nodiscard ]]
      const std::ptrdiff_t get_bone_matrix( ) {
         const std::ptrdiff_t chain[ ] = {
            0x000000a8, // player body
            0x00000028, // skeleton root
            0x00000028, // values
            0x00000010  // object
         };

         return read_chain< std::ptrdiff_t >( this, chain, 4 );
      }

      [[ nodiscard ]]
      const vec3_t< std::float_t > get_root_pos( ) {
         auto matrix{ get_bone_matrix( ) };
         if ( !matrix )
            return { };

         const std::ptrdiff_t chain[ ] = {
            0x00000020, // transform
            0x00000010, // object
            0x00000038  // vec3
         };

         auto bone{ read_chain< std::ptrdiff_t >( matrix, chain, 3 ) };
         if ( !bone )
            return { };

         return read< vec3_t< std::float_t > >( bone + 0x90 );
      }

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
