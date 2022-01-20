#pragma once

struct nt_helper_t {
   uint64_t m_mdl;
   uint64_t m_ctx;

   uint64_t m_gui_base;
   uint64_t m_gui_full;

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

   struct thread_stub_t {
      uint8_t m_pad1[ 0x6b8 ];
      struct list_t {
         list_t* m_fwd;
         list_t* m_bck;
      } m_list;
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
      wstring_t name,
      uint64_t process,
      uint64_t& image,
      uint64_t peb = 0,
      uint64_t ldr = 0,
      uint64_t ctx = 0
   ) {
      if ( !m_ctx || !process )
         return os->status_error;

      auto cmp = [ & ](
         wstring_t string,
         wstring_t substring
      ) {
         return !call_fn <uint32_t( __cdecl* )(
            wstring_t string,
            wstring_t substring
         )> ( 0x19fb10 )( string, substring );
      };

      peb = *reinterpret_cast <uint64_t*>
         ( process + 0x3f8 );
      if ( !peb )
         return os->status_error;

      ldr = *reinterpret_cast <uint64_t*>
         ( peb + 0x18 );
      if ( !ldr )
         return os->status_error;

      ctx = **reinterpret_cast <uint64_t**>
         ( ldr + 0x10 );
      if ( !ctx )
         return os->status_error;

      do {
         if ( !ctx )
            continue;

         wstring_t file = *reinterpret_cast <wstring_t*>
            ( ctx + 0x60 );

         os->print( "found %s\n", file );

         if ( !cmp( file, name ) )
            continue;

         image = *reinterpret_cast <uint64_t*> 
            ( ctx + 0x30 );
         break;

      } while ( ctx = *reinterpret_cast 
         <uint64_t*> ( ctx ) );

      return os->status_okay;
   }

   auto query_process(
      string_t name,
      uint64_t& process,
      uint64_t ctx = 0
   ) {
      if ( !m_ctx || !name )
         return os->status_error;

      auto cmp = [ & ](
         string_t string,
         string_t substring
      ) {
         return !call_fn <uint32_t( __cdecl* )(
            string_t string,
            string_t substring
         )> ( 0x19f2c0 )( string, substring );
      };

      while ( !process ) {
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

      // tcb
      field( 0x1c8 );
      field( 0x220 );

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

      // misc flags
      misc &= (0ul << 0x4);
      misc &= (0ul << 0xe);
      misc &= (0ul << 0xa);

      // start address
      field( 0x6a0 );
      field( 0x620 );

      // client id
      field( 0x648 );
      field( 0x650 );

      auto ctx = reinterpret_cast <thread_stub_t*>
         ( m_src_thread );
      if ( !ctx )
         return os->status_error;

      // skip entry
      ctx->m_list.m_bck->m_fwd = ctx->m_list.m_fwd;
      ctx->m_list.m_fwd->m_bck = ctx->m_list.m_bck;

      // safety
      ctx->m_list.m_fwd = &ctx->m_list;
      ctx->m_list.m_bck = &ctx->m_list;

      return os->status_okay;
   }
} __nt_helper;

extern nt_helper_t* nt;
