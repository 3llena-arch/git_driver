#pragma once

namespace nt {
   struct kernel_t {
      const enum mem_flag_t : std::uint8_t {
         phys = 0x00000001,
         virt = 0x00000002
      };

      const std::uint32_t copy_memory(
         const auto address,
         const auto buffer,
         const std::size_t size,
         const std::uint32_t flag
      ) {
         auto src{ ptr< std::ptrdiff_t >( address ) };
         auto dst{ ptr< std::ptrdiff_t >( buffer ) };
         if ( !src || !dst )
            return 0;

         std::size_t total{ };

         ptr< std::uint32_t( __stdcall* )(
            const std::ptrdiff_t dst,
            const std::ptrdiff_t src,
            const std::size_t size,
            const std::uint32_t flag,
            const std::size_t* total
         ) >( m_copy )( dst, src, size, flag, &total );

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
      const std::ptrdiff_t to_phys(
         const process_t* process,
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

         auto cr3{ ( process->m_dir_table ? process->m_dir_table
            : process->m_user_dir_table ) & 0xfffffffffffffff0 };
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

      const std::uint32_t close(
         const auto handle
      ) {
         auto ctx{ ptr< std::ptrdiff_t >( handle ) };
         if ( !ctx || ctx == -1 )
            return 0;

         static auto addr{ get_export( m_ntos, "ZwClose" ) };
         if ( !addr )
            return 0;

         ptr< std::uint32_t( __stdcall* )(
            const std::ptrdiff_t handle
         ) >( addr )( handle );

         return 1;
      }

      [[ nodiscard ]]
      const std::ptrdiff_t get_symbol(
         const std::ptrdiff_t image,
         const std::string_t name
      ) {
         auto ctx{ ptr< pdb_record_t* >( m_syms ) };
         if ( !ctx )
            return 0;

         do {
            if ( ctx->m_ptr == 'BOZO' )
               break;

            if ( !strcmp( ctx->m_name, name ) ) {
               auto dos_header{ read< virt, dos_header_t >( image ) };
               auto nt_headers{ read< virt, nt_headers_t >( image + dos_header.m_nt_offset ) };

               if ( dos_header.m_magic != 'ZM'
                 || nt_headers.m_magic != 'EP' )
                  return 0;

               auto seg{ ptr< std::uint16_t >( ctx->m_seg ) - 1 };
               if ( seg > nt_headers.m_sections )
                  return 0;

               auto ofs{ image + dos_header.m_nt_offset + sizeof( nt_headers_t ) };
               auto sec{ read< virt, nt_section_t >( ofs + ( seg * sizeof( nt_section_t ) ) ) };

               if ( !ofs || !sec.m_ptr )
                  return 0;

               return image + sec.m_ptr + ctx->m_ptr;
            }
         } while ( ctx++ );
         return 0;
      }

      [[ nodiscard ]]
      const process_t* get_system( ) {
         static auto addr{ get_export( m_ntos, "PsInitialSystemProcess" ) };
         if ( !addr )
            return 0;
         return read< virt, process_t* >( addr );
      }

      template< typename... arg_t >
      const std::uint32_t msg(
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
         const std::string_t name
      ) {
         if ( !image || !strlen( name ) )
            return 0;

         auto dos_header{ read< virt, dos_header_t >( image ) };
         auto nt_headers{ read< virt, nt_headers_t >( image + dos_header.m_nt_offset ) };

         if ( dos_header.m_magic != 'ZM'
           || nt_headers.m_magic != 'EP' )
            return 0;

         auto export_dir{ read< virt, export_dir_t >( image + nt_headers.m_eat_table ) };
         if ( !export_dir.m_names
           || !export_dir.m_ptrs
           || !export_dir.m_ords )
            return 0;

         auto names{ ptr< std::uint32_t* >( image + export_dir.m_names ) };
         auto ptrs{ ptr< std::uint32_t* >( image + export_dir.m_ptrs ) };
         auto ords{ ptr< std::uint16_t* >( image + export_dir.m_ords ) };

         for ( std::size_t i{ }; i < export_dir.m_count; i++ ) {
            auto ctx{ ptr< std::string_t >( image + names[ i ] ) };
            if ( !ctx || !strlen( ctx ) )
               continue;

            if ( !strcmp( name, ctx ) )
               return image + ptrs[ ords[ i ] ];
         }
         return 0;
      }

      [[ nodiscard ]]
      const pdb_table_t get_pdb_info(
         const process_t* process,
         const std::ptrdiff_t image
      ) {
         auto dos_header{ read< phys, dos_header_t >( to_phys( process, image ) ) };
         auto nt_headers{ read< phys, nt_headers_t >( to_phys( process, image + dos_header.m_nt_offset ) ) };

         if ( dos_header.m_magic != 'ZM'
           || nt_headers.m_magic != 'EP' )
            return { };

         auto debug_dir{ read< phys, debug_dir_t >( to_phys( process, image + nt_headers.m_dbg_table ) ) };
         auto debug_pdb{ read< phys, pdb_table_t >( to_phys( process, image + debug_dir.m_ptr ) ) };

         if ( debug_pdb.m_magic != 'SR' )
            return { };

         return debug_pdb;
      }

      const std::ptrdiff_t m_ntos;
      const std::ptrdiff_t m_pmdl;
      const std::ptrdiff_t m_syms;
      const std::ptrdiff_t m_copy;
   };
}
