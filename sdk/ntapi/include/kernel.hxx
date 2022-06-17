#pragma once

namespace nt {
   struct kernel_t {
      const enum win_ver_t : std::uint32_t {
         build_19h2 = 0x000047bb,
         build_21h2 = 0x000055f0
      };

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
      const std::ptrdiff_t get_system( ) {
         return read< virt, std::ptrdiff_t >( 
            get_export( m_ntos, "PsInitialSystemProcess" ) );
      }

      [[ nodiscard ]]
      const std::uint32_t diff(
         const std::uint32_t win10,
         const std::uint32_t win11
      ) {
         if ( get_winver( ) == build_19h2 ) return win10;
         if ( get_winver( ) == build_21h2 ) return win11;
         return 0;
      }

      [[ nodiscard ]]
      const std::ptrdiff_t get_cr3(
         const std::ptrdiff_t process
      ) {
         auto dir_base{ read< virt, std::ptrdiff_t >( process + 0x28 ) };
         auto usr_base{ read< virt, std::ptrdiff_t >( process + diff( 0x280, 0x388 ) ) };

         return dir_base ? dir_base : usr_base;
      }

      [[ nodiscard ]]
      const std::uint32_t get_winver( ) {
         static auto addr{ get_export( m_ntos, "RtlGetVersion" ) };
         if ( !addr )
            return 0;
         nt_version_t version{ };

         ptr< std::int32_t( __stdcall* )( 
            const nt_version_t* version ) >( addr )( &version );
         return version.m_build;
      }

      [[ nodiscard ]]
      const std::uint8_t clean_mdl_pfn( ) {
         auto mdl{ ptr< mdl_page_t* >( m_pmdl ) };
         if ( !mdl->m_ptr
           || !mdl->m_len )
            return 0;

         auto ratio{ ( mdl->m_ptr + mdl->m_begin ) & 0xfff };
         auto count{ ( ratio + mdl->m_len + 0xfff ) >> 0xc };

         for ( std::size_t i{ }; i < count; i++ )
            ptr< std::uint32_t* >( mdl + 1 )[ i ] = 0;

         return 1;
      }

      const std::uint8_t delete_lookaside(
         const lookaside_t* lookaside
      ) {
         static auto addr{ get_export( m_ntos, "ExDeleteLookasideListEx" ) };
         if ( !addr )
            return 0;

         ptr< std::int32_t( __stdcall* )(
            const lookaside_t* ) >( addr )( lookaside );
         return 1;
      }

      const std::uint8_t create_lookaside(
         const lookaside_t* lookaside
      ) {
         static auto addr{ get_export( m_ntos, "ExInitializeLookasideListEx" ) };
         if ( !addr )
            return 0;

         return !ptr< std::int32_t( __stdcall* )(
            const lookaside_t* lookaside,
            const std::ptrdiff_t alloc_fn,
            const std::ptrdiff_t free_fn,
            const std::uint32_t pool_type,
            const std::uint32_t flags,
            const std::size_t size,
            const std::uint32_t tag,
            const std::uint16_t depth
         ) >( addr )( lookaside, 0, 0, lookaside->m_type, 0,
            lookaside->m_size, lookaside->m_tag, 0 );
      }

      [[ nodiscard ]]
      const std::uint8_t clean_bigpool( ) {
         auto list{ ptr< lookaside_t* >( m_cint + diff( 0x36380, 0x31480 ) ) };
         if ( !list->m_size
           || !list->m_type )
            return 0;

         if ( !delete_lookaside( list )
           || !create_lookaside( list ) )
            return 0;

         return 1;
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

         return !ptr< std::uint32_t( __stdcall* )(
            const std::int32_t flag,
            const std::int32_t level,
            const std::string_t msg,
            const arg_t... variadic
         ) >( addr )( 0, 0, msg, args... );
      }

      const std::uint8_t close(
         const std::ptrdiff_t handle
      ) {
         static auto addr{ get_export( m_ntos, "ZwClose" ) };
         if ( !addr )
            return 0;

         return !ptr< std::int32_t( __stdcall* )(
            const std::ptrdiff_t handle ) >( addr )( handle );
      }

      [[ nodiscard ]]
      const std::ptrdiff_t new_thread(
         const auto callback
      ) {
         static auto addr{ get_export( m_ntos, "PsCreateSystemThreadEx" ) };
         static auto call{ ptr< std::ptrdiff_t >( callback ) };

         if ( !addr || !call )
            return 0;

         std::ptrdiff_t handle{ };

         ptr< std::int32_t( __stdcall* )(
            std::ptrdiff_t* thread,
            std::int32_t access,
            std::ptrdiff_t attributes,
            std::ptrdiff_t process,
            std::ptrdiff_t client,
            std::ptrdiff_t routine,
            std::ptrdiff_t context,
            std::ptrdiff_t unused,
            std::ptrdiff_t reserved
         ) >( addr )( &handle, 0, 0, 0, 0, call, 0, 0, 0 );

         return handle;
      }

      const std::uint8_t stack_attach(
         const std::ptrdiff_t process,
         const std::int8_t* apc_state
      ) {
         static auto addr{ get_export( m_ntos, "KeStackAttachProcess" ) };
         if ( !addr )
            return 0;

         ptr< std::int32_t( __stdcall* )(
            const std::ptrdiff_t process,
            const std::int8_t* apc_state
         ) >( addr )( process, apc_state );

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

         return 0;
      }

      const std::ptrdiff_t m_ntos;
      const std::ptrdiff_t m_pmdl;
      const std::ptrdiff_t m_cint;
   };
}

extern nt::kernel_t* kernel;
