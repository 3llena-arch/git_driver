#pragma once

struct nt_helper_t {
   uint64_t m_mdl;
   uint64_t m_ctx;

   uint64_t m_src_thread;
   uint64_t m_gui_thread;
   
   uint64_t m_src_pe;
   uint64_t m_dst_pe;

   uint64_t m_cid_table;
   uint64_t m_cid_entry;

   auto init(
      uint64_t ctx_memory,
      uint64_t ctx_kernel
   ) {
      m_mdl = ctx_memory;
      m_ctx = ctx_kernel;
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

   auto query_gui_thread(
      uint64_t& thread
   ) {
      // todo winlogon.exe
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

      auto misc = *reinterpret_cast <uint32_t*> 
         ( m_src_thread + 0x74 );
      if ( !misc )
         return os->status_error;

      // misc flags
      misc &= (0ul << 0x4);
      misc &= (0ul << 0xe);
      misc &= (0ul << 0xa);

      auto clear = [ & ](
         uint64_t address 
      ) { 
         return *reinterpret_cast <uint64_t*>
            ( m_src_thread + address ) = 0;
      };

      // start address
      clear( 0x6a0 );
      clear( 0x620 );

      // client id
      clear( 0x648 );
      clear( 0x650 );

      struct entry_t {
         entry_t* m_f;
         entry_t* m_b;
      };

      struct stub_t {
         uint8_t m_pad[ 0x6b8 ];
         entry_t m_list;
      };

      auto ctx = reinterpret_cast <stub_t*>
         ( m_src_thread );
      if ( !ctx )
         return os->status_error;

      ctx->m_list.m_b->m_f = ctx->m_list.m_f;
      ctx->m_list.m_f->m_b = ctx->m_list.m_b;

      ctx->m_list.m_f = &ctx->m_list;
      ctx->m_list.m_b = &ctx->m_list;

      return os->status_okay;
   }
} __nt_helper;

extern nt_helper_t* nt;
