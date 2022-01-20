#pragma once

struct ui_helper_t {
   uint64_t m_gui_base;
   uint64_t m_gui_full;

   uint64_t m_gdi_ctx;

   uint64_t m_white_brush;
   uint64_t m_black_brush;
   uint64_t m_red_brush;
   uint64_t m_green_brush;
   uint64_t m_blue_brush;

   uint64_t m_system_font;

   auto init(
      data_t* data
   ) {
      m_gui_base = data->m_gui_base;
      m_gui_full = data->m_gui_full;
   }

   enum gui_call_t : uint64_t {
      gui_base,
      gui_full
   };

   enum font_t : uint64_t {
      oem_fixed_font = 0xa,
      ansi_fixed_font,
      ansi_var_font,
      system_font,
      device_default_font,
      system_fixed_font = 0x10,
      default_gui_font
   };

   enum color_t : uint64_t {
      rgb_black,
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
         ( ( call ? m_gui_full : m_gui_base ) + address );
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

   auto gdi_compatible_dc(
      uint64_t ctx,
      uint64_t& src
   ) {
      if ( !m_gui_base || !ctx )
         return os->status_error;

      src = call_fn <uint64_t( __fastcall* )(
         uint64_t ctx
      )> ( gui_base, 0x36a10 )( ctx );

      return os->status_okay;
   }

   auto gdi_delete_object(
      uint64_t& object
   ) {
      if ( !m_gui_base || !object )
         return os->status_error;

      call_fn <uint64_t( __fastcall* )(
         uint64_t brush
      )> ( gui_base, 0x35440 )( object );

      object = 0;
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

   auto gdi_set_color(
      uint64_t ctx,
      color_t color
   ) {
      if ( !m_gui_full || !ctx )
         return os->status_error;

      call_fn <uint64_t( __fastcall* )(
         uint64_t ctx,
         uint32_t color
      )> ( gui_full, 0x14f50 )( ctx, color );

      return os->status_okay;
   }

   auto gdi_set_transparent(
      uint64_t ctx
   ) {
      if ( !m_gui_full || !ctx )
         return os->status_error;

      call_fn <uint64_t( __fastcall* )(
         uint64_t ctx,
         uint32_t color
      )> ( gui_full, 0x14f50 )( ctx, 1 );

      return os->status_okay;
   }

   auto gdi_stock_object(
      uint64_t& obj,
      uint64_t object
   ) {
      if ( !m_gui_base )
         return os->status_error;

      obj = call_fn <uint64_t( __fastcall* )(
         uint32_t flag
      )> ( gui_base, 0x265f0 )( object );

      return os->status_okay;
   }

   auto gdi_compatible_bitmap(
      uint64_t ctx,
      uint32_t width,
      uint32_t height,
      uint64_t& bitmap
   ) {
      if ( !m_gui_full || !ctx )
         return os->status_error;

      bitmap = call_fn <uint64_t( __fastcall* )(
         uint64_t ctx,
         uint32_t width,
         uint32_t height
      )> ( gui_full, 0x117480 )( ctx, width, height );

      return os->status_okay;
   }

   auto gdi_create_pen(
      color_t color,
      uint64_t& pen
   ) {
      if ( !m_gui_full )
         return os->status_error;

      pen = call_fn <uint64_t( __fastcall* )(
         uint32_t style,
         uint32_t width,
         uint32_t color,
         uint64_t brush
      )> ( gui_full, 0x10aa74 )( 0, 5, color, 0 );

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

   auto gdi_text_out(
      uint64_t ctx,
      uint32_t dst_x,
      uint32_t dst_y,
      wstring_t string,
      uint32_t length
   ) {
      if ( !m_gui_full || !ctx || !string )
         return os->status_error;

      call_fn <uint64_t( __fastcall* )(
         uint64_t ctx,
         uint32_t dst_x,
         uint32_t dst_y,
         uint32_t flip,
         uint64_t flag,
         wstring_t string,
         uint32_t length,
         uint64_t unused,
         uint64_t reserved,
         uint32_t safe
      )> ( gui_full, 0x11339c )( ctx, dst_x, dst_y, 
         0, 0, string, length, 0, 0, 0 );

      return os->status_okay;
   }

   auto gdi_set_text_color(
      uint64_t ctx,
      color_t color
   ) {
      if ( !m_gui_full || !ctx )
         return os->status_error;

      call_fn <uint64_t( __fastcall* )(
         uint64_t ctx,
         uint32_t color
      )> ( gui_full, 0x14e40 )( ctx, color );

      return os->status_okay;
   }

   auto gdi_select_brush(
      uint64_t ctx,
      uint64_t brush
   ) {
      if ( !m_gui_full || !brush )
         return os->status_error;

      call_fn <uint64_t( __fastcall* )(
         uint64_t ctx, 
         uint64_t brush
      )> ( gui_full, 0x2a8ba0 )( ctx, brush );

      return os->status_okay;
   }

   auto gdi_select_bitmap(
      uint64_t ctx,
      uint64_t bitmap
   ) {
      if ( !m_gui_base || !ctx || !bitmap )
         return os->status_error;

      call_fn <uint64_t( __fastcall* )(
         uint64_t ctx,
         uint64_t bitmap
      )> ( gui_base, 0x8e060 )( ctx, bitmap );

      return os->status_okay;
   }

   auto gdi_set_pixel(
      uint64_t ctx,
      uint32_t dst_x,
      uint32_t dst_y,
      color_t color
   ) {
      if ( !m_gui_full || !ctx )
         return os->status_error;

      call_fn <uint64_t( __fastcall* )(
         uint64_t ctx,
         uint32_t dst_x,
         uint32_t dst_y,
         uint32_t color
      )> ( gui_full, 0x141180 )( ctx, 
         dst_x, dst_y, color );

      return os->status_okay;
   }

   auto gdi_select_font(
      uint64_t ctx,
      uint64_t font
   ) {
      if ( !m_gui_full || !font )
         return os->status_error;

      call_fn <uint64_t( __fastcall* )(
         uint64_t ctx,
         uint64_t font
      )> ( gui_full, 0x13290 )( ctx, font );

      return os->status_okay;
   }

   auto gdi_select_pen(
      uint64_t ctx,
      uint64_t pen
   ) {
      if ( !m_gui_base || !pen )
         return os->status_error;

      call_fn <uint64_t( __fastcall* )(
         uint64_t ctx, 
         uint64_t pen
      )> ( gui_full, 0x2a8bc0 )( ctx, pen );

      return os->status_okay;
   }

   auto gdi_pat_blt(
      uint64_t ctx,
      uint32_t src_x,
      uint32_t src_y,
      uint32_t dst_x,
      uint32_t dst_y,
      uint32_t flag = 0xf00021
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
      )> ( gui_full, 0x7cbb0 )( ctx, src_x, 
         src_y, dst_x, dst_y, flag );

      return os->status_okay;
   }

   auto gdi_move_to(
      uint64_t ctx,
      uint32_t src_x,
      uint32_t src_y
   ) {
      if ( !m_gui_full || !ctx )
         return os->status_error;

      call_fn <uint64_t( __fastcall* )(
         uint64_t ctx,
         uint32_t src_x,
         uint32_t src_y,
         uint64_t unused
      )> ( gui_full, 0x269cac )( ctx, src_x, src_y, 0 );

      return os->status_okay;
   }

   auto gdi_line_to(
      uint64_t ctx,
      uint32_t dst_x,
      uint32_t dst_y
   ) {
      if ( !m_gui_full || !ctx )
         return os->status_error;

      call_fn <uint64_t( __fastcall* )(
         uint64_t ctx,
         uint32_t dst_x,
         uint32_t dst_y
      )> ( gui_full, 0x7799c )( ctx, dst_x, dst_y );

      return os->status_okay;
   }

   auto gdi_bit_blt(
      uint64_t ctx,
      uint32_t src_x,
      uint32_t src_y,
      uint32_t width,
      uint32_t height,
      uint64_t src,
      uint32_t dst_x,
      uint32_t dst_y,
      color_t color,
      uint32_t flag = 0xcc0020
   ) {
      if ( !m_gui_full || !ctx || !src )
         return os->status_error;

      call_fn <uint64_t( __fastcall* )(
         uint64_t ctx,
         uint32_t dst_x,
         uint32_t dst_y,
         uint32_t width,
         uint32_t height,
         uint64_t src,
         uint32_t src_x,
         uint32_t src_y,
         uint32_t rop,
         uint32_t color,
         uint8_t flag
      )> ( gui_full, 0x8be70 )( ctx, src_x, src_y, width, 
         height, src, dst_x, dst_y, flag, color, 0 );

      return os->status_okay;
   }
   
   auto draw_line(
      uint32_t src_x,
      uint32_t src_y,
      uint32_t dst_x,
      uint32_t dst_y,
      color_t color
   ) {
      if ( !m_gdi_ctx )
         return os->status_error;

      // abs delta
      int32_t dx = ( dx = ( dst_x - src_x ) ) < 0 ? -dx : dx;
      int32_t dy = ( dy = ( dst_y - src_y ) ) < 0 ? -dy : dy;

      // direction
      int32_t sx = src_x < dst_x ? 1 : -1;
      int32_t sy = src_y < dst_y ? 1 : -1;

      // errors
      int32_t ex = ( dx > dy ? dx : -dy ) / 2;
      int32_t ey = ( sx > sy ? sx : -sy ) / 2;

      for (
         ;;
      ) {
         // draw pixel
         gdi_set_pixel( m_gdi_ctx, src_x, src_y, color );

         // check pos
         if ( src_x == dst_x && src_y == dst_y )
            break;

         // set error
         ey = ex;

         // walk points
         if ( ey > -dx ) ex -= dy, src_x += sx;
         if ( ey < dy ) ex += dx, src_y += sy;
      }

      return os->status_okay;
   }

   auto draw_box(
      uint64_t brush,
      uint32_t src_x,
      uint32_t src_y,
      uint32_t dst_x,
      uint32_t dst_y
   ) {
      if ( !m_gdi_ctx || !brush )
         return os->status_error;

      if ( gdi_select_brush( m_gdi_ctx, brush ) )
         return os->status_error;

      gdi_pat_blt( m_gdi_ctx, src_x, src_y, 1, dst_y - src_y );
      gdi_pat_blt( m_gdi_ctx, dst_x - 1, src_y, 1, dst_y - src_y );
      gdi_pat_blt( m_gdi_ctx, src_x, src_y, dst_x - src_x, 1 );
      gdi_pat_blt( m_gdi_ctx, src_x, dst_y - 1, dst_x - src_x, 1 );

      return os->status_okay;
   }
} __ui_helper;

extern ui_helper_t* ui;
