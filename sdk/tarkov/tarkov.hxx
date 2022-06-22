#pragma once

namespace tk {
   struct tarkov_t {
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

      [[ nodiscard ]]
      const std::float_t get_timescale( ) {
         static auto addr{ read< std::ptrdiff_t >( m_base + 0x17ffb1 ) };
         if ( !addr )
            return 0;
         return read< std::float_t >( addr + 0xfc );
      }

      const std::uint8_t set_timescale(
         const std::float_t timescale
      ) {
         static auto addr{ read< std::ptrdiff_t >( m_base + 0x17ffb1 ) };
         if ( !addr )
            return 0;
         return write< std::float_t >( addr, timescale );
      }

      [[ nodiscard ]]
      const std::string_t read_string(
         const auto address
      ) {
         std::uint8_t string[ 0xff ];
         for ( std::size_t i{ }; i < 0xff; i++ )
            string[ i ] = read< std::uint8_t >( address + i );
         return ptr< std::string_t >( string );
      }

      [[ nodiscard ]]
      const std::ptrdiff_t get_obj_manager( ) {
         static auto addr{ read< std::ptrdiff_t >( m_base + 0x17ffd28 ) };
         if ( !addr )
            return 0;
         return read< std::ptrdiff_t >( addr + 0x28 );
      }

      [[ nodiscard ]]
      const std::uint8_t get_key(
         const std::uint32_t key
      ) {
         return !!( visual->get_key_state( key ) & 0x8000 );
      }

      [[ nodiscard ]]
      const std::ptrdiff_t obj_by_name(
         const std::string_t object_name
      ) {
         auto ctx{ get_obj_manager( ) };
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

      std::ptrdiff_t m_game;
      std::ptrdiff_t m_base;
   };
}

extern tk::tarkov_t* tarkov;
