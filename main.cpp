#include "shared/shared.hpp"

os_helper_t* os = &__os_helper;
nt_helper_t* nt = &__nt_helper;
ui_helper_t* ui = &__ui_helper;

// 
// todo
//    - walk thread list and check for win32thread and process
//    - set tcb, then unset in draw loop
// 

auto players( ) {
   auto cmp = [ & ](
      string_t string,
      string_t substring
   ) {
      return !nt->call_fn <uint32_t( __cdecl* )(
         string_t string,
         string_t substring
      )> ( 0x19f2c0 )( string, substring );
   };

   auto manager = *reinterpret_cast <uint64_t*>
      ( nt->m_unity_player + 0x17F8D28 );
   if ( !manager )
      return;

   auto first = *reinterpret_cast <uint64_t*> ( manager + 0x10 );
   auto last = *reinterpret_cast <uint64_t*> ( manager + 0x18 );

   for ( 
      auto i = first; i != last;
      i = *reinterpret_cast <uint64_t*> ( i + 0x8 ) ) {
      if ( !i )
         continue;

      auto ctx = *reinterpret_cast <uint64_t*> ( i + 0x10 ); // deref nullptr
      auto name = *reinterpret_cast <string_t*> ( ctx + 0x60 );

      if ( !cmp( name, "GameWorld" ) )
         continue;

      os->print( "found gameworld at object 0x%llx\n", ctx );
   }
}

auto crosshair( ) {
   ui->draw_line( 400, 200, 500, 300, ui->rgb_white );
   ui->draw_line( 500, 300, 400, 400, ui->rgb_white );
   ui->draw_line( 400, 400, 300, 300, ui->rgb_white );
   ui->draw_line( 300, 300, 400, 200, ui->rgb_white );

   ui->draw_line( 385, 300, 415, 300, ui->rgb_green );
   ui->draw_line( 400, 285, 400, 315, ui->rgb_green );
}

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

   // wait for game
   nt->query_process( "EscapeFromTark", nt->m_dst_pe );
   nt->attach_process( nt->m_dst_pe, nt->m_process_apc );

   // grab images
   nt->query_image( L"UnityPlayer.dll", nt->m_unity_player );
   nt->detach_process( nt->m_process_apc );
   os->print("UnityPlayer.dll at %llx\n", nt->m_unity_player);

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
      ui->gdi_create_brush( ui->rgb_black, ui->m_black_brush);
      ui->gdi_create_brush( ui->rgb_red, ui->m_red_brush);
      ui->gdi_create_brush( ui->rgb_green, ui->m_green_brush );
      ui->gdi_create_brush( ui->rgb_blue, ui->m_blue_brush);

      // draw
      crosshair( );
      //players( );

      // clear brushes
      ui->gdi_delete_object( ui->m_white_brush );
      ui->gdi_delete_object( ui->m_black_brush );
      ui->gdi_delete_object( ui->m_red_brush );
      ui->gdi_delete_object( ui->m_green_brush );
      ui->gdi_delete_object( ui->m_blue_brush );

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
