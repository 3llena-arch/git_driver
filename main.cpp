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
   nt->query_process( "dwm.exe", nt->m_dst_pe );
   nt->query_current_process( nt->m_src_pe );

   // attach and gui
   nt->attach_session( nt->m_dst_pe );
   nt->spoof_thread( nt->m_src_thread );

   for (
      ;;
   ) {
      // open drawing
      ui->gdi_display_dc( ui->m_gdi_ctx );

      // create brushes
      ui->gdi_create_brush( ui->rgb_white, ui->m_white_brush);
      ui->gdi_create_brush( ui->rgb_black, ui->m_black_brush);
      ui->gdi_create_brush( ui->rgb_red, ui->m_red_brush);
      ui->gdi_create_brush( ui->rgb_green, ui->m_green_brush );
      ui->gdi_create_brush( ui->rgb_blue, ui->m_blue_brush);

      //
      // todo
      // - SetTextColor
      // - ExtTextOutW
      // In the draw loop, we should really be attaching
      // setting the win32thread and kprocess, drawing
      // then unsetting and detaching to avoid unecesarry
      // overhead in the stolen context.
      // 

      ui->draw_line( 0, 0, 800, 600, ui->rgb_white );
      ui->draw_line( 800, 0, 0, 600, ui->rgb_white );
      ui->draw_line( 0, 300, 800, 300, ui->rgb_white );
      ui->draw_line( 400, 0, 400, 600, ui->rgb_white );

      // clear brushes
      ui->gdi_delete_object( ui->m_white_brush );
      ui->gdi_delete_object( ui->m_black_brush );
      ui->gdi_delete_object( ui->m_red_brush );
      ui->gdi_delete_object( ui->m_green_brush );
      ui->gdi_delete_object( ui->m_blue_brush );

      // close drawing
      ui->gdi_release_dc( ui->m_gdi_ctx );
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
