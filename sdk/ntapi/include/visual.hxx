#pragma once

namespace nt {
   struct visual_t {
      const enum affinity_t : std::uint8_t {
         wda_default = 0x00000000,
         wda_monitor = 0x00000001,
         wda_exclude = 0x00000011
      };

      const enum raster_t : std::uint32_t {
         pat_copy = 0x00f00021,
         pat_invert = 0x005a0049,
         blackness = 0x00000042,
         whiteness = 0x00ff0062
      };

      [[ nodiscard ]]
      constexpr std::int32_t rgb(
         const std::uint8_t r,
         const std::uint8_t g,
         const std::uint8_t b
      ) {
         return std::int32_t{ r | ( g << 0x8 ) | ( b << 0x10 ) };
      }

      const std::uint8_t invalidate_wnd(
         const std::ptrdiff_t window
      ) {
         static auto addr{ kernel->get_export( m_full, "NtUserInvalidateRect" ) };
         if ( !addr )
            return 0;

         return !!ptr< std::ptrdiff_t( __stdcall* )(
            const std::ptrdiff_t window,
            const std::ptrdiff_t rect,
            const std::uint32_t erase
         ) >( addr )( window, 0, 0 );
      }

      [[ nodiscard ]]
      const std::int32_t get_affinity(
         const std::ptrdiff_t window
      ) {
         static auto addr{ kernel->get_export( m_full, "NtUserGetWindowDisplayAffinity" ) };
         if ( !addr )
            return 0;

         std::int32_t affinity{ };

         ptr< std::ptrdiff_t( __stdcall* )(
            const std::ptrdiff_t window,
            const std::int32_t* affinity
         ) >( addr )( window, &affinity );

         return affinity;
      }

      [[ nodiscard ]]
      const std::int16_t get_key_state(
         const std::uint32_t key
      ) {
         static auto addr{ kernel->get_export( m_base, "NtUserGetAsyncKeyState" ) };
         if ( !addr )
            return 0;

         return ptr< std::int16_t( __stdcall* )(
            const std::uint32_t key ) >( addr )( key );
      }

      [[ nodiscard ]]
      const std::ptrdiff_t create_brush(
         const std::ptrdiff_t context,
         const std::uint32_t color
      ) {
         static auto addr{ kernel->get_export( m_full, "NtGdiCreateSolidBrush" ) };
         if ( !addr )
            return 0;

         return ptr< std::ptrdiff_t( __stdcall* )(
            const std::ptrdiff_t context,
            const std::uint32_t color
         ) >( addr )( context, color );
      }

      [[ nodiscard ]]
      const rect_t get_wnd_rect(
         const std::ptrdiff_t window
      ) {
         static auto addr{ kernel->get_export( m_full, "DxgkEngGetWindowRect" ) };
         if ( !addr )
            return { };

         rect_t rectangle{ };

         ptr< std::ptrdiff_t( __stdcall* )(
            const std::ptrdiff_t window,
            const rect_t* rectangle
         ) >( addr )( window, &rectangle );

         return rectangle;
      }

      const std::uint8_t delete_object(
         const std::ptrdiff_t object
      ) {
         static auto addr{ kernel->get_export( m_base, "GreDeleteObject" ) };
         if ( !addr )
            return 0;

         return !!ptr< std::ptrdiff_t( __stdcall* )(
            const std::ptrdiff_t object ) >( addr )( object );
      }

      const std::ptrdiff_t select_brush(
         const std::ptrdiff_t context,
         const std::ptrdiff_t brush
      ) {
         static auto addr{ kernel->get_export( m_full, "NtGdiSelectBrush" ) };
         if ( !addr )
            return 0;

         return ptr< std::ptrdiff_t( __stdcall* )(
            const std::ptrdiff_t context,
            const std::ptrdiff_t brush
         ) >( addr )( context, brush );
      }

      [[ nodiscard ]]
      const std::ptrdiff_t get_user_dc( ) {
         static auto addr{ kernel->get_export( m_base, "NtUserGetDC" ) };
         if ( !addr )
            return 0;

         return ptr< std::ptrdiff_t( __stdcall* )(
            const std::ptrdiff_t window ) >( addr )( 0 );
      }

      const std::uint8_t release_dc(
         const std::ptrdiff_t context
      ) {
         static auto addr{ kernel->get_export( m_base, "NtUserReleaseDC" ) };
         if ( !addr )
            return 0;

         return !!ptr< std::ptrdiff_t( __stdcall* )(
            const std::ptrdiff_t context ) >( addr )( context );
      }

      [[ nodiscard ]]
      const std::ptrdiff_t get_dc_wnd(
         const std::ptrdiff_t context
      ) {
         static auto addr{ kernel->get_export( m_full, "NtUserWindowFromDC" ) };
         if ( !addr )
            return 0;

         return ptr< std::ptrdiff_t( __stdcall* )(
            const std::ptrdiff_t context ) >( addr )( context );
      }

      [[ nodiscard ]]
      const std::ptrdiff_t get_valid_wnd(
         const std::ptrdiff_t window
      ) {
         static auto addr{ kernel->get_export( m_base, "ValidateHwnd" ) };
         if ( !addr )
            return 0;

         return ptr< std::ptrdiff_t( __stdcall* )(
            const std::ptrdiff_t window ) >( addr )( window );
      }

      const std::ptrdiff_t set_tree_protection(
         const std::ptrdiff_t window,
         const std::int32_t affinity
      ) {
         static auto addr{ m_full + kernel->diff( 0x244308, 0x245a30 ) };
         if ( !addr )
            return 0;

         return ptr< std::ptrdiff_t( __stdcall* )(
            const std::ptrdiff_t window,
            const std::int32_t affinity
         ) >( addr )( window, affinity);
      }

      const std::uint8_t draw_line(
         const std::ptrdiff_t context,
         std::uint32_t src_x,
         std::uint32_t src_y,
         const std::uint32_t dst_x,
         const std::uint32_t dst_y,
         const std::uint32_t color
      ) {
         std::int32_t dx = ( dx = ( dst_x - src_x ) ) < 0 ? -dx : dx;
         std::int32_t dy = ( dy = ( dst_y - src_y ) ) < 0 ? -dy : dy;

         std::int32_t sx = src_x < dst_x ? 1 : -1;
         std::int32_t sy = src_y < dst_y ? 1 : -1;

         std::int32_t ex = ( dx > dy ? dx : -dy ) / 2;
         std::int32_t ey = ( sx > sy ? sx : -sy ) / 2;

         for ( ;; ) {
            set_pixel( context, src_x, src_y, color );
            if ( src_x == dst_x && src_y == dst_y )
               break;

            ey = ex;
            if ( ey > -dx ) ex -= dy, src_x += sx;
            if ( ey < dy ) ex += dx, src_y += sy;
         }
         return 1;
      }

      const std::uint8_t pat_blt(
         const std::ptrdiff_t context,
         const std::uint32_t x,
         const std::uint32_t y,
         const std::uint32_t w,
         const std::uint32_t h,
         const std::uint32_t flags
      ) {
         static auto addr{ kernel->get_export( m_full, "NtGdiPatBlt" ) };
         if ( !addr )
            return 0;

         return !!ptr< std::ptrdiff_t( __stdcall* )(
            const std::ptrdiff_t context,
            const std::uint32_t x,
            const std::uint32_t y,
            const std::uint32_t w,
            const std::uint32_t h,
            const std::uint32_t flags
         ) >( addr )( context, x, y, w, h, flags );
      }

      const std::uint8_t set_pixel(
         const std::ptrdiff_t context,
         const std::uint32_t x,
         const std::uint32_t y,
         const std::uint32_t color
      ) {
         static auto addr{ kernel->get_export( m_full, "NtGdiSetPixel" ) };
         if ( !addr )
            return 0;

         return !!ptr< std::ptrdiff_t( __stdcall* )(
            const std::ptrdiff_t context,
            const std::uint32_t x,
            const std::uint32_t y,
            const std::uint32_t color
         ) >( addr )( context, x, y, color );
      }

      [[ nodiscard ]]
      const std::uint8_t validate_affinity( ) {
         auto hdc{ get_user_dc( ) };
         auto wnd{ get_dc_wnd( hdc ) };

         if ( !hdc || !wnd )
            return 0;

         if ( get_affinity( wnd ) == wda_monitor
           || get_affinity( wnd ) == wda_exclude )
            return 0;

         return !!release_dc( hdc );
      }

      [[ nodiscard ]]
      const std::uint8_t set_tree_affinity( ) {
         auto hdc{ get_user_dc( ) };
         auto wnd{ get_dc_wnd( hdc ) };

         if ( !hdc || !wnd )
            return 0;

         set_tree_protection( get_valid_wnd( wnd ), wda_exclude );
         return !!release_dc( hdc );
      }

      const std::ptrdiff_t m_base;
      const std::ptrdiff_t m_full;
   };
}

extern nt::visual_t* visual;
