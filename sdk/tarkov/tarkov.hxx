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

   [[ nodiscard ]]
   const std::float_t get_timescale( ) {
      auto addr{ read< std::ptrdiff_t >( m_base + 0x17ffb18 ) };
      if ( !addr )
         return 0.f;
      return read< std::float_t >( addr + 0xfc );
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
   const vec2_t< std::int32_t >to_screen(
      const matrix_t view_matrix,
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

      vec3_t< std::float_t >t = { view_matrix._14, view_matrix._24, view_matrix._34 };
      vec3_t< std::float_t >r = { view_matrix._11, view_matrix._21, view_matrix._31 };
      vec3_t< std::float_t >u = { view_matrix._12, view_matrix._22, view_matrix._32 };

      std::float_t w{ dot( t, u ) + view_matrix._44 };
      if ( w < 0.1f )
         return { };

      std::float_t x{ dot( r, world ) + view_matrix._41 };
      std::float_t y{ dot( u, world ) + view_matrix._42 };

      return vec2_t< std::int32_t >{
         ptr< std::int32_t >( ( get_screen_size( ).m_x >> 0x1 ) * ( 1.f + x / w ) ),
         ptr< std::int32_t >( ( get_screen_size( ).m_y >> 0x1 ) * ( 1.f - y / w ) )
      };
   }

   const std::uint8_t run_loop( ) {
      auto fps_cam{ obj_by_name< tag >( "FPS Camera" ) };
      if ( !fps_cam )
         return 0;

      std::ptrdiff_t chain[ ] = { 0x30, 0x18, 0xc0 };
      auto matrix{ read_chain< matrix_t >( fps_cam, chain, 3 ) };
      auto screen{ to_screen(matrix, vec3_t< std::float_t >{ 0.f, 0.f, 0.f } ) };

      auto hdc{ visual->get_user_dc( ) };
      if ( !hdc )
         return 0;

      visual->draw_line( hdc, 0, 0, screen.m_x, screen.m_y, visual->rgb( 255, 255, 255 ) );
      visual->release_dc( hdc );
      return 1;
   }
}
