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

   enum color_t : uint64_t {
      rbg_black = 0,
      rgb_red = 0xff,
      rgb_green = 0xff00,
      rgb_blue = 0xff0000,
      rgb_white = 0xffffff
   };

   auto ctx = reinterpret_cast <uint64_t( __fastcall* )(
      uint64_t window
   )> ( nt->m_gui_base + 0x3a430 )( 0 );

   auto brush = reinterpret_cast <uint64_t( __fastcall* )(
      uint64_t color,
      uint64_t hbr
   )> ( nt->m_gui_full + 0x12d650 )( color_t::rgb_white, 0 );

   io->print("draw ctx: 0x%llx\n", ctx );
   io->print("brush: 0x%llx\n", brush );

   reinterpret_cast <uint64_t( __fastcall* )(
      uint64_t ctx, 
      uint64_t brush
   )> ( nt->m_gui_full + 0x2a8ba0 )( ctx, brush );


   struct stub_t {
      uint32_t m_left;
      uint32_t m_top;
      uint32_t m_right;
      uint32_t m_bottom;
   };

   auto draw = [ ](
      uint64_t ctx,
      uint64_t left,
      uint32_t top,
      uint32_t right,
      uint32_t bottom
   ) {
      reinterpret_cast <uint64_t( __fastcall* )(
         uint64_t ctx,
         uint64_t left,
         uint32_t top,
         uint32_t right,
         uint32_t bottom,
         uint32_t flag
      )> ( nt->m_gui_full + 0x7cbb0 ) ( ctx, left, 
         top, right, bottom, 0xf00021 );
   };

   auto w_draw = [ & ](
      uint64_t& c,
      uint64_t b,
      stub_t d
   ) {
      reinterpret_cast <uint64_t( __fastcall* )(
         uint64_t ctx, 
         uint64_t brush
      )> ( nt->m_gui_full + 0x2a8ba0 )( c, b );

      draw( c, d.m_left, d.m_top, 1, d.m_bottom - d.m_top );
      draw( c, d.m_right - 1, d.m_top, 1, d.m_bottom - d.m_top );
      draw( c, d.m_left, d.m_top, d.m_right - d.m_left, 1 );
      draw( c, d.m_left, d.m_bottom - 1, d.m_right - d.m_left, 1 );
   };

   uint64_t cycle = 0;
   while (
      true
   ) {
      if ( cycle >= 0x7fffffff ) {
         io->print( "loop...\n" ), cycle = 0;
      }
      cycle++;

      w_draw( ctx, brush, {
         .m_left = 100,
         .m_top = 100,
         .m_right = 200,
         .m_bottom = 200
      });
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
