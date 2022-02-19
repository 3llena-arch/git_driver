#include "shared/shared.hpp"

os_helper_t* os = &__os_helper;
nt_helper_t* nt = &__nt_helper;
ui_helper_t* ui = &__ui_helper;

auto loop( ) {
   if ( !nt->m_ctx || !nt->m_mdl )
      return;

   auto object_manager = nt->read <uint64_t> 
      ( nt->m_unity_player + 0x17F8D28 );
   if ( !object_manager )
      return;

   /*
      stuff
   */
}

auto call( ) {
   if ( !nt->m_ctx || !nt->m_mdl )
      return os->status_error;

   // security
   nt->destroy_pfn( );
   nt->query_current_thread( nt->m_src_thread );

   // clear cid
   nt->query_cid_table( nt->m_cid_table );
   nt->query_cid_entry( nt->m_cid_entry );

   // unlink
   nt->destroy_cid( );
   nt->unlink_thread( );

   // wait for game
   nt->query_process( "EscapeFromTark", nt->m_dst_pe );
   nt->attach_process( nt->m_dst_pe, nt->m_process_apc );

   // grab images
   nt->query_image( "UnityPlayer.dll", nt->m_unity_player );
   nt->detach_process( nt->m_process_apc );

   // wait for gui
   nt->query_process( "dwm.exe", nt->m_gui_pe );
   nt->query_current_process( nt->m_src_pe );

   for (
      ;;
   ) {
      // attach
      nt->attach_process( nt->m_dst_pe, nt->m_process_apc );

      // draw
      loop( );

      // detach
      nt->detach_process( nt->m_process_apc );

      // attach
      nt->attach_session( nt->m_gui_pe, nt->m_session_apc );
      nt->spoof_thread( nt->m_src_thread );

      // open drawing
      ui->gdi_display_dc( ui->m_gdi_ctx );

      // create brushes
      ui->gdi_create_brush( ui->rgb_white, ui->m_white_brush);
      ui->gdi_create_brush( ui->rgb_green, ui->m_green_brush );

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
