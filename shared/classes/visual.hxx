#pragma once

struct visual_t {   
   [[ nodiscard ]]
   constexpr std::int32_t color(
      const std::uint8_t r,
      const std::uint8_t g,
      const std::uint8_t b
   ) {
      return std::int32_t{ r | ( g << 0x8 ) | ( b << 0x10 ) };
   }

   [[ nodiscard ]]
   const std::ptrdiff_t get_display(
      const std::ptrdiff_t window
   ) {
      return ptr< std::ptrdiff_t( __fastcall* )(
         std::ptrdiff_t window
      ) >( m_base + 0x3a430 )( window );
   }

   const std::uint8_t end_display(
      const std::ptrdiff_t display
   ) {
      return !ptr< std::int32_t( __fastcall* )(
         std::ptrdiff_t display
      ) >( m_base + 0x3a6c0 )( display );
   }

   const std::uint8_t set_pixel(
      const std::ptrdiff_t hdc,
      const std::uint32_t dst_x,
      const std::uint32_t dst_y,
      const std::uint32_t color
   ) {
      return !!ptr< std::int32_t( __fastcall* )(
         std::ptrdiff_t hdc,
         std::uint32_t dst_x,
         std::uint32_t dst_y,
         std::uint32_t color
      ) >( m_full + 0x141180 )( hdc, dst_x, dst_y, color );
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

   std::ptrdiff_t m_base;
   std::ptrdiff_t m_full;
};

extern visual_t* visual;
