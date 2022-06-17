#pragma once

namespace nt {
   struct kernel_t {
      const enum mem_flag_t : std::uint8_t {
         phys = 0x00000001,
         virt = 0x00000002
      };

      const std::uint8_t copy_memory(
         const auto address,
         const auto buffer,
         const std::size_t size,
         const std::uint32_t flag
      ) {
         static auto addr{ get_export( m_ntos, "MmCopyMemory" ) };
         if ( !addr )
            return 0;

         auto src{ ptr< std::ptrdiff_t >( address ) };
         auto dst{ ptr< std::ptrdiff_t >( buffer ) };

         ptr< std::uint32_t( __stdcall* )(
            const std::ptrdiff_t dst,
            const std::ptrdiff_t src,
            const std::size_t size,
            const std::uint32_t flag,
            const std::size_t* total
         ) >( addr )( dst, src, size, flag, &src );

         return 1;
      }

      template< std::uint32_t flag, typename type_t >
      const type_t read(
         const auto address
      ) {
         type_t buffer{ };
         copy_memory( address, &buffer, sizeof( type_t ), flag );
         return buffer;
      }

      [[ nodiscard ]]
      const std::ptrdiff_t get_cr3(
         const std::ptrdiff_t process
      ) {
         auto dir_base{ *ptr< std::ptrdiff_t* >( process + 0x028 ) };
         auto usr_base{ *ptr< std::ptrdiff_t* >( process + 0x280 ) };

         return dir_base ? dir_base : usr_base;
      }

      [[ nodiscard ]]
      const std::ptrdiff_t to_phys(
         const std::ptrdiff_t process,
         const auto address
      ) {
         auto addr{ ptr< std::ptrdiff_t >( address ) };
         if ( !addr )
            return 0;

         const std::ptrdiff_t page[ ] = {
            ( addr << 0x10 ) >> 0x37,
            ( addr << 0x19 ) >> 0x37,
            ( addr << 0x22 ) >> 0x37,
            ( addr << 0x2b ) >> 0x37,
            ( addr << 0x34 ) >> 0x34
         };

         enum id_t : std::uint8_t { 
            dir_ptr, // page directory pointer
            dir,     // page directory
            tab_ptr, // page table
            tab,     // page table entry
            ofs      // page offset
         };

         auto cr3{ get_cr3( process ) & 0xfffffffffffffff0 };
         if ( !cr3 )
            return 0;

         const std::ptrdiff_t data[ ] = {
            read< phys, std::ptrdiff_t >( cr3 + 8 * page[ dir_ptr ] ),
            read< phys, std::ptrdiff_t >( ( data[ 0 ] & 0xffffff000 ) + 8 * page[ dir ] ),
            read< phys, std::ptrdiff_t >( ( data[ 1 ] & 0xffffff000 ) + 8 * page[ tab_ptr ] ),
            read< phys, std::ptrdiff_t >( ( data[ 2 ] & 0xffffff000 ) + 8 * page[ tab ] )
         };

         if ( data[ 1 ] & 0x80 ) return ( data[ 1 ] & 0xfffffc0000000 ) + ( addr & 0x3fffffffll );
         if ( data[ 2 ] & 0x80 ) return ( data[ 2 ] & 0x0000ffffff000 ) + ( addr & 0x001fffffll );
         return ( data[ 3 ] & 0x0000ffffff000 ) + page[ ofs ];
      }

      template< typename... arg_t >
      const std::uint8_t msg(
         const std::string_t msg,
         const arg_t... args
      ) {
         static auto addr{ get_export( m_ntos, "DbgPrintEx" ) };
         if ( !addr )
            return 0;

         ptr< std::uint32_t( __stdcall* )(
            const std::int32_t flag,
            const std::int32_t level,
            const std::string_t msg,
            const arg_t... variadic
         ) >( addr )( 0, 0, msg, args... );

         return 1;
      }

      [[ nodiscard ]]
      const std::ptrdiff_t get_export(
         const std::ptrdiff_t image,
         const std::string_t symbol
      ) {
         auto dos_header{ ptr< dos_header_t* >( image ) };
         auto nt_headers{ ptr< nt_headers_t* >( image + dos_header->m_offset ) };
         if ( dos_header->m_magic != 'ZM'
           || nt_headers->m_magic != 'EP' )
            return 0;

         auto export_dir{ ptr< export_dir_t* >( image + nt_headers->m_exports ) };
         if ( !export_dir->m_names
           || !export_dir->m_ptrs
           || !export_dir->m_ords )
            return 0;

         auto name{ ptr< std::uint32_t* >( image + export_dir->m_names ) };
         auto ptrs{ ptr< std::uint32_t* >( image + export_dir->m_ptrs ) };
         auto ords{ ptr< std::uint16_t* >( image + export_dir->m_ords ) };

         for ( std::size_t i{ }; i < export_dir->m_count; i++ )
            if ( !strcmp( symbol, ptr< std::string_t >( image + name[ i ] ) ) )
               return image + ptrs[ ords[ i ] ];
      }

      const std::ptrdiff_t m_ntos;
      const std::ptrdiff_t m_pmdl;
   };
}
