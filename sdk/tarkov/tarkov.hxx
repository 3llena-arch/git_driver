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
      const auto address,
      const std::ptrdiff_t* offs,
      const std::size_t length
   ) {
      auto ctx{ read< std::ptrdiff_t >( address + offs[ 0 ] ) };
      if ( !ctx )
         return 0;

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

   const std::uint8_t run_loop( ) {
      return 0;
   }
}
