#pragma once

struct nt_helper_t {
   uint64_t m_mdl;
   uint64_t m_ctx;

   uint64_t m_gui_base;
   uint64_t m_gui_full;

   uint64_t m_unity_player;
   uint64_t m_object_manager;

   uint64_t m_src_thread;
   uint64_t m_gui_thread;

   uint64_t m_gui_pe;
   uint64_t m_src_pe;
   uint64_t m_dst_pe;

   uint64_t m_cid_table;
   uint64_t m_cid_entry;

   struct apc_stub_t {
      uint8_t m_pad1[ 0x30 ];
   };

   struct list_stub_t {
      list_stub_t* m_fwd;
      list_stub_t* m_bck;
   };

   struct thread_stub_t {
      uint8_t m_pad1[ 0x2f8 ];
      list_stub_t m_list1;
      uint8_t m_pad2[ 0x3b0 ];
      list_stub_t m_list2;
   };

   apc_stub_t m_session_apc;
   apc_stub_t m_process_apc;

   auto init(
      data_t* data
   ) {
      m_mdl = data->m_memory;
      m_ctx = data->m_kernel;

      m_gui_base = data->m_gui_base;
      m_gui_full = data->m_gui_full;
   }

   template <typename type_t>
   auto call_fn(
      uint64_t address
   ) {
      return reinterpret_cast <type_t> 
         ( m_ctx + address );
   }

   auto destroy_pfn(
      uint32_t mdl_value = 0
   ) {
      if ( !m_ctx || !m_mdl )
         return os->status_error;

      auto mdl_store = reinterpret_cast <uint64_t*> 
         ( m_mdl ) + 0x1;
      if ( !mdl_store )
         return os->status_error;

      for ( auto i = 0; i < 0x10; i++ ) {
         call_fn <uint64_t( __cdecl* )(
            uint64_t* dst,
            uint32_t* src,
            uint64_t bytes
         )> ( 0x1d4040 )( &mdl_store[ i ], &mdl_value, 0x4 );
      }
      return os->status_okay;
   }

   auto create_thread(
      auto call,
      uint64_t handle = 0
   ) {
      auto cast = reinterpret_cast <uint64_t> ( call );
      if ( !m_ctx )
         return os->status_error;

      call_fn <uint32_t( __stdcall* )(
         uint64_t* thread,
         uint32_t access,
         uint64_t attributes,
         uint64_t process,
         uint64_t client,
         uint64_t routine,
         uint64_t context
      )> ( 0x6eb360 )( &handle, 0, 0, 0, 0, cast, 0 );

      if ( !handle )
         return os->status_warning;

      call_fn <uint32_t( __stdcall* )(
         uint64_t handle
      )> ( 0x1bd9d0 )( handle );
      
      return os->status_okay;
   }

   auto query_image(
      string_t name,
      uint64_t& image
   ) {
      if ( !m_ctx || !m_dst_pe )
         return os->status_error;

      auto len = [ & ](
         wstring_t string
      ) {
         return call_fn <uint64_t( __cdecl* )(
            wstring_t string
         )> ( 0x19fba0 )( string );
      };

      auto cmp = [ & ](
         string_t string,
         string_t substring
      ) {
         return !call_fn <uint32_t( __cdecl* )(
            string_t string,
            string_t substring
         )> ( 0x19d710 )( string, substring );
      };
      
      while ( 
         !image 
      ) {
         auto peb = *reinterpret_cast <uint64_t*>
            ( m_dst_pe + 0x3f8 );
         if ( !peb )
            continue;

         auto ldr = *reinterpret_cast <uint64_t*>
            ( peb + 0x18 );
         if ( !ldr )
            continue;

         auto ctx = **reinterpret_cast <uint64_t**>
            ( ldr + 0x10 );
         if ( !ctx )
            continue;

         do {
            if ( !ctx )
               continue;

            auto entry = *reinterpret_cast <wstring_t*>
               ( ctx + 0x60 );
            if ( !entry )
               continue;

            char file[ 0x100 ];

            if ( !std::memset( file, 0, sizeof( file ) ) )
               continue;

            for ( auto i = 0; entry[ i ] != '\0'; i++ )
               file[ i ] = entry[ i ];

            if ( !cmp( file, name ) )
               continue;

            image = *reinterpret_cast <uint64_t*> 
               ( ctx + 0x30 );

            os->print("--+ captured %s at 0x%llx\n",
               file, image );
            break;

         } while ( ctx = *reinterpret_cast 
            <uint64_t*> ( ctx ) );
      }

      return os->status_okay;
   }

   auto copy_virtual(
      uint64_t src_process,
      uint64_t src_address,
      uint64_t dst_process,
      uint64_t dst_address,
      uint64_t size,
      uint64_t read = 0
   ) {
      if ( !m_ctx )
         return os->status_error;

      call_fn <uint32_t( __fastcall* )(
         uint64_t src_process,
         uint64_t src_address,
         uint64_t dst_process,
         uint64_t dst_address,
         uint64_t size,
         uint8_t mode,
         uint64_t* read
      )> ( 0x6221c0 )( src_process, src_address,
         dst_process, dst_address, 0, size, &read );

      return os->status_okay;
   }

   auto query_process(
      string_t name,
      uint64_t& process,
      uint64_t idle = 0,
      uint64_t ctx = 0
   ) {
      if ( !m_ctx || !name )
         return os->status_error;

      while ( idle < 0x7fffffff )
         idle++;

      auto cmp = [ & ](
         string_t string,
         string_t substring
      ) {
         return !call_fn <uint32_t( __cdecl* )(
            string_t string,
            string_t substring
         )> ( 0x19d710 )( string, substring );
      };

      while (
         !process
      ) {
         for ( auto i = 0x4; i < 0x7fff; i += 0x4 ) {
            call_fn <uint32_t( __stdcall* )( 
               uint64_t id,
               uint64_t* process
            )> ( 0x61c520 )( i, &ctx );

            if ( !ctx )
               continue;

            auto file = reinterpret_cast <string_t> 
               ( ctx + 0x450 );

            if ( !cmp( file, name ) )
               continue;

            process = ctx;
            
            os->print("--+ captured %s at 0x%llx\n",
               file, process );
            break;
         }
      }
      
      return os->status_okay;
   }

   auto spoof_thread(
      uint64_t thread
   ) {
      if ( !m_ctx || !m_gui_pe )
         return os->status_error;

      auto ctx = *reinterpret_cast <uint64_t*>
         ( m_gui_pe + 0x30 ) - 0x2f8;
      if ( !ctx )
         return os->status_error;

      m_gui_thread = ctx;

      auto field = [ & ](
         uint64_t address
      ) {
         auto src = reinterpret_cast <uint64_t*> 
            ( thread + address );

         auto dst = reinterpret_cast <uint64_t*> 
            ( m_gui_thread + address );

         *src = *dst;
      };
      
      // cid
      field( 0x648 );
      field( 0x650 );

      // tcb
      field( 0x1c8 );
      field( 0x220 );

      return os->status_okay;
   }

   auto unspoof_thread(
      uint64_t thread
   ) {
      if ( !m_ctx || !m_gui_pe )
         return os->status_error;

      auto write = [ & ](
         uint64_t address,
         uint64_t value
      ) {
         auto src = reinterpret_cast <uint64_t*> 
            ( thread + address );

         *src = value;
      };

      // cid
      write( 0x648, 0);
      write( 0x650, 0);

      // tcb
      write( 0x1c8, 0 );
      write( 0x220, m_ctx );
      
      return os->status_okay;
   }

   auto attach_process(
      uint64_t process,
      apc_stub_t apc
   ) {
      if ( !m_ctx )
         return os->status_error;

      call_fn <void( __stdcall* )(
         uint64_t process,
         apc_stub_t* apc
      )> ( 0x67c50 )( process, &apc );

      return os->status_okay;
   }

   auto detach_process(
      apc_stub_t apc
   ) {
      if ( !m_ctx )
         return os->status_error;

      call_fn <void( __stdcall* )(
         apc_stub_t* apc
      )> ( 0x35430 )( &apc );

      return os->status_okay;
   }

   auto attach_session(
      uint64_t process,
      apc_stub_t apc,
      uint64_t session = 0,
      uint32_t id = 0
   ) {
      if ( !m_ctx )
         return os->status_error;

      id = call_fn <uint64_t( __fastcall* )(
         uint64_t process
      )> ( 0x3a270 )( process );

      session = call_fn <uint64_t( __fastcall* )(
         uint32_t id
      )> ( 0x23f0 )( id );

      call_fn <uint64_t( __fastcall* )(
         uint64_t session,
         apc_stub_t* state
      )> ( 0xe9e00 )( session, &apc );

      return os->status_okay;
   }

   auto detach_session(
      uint64_t process,
      apc_stub_t& apc,
      uint64_t session = 0,
      uint32_t id = 0
   ) {
      if ( !m_ctx )
         return os->status_error;

      id = call_fn <uint64_t( __fastcall* )(
         uint64_t process
      )> ( 0x3a270 )( process );

      session = call_fn <uint64_t( __fastcall* )(
         uint32_t id
      )> ( 0x23f0 )( id );

      call_fn <uint64_t( __fastcall* )( 
         uint64_t session,
         apc_stub_t* state
      )> ( 0xe9d60 )( session, &apc );

      apc = { 0 };
      return os->status_okay;
   }

   auto query_current_process(
      uint64_t& process
   ) {
      if ( !m_ctx )
         return os->status_error;

      process = reinterpret_cast <uint64_t( __stdcall* )( )>
         ( m_ctx + 0x67de0 )( );

      return os->status_okay;
   }

   auto query_current_thread(
      uint64_t& thread
   ) {
      if ( !m_ctx )
         return os->status_error;

      thread = reinterpret_cast <uint64_t( __stdcall* )( )>
         ( m_ctx + 0x31e40 )( );

      return os->status_okay;
   }

   auto query_cid_table(
      uint64_t& table
   ) {
      if ( !m_ctx )
         return os->status_error;

      table = *reinterpret_cast <uint64_t*> 
         ( m_ctx + 0x574530 );
      
      return os->status_okay;
   }

   auto query_cid_entry(
      uint64_t& entry
   ) {
      if ( !m_ctx || !m_src_thread || !m_cid_table )
         return os->status_error;

      auto id = *reinterpret_cast <uint64_t*>
         ( m_src_thread + 0x650 );
      if ( !id )
         return os->status_error;

      entry = call_fn <uint64_t( __fastcall* )(
         uint64_t table,
         uint64_t id
      )> ( 0x5f13e0 )( m_cid_table, id );

      return os->status_okay;
   }

   auto destroy_cid( ) {
      if ( !m_ctx || !m_cid_table || !m_cid_entry )
         return os->status_error;

      auto id = *reinterpret_cast <uint64_t*>
         ( m_src_thread + 0x650 );
      if ( !id )
         return os->status_error;

      call_fn <uint64_t( __fastcall* )(
         uint64_t table,
         uint64_t id,
         uint64_t entry
      )> ( 0x660360 )( m_cid_table, id, m_cid_entry );

      return os->status_okay;
   }

   auto unlink_thread( ) {
      if ( !m_ctx || !m_src_thread )
         return os->status_error;

      auto field = [ & ](
         uint64_t address 
      ) { 
         return *reinterpret_cast <uint64_t*>
            ( m_src_thread + address ) = 0;
      };

      auto misc = *reinterpret_cast <uint32_t*> 
         ( m_src_thread + 0x74 );
      if ( !misc )
         return os->status_error;

      misc &= (0ul << 0x4);
      misc &= (0ul << 0xe);
      misc &= (0ul << 0xa);
      
      field( 0x30 );
      field( 0x38 );
      field( 0x58 );

      field( 0x6a0 );
      field( 0x620 );
      field( 0x648 );
      field( 0x650 );
      field( 0x724 );

      auto ctx = reinterpret_cast <thread_stub_t*>
         ( m_src_thread );
      if ( !ctx )
         return os->status_error;

      ctx->m_list1.m_bck->m_fwd = ctx->m_list1.m_fwd;
      ctx->m_list1.m_fwd->m_bck = ctx->m_list1.m_bck;
      ctx->m_list2.m_bck->m_fwd = ctx->m_list2.m_fwd;
      ctx->m_list2.m_fwd->m_bck = ctx->m_list2.m_bck;

      ctx->m_list1.m_fwd = &ctx->m_list1;
      ctx->m_list1.m_bck = &ctx->m_list1;
      ctx->m_list2.m_fwd = &ctx->m_list2;
      ctx->m_list2.m_bck = &ctx->m_list2;

      return os->status_okay;
   }

   template <typename type_t>
   auto read(
      uint64_t address
   ) {
      type_t buffer{};
      copy_virtual( m_dst_pe, address, m_src_pe, reinterpret_cast 
         <uint64_t> ( &buffer ), sizeof( type_t ) );
      return buffer;
   }

   template <typename type_t>
   auto write(
      uint64_t address,
      type_t buffer
   ) {
      copy_virtual( m_src_pe, reinterpret_cast <uint64_t> ( buffer ), 
         m_dst_pe, address, sizeof( type_t ) );
   }
} __nt_helper;

extern nt_helper_t* nt;
