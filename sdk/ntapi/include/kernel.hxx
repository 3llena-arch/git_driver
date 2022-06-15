#pragma once

namespace nt {
   struct kernel_t {
      const std::uint32_t copy_memory(
         const std::ptrdiff_t dst,
         const std::ptrdiff_t src,
         const std::size_t size,
         const std::uint32_t flag,
         const std::size_t* total
      ) {
         static auto ctx{ get_export( m_ntos, "MmCopyMemory" ) };
         if ( !ctx )
             return 0;

         return ptr< std::uint32_t( __stdcall* )(
            const std::ptrdiff_t dst,
            const std::ptrdiff_t src,
            const std::size_t size,
            const std::uint32_t flag,
            const std::size_t* total
         ) >( ctx )( dst, src, size, flag, total );
      }

      const enum mem_t : std::uint8_t { phys = 0x1, virt };

      template< mem_t flag, typename type_t >
      const type_t read(
         const auto address
      ) {
         type_t buffer{ };
         std::size_t read{ };
         copy_memory( ptr< std::ptrdiff_t >( &buffer ),
            ptr< std::ptrdiff_t >( address ), sizeof( type_t ), flag, &read );
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

      [[ nodiscard ]]
      const process_t* get_initial_process( ) {
         static auto ctx{ get_export( m_ntos, "PsInitialSystemProcess" ) };
         if ( !ctx )
            return 0;
         return *ptr< process_t** >( ctx );
      }

      template< typename... arg_t >
      const std::uint32_t msg(
         const std::string_t msg,
         const arg_t... args
      ) {
         static auto ctx{ get_export( m_ntos, "DbgPrintEx" ) };
         if ( !ctx )
            return 0;

         return ptr< std::uint32_t( __stdcall* )(
            const std::int32_t flag,
            const std::int32_t level,
            const std::string_t msg,
            const arg_t... variadic
         ) >( ctx )( 0, 0, msg, args... );
      }

      [[ nodiscard ]]
      const std::ptrdiff_t get_symbol(
         const process_t* process,
         const std::ptrdiff_t image,
         const std::string_t name
      ) {
         // check if file exists at C:\\Symbols\\guid.pdb
         ( process, image, name );
         // convert guid to path string
         return image + 0; // + offset from syms
      }

      [[ nodiscard ]]
      const std::ptrdiff_t get_export(
         const std::ptrdiff_t image,
         const std::string_t name
      ) {
         if ( !image || !get_strlen( name ) )
            return 0;

         auto dos_header{ ptr< dos_header_t* >( image ) };
         auto nt_headers{ ptr< nt_headers_t* >( image + dos_header->m_nt_offset ) };

         if ( dos_header->m_magic != 'ZM'
           || nt_headers->m_magic != 'EP' )
            return 0;

         auto export_dir{ ptr< export_dir_t* >( image + nt_headers->m_eat_table ) };
         if ( !export_dir->m_names
           || !export_dir->m_ptrs
           || !export_dir->m_ords )
            return 0;

         auto names{ ptr< std::uint32_t* >( image + export_dir->m_names ) };
         auto ptrs{ ptr< std::uint32_t* >( image + export_dir->m_ptrs ) };
         auto ords{ ptr< std::uint16_t* >( image + export_dir->m_ords ) };

         for ( std::size_t i{ }; i < export_dir->m_count; i++ ) {
            auto ctx{ ptr< std::string_t >( image + names[ i ] ) };
            if ( !ctx || !get_strlen( ctx ) )
               continue;

            if ( strcmp( name, ctx ) )
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
   };
}
