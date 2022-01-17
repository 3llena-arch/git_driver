#include "shared/shared.hpp"

os_helper_t* os = &__os_helper;
io_helper_t* io = &__io_helper;
nt_helper_t* nt = &__nt_helper;

auto call( ) {
   if ( !nt->m_ctx || !nt->m_mdl )
      return os->status_error;

   // clear pfn
   nt->destroy_pfn( );
   
   // threads
   nt->query_current_thread( nt->m_src_thread );
   nt->query_gui_thread( nt->m_gui_thread );

   // cid table
   nt->query_cid_table( nt->m_cid_table );
   nt->query_cid_entry( nt->m_cid_entry );

   // hide thread
   nt->destroy_cid( );
   nt->unlink_thread( );
   
   // processes
   nt->query_process( "windbg.exe", nt->m_dst_pe );
   nt->query_current_process( nt->m_src_pe );

   // attach process
   nt->attach_session( nt->m_dst_pe );
   
   // get dc


   uint64_t cycle = 0;
   while (
      true
   ) {
      if ( cycle >= 0x7fffffff ) {
			io->print( "loop...\n" ), cycle = 0;
		}
		cycle++;
   }

   return os->status_okay;
}

auto main(
   data_t* ctx_data
) -> uint32_t {
   if ( !ctx_data )
      return os->status_error;

   io->init( ctx_data );
   nt->init( ctx_data );

   return nt->create_thread( &call );
}
