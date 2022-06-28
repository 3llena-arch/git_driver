#pragma once

#include "include/types/matrix.hxx"
#include "include/types/vector.hxx"

namespace tk {
   static std::ptrdiff_t m_game;
   static std::ptrdiff_t m_base;

   const enum obj_flag_t : std::uint8_t {
      act = 0x00000000,
      tag = 0x00000001
   };

   template< typename type_t >
   const type_t read(
      const auto address
   ) {
      auto addr{ kernel->to_phys( m_game, address ) };
      auto read{ kernel->read< kernel->phys, type_t >( addr ) };

      return read;
   }

   template< typename type_t >
   const std::uint8_t write(
      const auto address,
      const type_t value
   ) {
      auto addr{ kernel->to_phys( m_game, address ) };
      auto read{ kernel->write< kernel->phys, type_t >( addr, value ) };

      return read;
   }

   template< typename type_t >
   const type_t read_chain(
      const std::ptrdiff_t address,
      const std::ptrdiff_t* offs,
      const std::size_t length
   ) {
      auto ctx{ read< std::ptrdiff_t >( address + offs[ 0 ] ) };
      if ( !ctx )
         return { };

      for ( std::size_t i{ 1 }; i < length - 1; i++ )
         ctx = read< std::ptrdiff_t >( ctx + offs[ i ] );

      return read< type_t >( ctx + offs[ length - 1 ] );
   }

   const std::uint8_t get_key(
      const std::int32_t key
   ) {
      return !!( visual->get_key_state( key ) & 0x8000 );
   }

   const std::uint8_t set_timescale(
      const std::float_t timescale
   ) {
      auto addr{ read< std::ptrdiff_t >( m_base + 0x17ffb18 ) };
      if ( !addr )
         return 0;
      return write< std::float_t >( addr + 0xfc, timescale );
   }

   const std::uint8_t set_weapon_anim_mask(
      const std::ptrdiff_t player,
      const std::int32_t mask
   ) {
      auto addr{ read< std::ptrdiff_t >( player + 0x198 ) };
      if ( !addr )
         return 0;
      return write< std::int32_t >( addr + 0x100, mask );
   }

   [[ nodiscard ]]
   const std::uint8_t is_local(
      const std::ptrdiff_t player
   ) {
      return !!read< std::uint8_t >( player + 0x7f7 );
   }

   [[ nodiscard ]]
   const std::uint8_t is_dead(
      const std::ptrdiff_t player
   ) {
      return !!read< std::uint8_t >( player + 0x6d0 );
   }

   [[ nodiscard ]]
   const std::float_t get_timescale( ) {
      auto addr{ read< std::ptrdiff_t >( m_base + 0x17ffb18 ) };
      if ( !addr )
         return 0;
      return read< std::float_t >( addr + 0xfc );
   }

   [[ nodiscard ]]
   const std::string_t read_string(
      const auto address
   ) {
      std::int8_t string[ 0xff ]{ };
      for ( auto i = 0; i < 0xff; i++ )
         string[ i ] = read< std::int8_t >( address + i );
      return string;
   }

   template< std::uint32_t flag >
   const std::ptrdiff_t get_manager( ) {
      auto addr{ read< std::ptrdiff_t >( m_base + 0x17ffd28 ) };
      if ( !addr )
         return 0;

      if ( flag == act ) return read< std::ptrdiff_t >( addr + 0x28 );
      if ( flag == tag ) return read< std::ptrdiff_t >( addr + 0x18 );
      return 0;
   }

   template< std::uint32_t flag >
   const std::ptrdiff_t obj_by_name(
      const std::string_t object_name
   ) {
      auto ctx{ get_manager< flag >( ) };
      if ( !ctx )
         return 0;

      static auto next{ 0x08 };
      static auto base{ 0x10 };
      static auto name{ 0x60 };

      while ( ctx = read< std::ptrdiff_t >( ctx + next ), ctx ) {

         auto obj{ read< std::ptrdiff_t >( ctx + base ) };
         auto buf{ read< std::ptrdiff_t >( obj + name ) };

         if ( !obj || !buf )
            continue;

         if ( !nt::strcmp( read_string( buf ), object_name ) )
            return obj;
      }
      return 0;
   }

   [[ nodiscard ]]
   const vec2_t< std::int32_t >get_screen_size( ) {
      auto hdc{ visual->get_user_dc( ) };
      auto wnd{ visual->get_dc_wnd( hdc ) };

      if ( !hdc || !wnd )
         return { };

      vec2_t< std::int32_t > out{
         visual->get_wnd_rect( wnd ).m_right,
         visual->get_wnd_rect( wnd ).m_bottom
      };

      visual->release_dc( hdc );
      return out;
   }

   [[ nodiscard ]]
   const matrix_t get_view_matrix( ) {
      auto cam{ obj_by_name< tag >( "FPS Camera" ) };
      if ( !cam )
         return { };

      const std::ptrdiff_t chain[ ] = {
         0x00000030,
         0x00000018,
         0x000002e4
      };

      return read_chain< matrix_t >( cam, chain, 3 );
   }

   const std::uint8_t set_physical_stamina(
      const std::ptrdiff_t player,
      const std::float_t stamina
   ) {
      auto addr{ read< std::ptrdiff_t >( player + 0x4f0 ) };
      if ( !addr )
         return 0;

      auto run{ read< std::ptrdiff_t >( addr + 0x38 ) };
      auto aim{ read< std::ptrdiff_t >( addr + 0x40 ) };

      return write< std::float_t >( run + 0x48, 100.f )
          && write< std::float_t >( aim + 0x48, 100.f );
   }

   [[ nodiscard ]]
   const std::ptrdiff_t get_game_world( ) {
      auto world{ obj_by_name< act >( "GameWorld" ) };
      if ( !world )
         return 0;

      const std::ptrdiff_t chain[ ] = {
         0x00000030,
         0x00000018,
         0x00000028
      };

      return read_chain< std::ptrdiff_t >( world, chain, 3 );
   }

   [[ nodiscard ]]
   const vec2_t< std::int32_t >to_screen(
      const vec3_t< std::float_t >world
   ) {
      const auto dot = [ ](
         vec3_t< std::float_t >src,
         vec3_t< std::float_t >dst
      ) {
         return ( src.m_x * dst.m_x )
              + ( src.m_y * dst.m_y )
              + ( src.m_z * dst.m_z );
      };

      auto view_matrix{ get_view_matrix( ) };
      if ( !&view_matrix )
         return { };

      vec3_t< std::float_t >t = { view_matrix._14, view_matrix._24, view_matrix._34 };
      vec3_t< std::float_t >r = { view_matrix._11, view_matrix._21, view_matrix._31 };
      vec3_t< std::float_t >u = { view_matrix._12, view_matrix._22, view_matrix._32 };

      std::float_t w{ dot( t, world ) + view_matrix._44 };
      if ( w < 0.098f )
         return { };

      std::float_t x{ dot( r, world ) + view_matrix._41 };
      std::float_t y{ dot( u, world ) + view_matrix._42 };

      vec2_t< std::int32_t > out{
         ptr< std::int32_t >( ( get_screen_size( ).m_x >> 0x1 ) * ( 1.f + x / w ) ),
         ptr< std::int32_t >( ( get_screen_size( ).m_y >> 0x1 ) * ( 1.f - y / w ) )
      };

      if ( out.m_x > get_screen_size( ).m_x ) out.m_x = get_screen_size( ).m_x;
      if ( out.m_y > get_screen_size( ).m_y ) out.m_y = get_screen_size( ).m_y;

      if ( out.m_x < 0 ) out.m_x = 0;
      if ( out.m_y < 0 ) out.m_y = 0;

      return out;
   }

   const std::uint8_t set_view_angles(
      const std::ptrdiff_t player,
      const vec3_t< std::float_t >angles
   ) {
      auto addr{ read< std::ptrdiff_t >( player + 0x40 ) };
      if ( !addr )
         return 0;

      return write< std::float_t >( addr + 0x22C, angles.m_x )
          && write< std::float_t >( addr + 0x230, angles.m_y );
   }

   const void run_loop( ) {
      if ( !get_timescale( ) )
         return;

      auto world{ get_game_world( ) };
      if ( !world )
         return;

      auto list{ read< std::ptrdiff_t >( world + 0x88 ) };
      auto base{ read< std::ptrdiff_t >( list + 0x10 ) };
      auto size{ read< std::int32_t >( list + 0x18 ) };

      auto hdc{ visual->get_user_dc( ) };
      if ( !hdc )
         return;

      for ( std::size_t i{ }; i < size; i++ ) {
         auto ctx{ read< std::ptrdiff_t >( base + 0x20 + ( i * 0x8 ) ) };
         if ( !ctx )
            continue;

         if ( is_local( ctx ) ) {
            set_weapon_anim_mask( ctx, 0 );
            set_physical_stamina( ctx, 100.f );
         }

         if ( !is_local( ctx ) && !is_dead( ctx ) ) {
            auto pedo{ read< std::ptrdiff_t >( ctx + 0x48 ) };
            if ( !pedo )
               continue;

            auto screen{ to_screen( read< vec3_t< std::float_t > >( pedo + 0x30 ) ) };
            if ( !screen.m_x
              || !screen.m_y )
               continue;

            auto center_x{ get_screen_size( ).m_x >> 0x1 };
            auto center_y{ get_screen_size( ).m_y >> 0x1 };

            visual->draw_line( hdc, center_x, center_y,
               screen.m_x, screen.m_y, visual->rgb( 255, 255, 255 ) );
         }
      }

      set_timescale( 1.8f );

      visual->invalidate_wnd( visual->get_dc_wnd( hdc ) );
      visual->release_dc( hdc );
   }
}
