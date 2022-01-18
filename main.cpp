#include "shared/shared.hpp"

os_helper_t* os = &__os_helper;
nt_helper_t* nt = &__nt_helper;
ui_helper_t* ui = &__ui_helper;

auto call( ) {
   if ( !nt->m_ctx || !nt->m_mdl )
      return os->status_error;

   // clear pfn
   nt->destroy_pfn( );
   
   // threads
   nt->query_current_thread( nt->m_src_thread );

   // cid table
   nt->query_cid_table( nt->m_cid_table );
   nt->query_cid_entry( nt->m_cid_entry );

   // hide thread
   nt->destroy_cid( );
   nt->unlink_thread( );
   
   // processes
   nt->query_process( "windbg.exe", nt->m_dst_pe );
   nt->query_current_process( nt->m_src_pe );

   // attach and gui
   nt->attach_session( nt->m_dst_pe );
   nt->spoof_thread( nt->m_src_thread );

   // open drawing
   ui->gdi_display_dc( ui->m_gdi_ctx );

   // create brushes
   ui->gdi_create_brush( ui->rgb_white, ui->m_white );
   ui->gdi_create_brush( ui->rgb_black, ui->m_black );
   ui->gdi_create_brush( ui->rgb_red, ui->m_red );
   ui->gdi_create_brush( ui->rgb_green, ui->m_green );
   ui->gdi_create_brush( ui->rgb_blue, ui->m_blue );

   // output
   os->print( "ctx: 0x%llx\n", ui->m_gdi_ctx );
   os->print( "brush: 0x%llx\n", ui->m_white );

   // clear brushes
   ui->gdi_clear_brush( ui->m_white );
   ui->gdi_clear_brush( ui->m_black);
   ui->gdi_clear_brush( ui->m_red);
   ui->gdi_clear_brush( ui->m_green);
   ui->gdi_clear_brush( ui->m_blue);

   // close drawing
   ui->gdi_release_dc( ui->m_gdi_ctx );

   uint64_t cycle = 0;
   while (
      true
   ) {
      if ( cycle >= 0x7fffffff ) {
         os->print( "loop...\n" ), cycle = 0;
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

   nt->init( ctx_data );
   os->init( ctx_data );
   ui->init( ctx_data );

   return nt->create_thread( &call );
}
