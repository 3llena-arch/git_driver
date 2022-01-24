#include "shared/shared.hpp"

os_helper_t* os = &__os_helper;
nt_helper_t* nt = &__nt_helper;
ui_helper_t* ui = &__ui_helper;

// todo
// - add wcstombs in nt->query_image

auto draw( ) {
   if ( !nt->m_ctx || !nt->m_mdl )
      return os->status_error;

   // enum entlist and shit

   return os->status_okay;
}

auto call( ) {
   if ( !nt->m_ctx || !nt->m_mdl )
      return os->status_error;

   // clear pfn
   nt->destroy_pfn( );
   nt->query_current_thread( nt->m_src_thread );

   // cid table
   nt->query_cid_table( nt->m_cid_table );
   nt->query_cid_entry( nt->m_cid_entry );

   // hide thread
   nt->destroy_cid( );
   nt->unlink_thread( );

   // wait for game
   nt->query_process( "r5apex.exe", nt->m_dst_pe );
   nt->attach_process( nt->m_dst_pe, nt->m_process_apc );

   // grab base
   nt->query_image( "r5apex.exe", nt->m_test_image );
   nt->detach_process( nt->m_process_apc );

   // wait for gui
   nt->query_process( "dwm.exe", nt->m_gui_pe );
   nt->query_current_process( nt->m_src_pe );

   for (
      ;;
   ) {
      // attach
      nt->attach_session( nt->m_gui_pe, nt->m_session_apc );
      nt->spoof_thread( nt->m_src_thread );

      // open drawing
      ui->gdi_display_dc( ui->m_gdi_ctx );

      // create brushes
      ui->gdi_create_brush( ui->rgb_white, ui->m_white_brush);
      ui->gdi_create_brush( ui->rgb_green, ui->m_green_brush );

      // draw
      draw( );

      // clear brushes
      ui->gdi_delete_object( ui->m_white_brush );
      ui->gdi_delete_object( ui->m_green_brush );

      // close drawing
      ui->gdi_release_dc( ui->m_gdi_ctx );

      // reset thread
      nt->unspoof_thread( nt->m_src_thread );
      nt->detach_session( nt->m_gui_pe, nt->m_session_apc );
   }
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
