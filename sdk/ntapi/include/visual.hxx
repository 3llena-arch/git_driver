#pragma once

namespace nt {
   struct visual_t {
      const enum affinity_t : std::uint8_t {
         wda_default = 0x00000000,
         wda_monitor = 0x00000001,
         wda_exclude = 0x00000011
      };

      [[ nodiscard ]]
      constexpr std::int32_t rgb(
         const std::uint8_t r,
         const std::uint8_t g,
         const std::uint8_t b
      ) {
         return std::int32_t{ r | ( g << 0x8 ) | ( b << 0x10 ) };
      }

      [[ nodiscard ]]
      const std::int32_t get_affinity(
         const std::ptrdiff_t window
      ) {
         static auto addr{ m_full + kernel->diff( 0x2445b8, 0x245d40 ) };
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
      const std::ptrdiff_t get_user_dc(
         const std::ptrdiff_t window
      ) {
         static auto addr{ kernel->get_export( m_base, "NtUserGetDC" ) };
         if ( !addr )
            return 0;

         return ptr< std::ptrdiff_t( __stdcall* )(
            const std::ptrdiff_t window ) >( addr )( window );
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

      const std::ptrdiff_t release_dc(
         const std::ptrdiff_t user_dc
      ) {
         static auto addr{ kernel->get_export( m_base, "NtUserReleaseDC" ) };
         if ( !addr )
            return 0;

         return ptr< std::ptrdiff_t( __stdcall* )(
            const std::ptrdiff_t user_dc ) >( addr )( user_dc );
      }

      const std::uint8_t draw_line(
         const std::ptrdiff_t hdc,
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
            set_pixel( hdc, src_x, src_y, color );
            if ( src_x == dst_x && src_y == dst_y )
               break;

            ey = ex;
            if ( ey > -dx ) ex -= dy, src_x += sx;
            if ( ey < dy ) ex += dx, src_y += sy;
         }
         return 1;
      }

      const std::ptrdiff_t set_pixel(
         const std::ptrdiff_t user_dc,
         const std::uint32_t x,
         const std::uint32_t y,
         const std::uint32_t color
      ) {
         static auto addr{ kernel->get_export( m_full, "NtGdiSetPixel" ) };
         if ( !addr )
            return 0;

         return ptr< std::ptrdiff_t( __stdcall* )(
            const std::ptrdiff_t user_dc,
            const std::uint32_t x,
            const std::uint32_t y,
            const std::uint32_t color
         ) >( addr )( user_dc, x, y, color );
      }

      [[ nodiscard ]]
      const std::uint8_t validate_affinity( ) {
         auto hdc{ get_user_dc( 0 ) };
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
         auto hdc{ get_user_dc( 0 ) };
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
