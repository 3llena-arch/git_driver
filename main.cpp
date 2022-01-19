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

   for (
      ;;
   ) {
      // open drawing
      ui->gdi_display_dc( ui->m_gdi_ctx );
      ui->gdi_compatible_dc( ui->m_gdi_ctx, ui->m_gdi_src );

      // create bitmap
      ui->gdi_compatible_bitmap( ui->m_gdi_ctx, 800, 600,
         ui->m_gdi_bitmap );

      // select bitmap
      ui->gdi_select_bitmap( ui->m_gdi_src, ui->m_gdi_bitmap );

      // create brushes
      ui->gdi_create_brush( ui->rgb_white, ui->m_white_brush);
      ui->gdi_create_brush( ui->rgb_black, ui->m_black_brush);
      ui->gdi_create_brush( ui->rgb_red, ui->m_red_brush);
      ui->gdi_create_brush( ui->rgb_green, ui->m_green_brush );
      ui->gdi_create_brush( ui->rgb_blue, ui->m_blue_brush);

      // create pens
      ui->gdi_create_pen( ui->rgb_white, ui->m_white_pen );
      ui->gdi_create_pen( ui->rgb_black, ui->m_black_pen );
      ui->gdi_create_pen( ui->rgb_red, ui->m_red_pen );
      ui->gdi_create_pen( ui->rgb_green, ui->m_green_pen );
      ui->gdi_create_pen( ui->rgb_blue, ui->m_blue_pen );

      // draw
      //ui->draw_box( ui->m_white_brush, 300, 300, 500, 500 );
      ui->draw_line( ui->m_white_pen, 0, 0, 200, 200 );

      // copy buffer
      ui->gdi_bit_blt( ui->m_gdi_ctx, 0, 0, 800, 600,
         ui->m_gdi_src, 0, 0, ui->rgb_black );

      // delete bitmap
      ui->gdi_delete_object( ui->m_gdi_bitmap );

      // clear pens
      ui->gdi_delete_object( ui->m_white_pen );
      ui->gdi_delete_object( ui->m_black_pen );
      ui->gdi_delete_object( ui->m_red_pen );
      ui->gdi_delete_object( ui->m_green_pen );
      ui->gdi_delete_object( ui->m_blue_pen );

      // clear brushes
      ui->gdi_delete_object( ui->m_white_brush );
      ui->gdi_delete_object( ui->m_black_brush );
      ui->gdi_delete_object( ui->m_red_brush );
      ui->gdi_delete_object( ui->m_green_brush );
      ui->gdi_delete_object( ui->m_blue_brush );

      // close drawing
      ui->gdi_release_dc( ui->m_gdi_src );
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
