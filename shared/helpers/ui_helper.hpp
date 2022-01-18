#pragma once

struct ui_helper_t {
   uint64_t m_gui_base;
   uint64_t m_gui_full;

   uint64_t m_gdi_ctx;

   uint64_t m_white;
   uint64_t m_black;
   uint64_t m_red;
   uint64_t m_green;
   uint64_t m_blue;

   auto init(
      data_t* data
   ) {
      m_gui_base = data->m_gui_base;
      m_gui_full = data->m_gui_full;
   }

   enum gui_call_t : uint64_t {
      gui_base = 0,
      gui_full
   };

   enum color_t : uint64_t {
      rgb_black = 0,
      rgb_red = 0xff,
      rgb_green = 0xff00,
      rgb_blue = 0xff0000,
      rgb_white = 0xffffff
   };

   template <typename type_t>
   auto call_fn(
      gui_call_t call,
      uint64_t address
   ) {
      return reinterpret_cast <type_t> 
         ( call ? m_gui_full : m_gui_base + address );
   }

   auto gdi_display_dc(
      uint64_t& ctx
   ) {
      if ( !m_gui_base )
         return os->status_error;

      ctx = call_fn <uint64_t( __fastcall* )(
         uint64_t window
      )> ( gui_base, 0x3a430 )( 0 );

      return os->status_okay;
   }

   auto gdi_clear_brush(
      uint64_t& brush
   ) {
      if ( !m_gui_base || !brush )
         return os->status_error;

      call_fn <uint64_t( __fastcall* )(
         uint64_t brush
      )> ( gui_base, 0x35440 )( brush );

      brush = 0;
      return os->status_okay;
   }

   auto gdi_release_dc(
      uint64_t& ctx
   ) {
      if ( !m_gui_base || !ctx )
         return os->status_error;

      call_fn < uint64_t( __fastcall* )(
         uint64_t ctx
      )> ( gui_base, 0x3a6c0 )( ctx );

      ctx = 0;
      return os->status_okay;
   }

   auto gdi_create_brush(
      color_t color,
      uint64_t& brush
   ) {
      if ( !m_gui_full )
         return os->status_error;

      brush = call_fn <uint64_t( __fastcall* )(
         uint32_t color,
         uint32_t misc,
         uint64_t brush,
         uint32_t flag
      )> ( gui_base, 0xb1c0 )( color, 0, 0, 1 );

      return os->status_okay;
   }

   auto gdi_pat_blt(
      uint64_t ctx,
      uint32_t left,
      uint32_t top,
      uint32_t right,
      uint32_t bottom
   ) {
      if ( !m_gui_full || !ctx )
         return os->status_error;

      call_fn <uint64_t( __fastcall* )(
         uint64_t ctx,
         uint64_t left,
         uint32_t top,
         uint32_t right,
         uint32_t bottom,
         uint32_t flag
      )> ( gui_full, 0x7cbb0 )( ctx, left, top, 
         right, bottom, 0xf00021 );

      return os->status_okay;
   }
} __ui_helper;

extern ui_helper_t* ui;
