#pragma once

namespace nt {
   struct visual_t {
      [[ nodiscard ]]
      constexpr std::int32_t new_color(
         const std::uint8_t r,
         const std::uint8_t g,
         const std::uint8_t b
      ) {
         return std::int32_t{ r | ( g << 0x8 ) | ( b << 0x10 ) };
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

      const std::ptrdiff_t release_dc(
         const std::ptrdiff_t user_dc
      ) {
         static auto addr{ kernel->get_export( m_base, "NtUserReleaseDC" ) };
         if ( !addr )
            return 0;

         return ptr< std::ptrdiff_t( __stdcall* )(
            const std::ptrdiff_t user_dc ) >( addr )( user_dc );
      }

      const std::ptrdiff_t set_pixel(
         const std::ptrdiff_t user_dc,
         const std::uint32_t x,
         const std::uint32_t y,
         const std::uint32_t color
      ) {
         static auto addr{ kernel->get_export( m_base, "NtGdiSetPixel" ) };
         if ( !addr )
            return 0;

         return ptr< std::ptrdiff_t( __stdcall* )(
            const std::ptrdiff_t user_dc,
            const std::uint32_t x,
            const std::uint32_t y,
            const std::uint32_t color
         ) >( addr )( user_dc, x, y, color );
      }

      const std::ptrdiff_t m_base;
      const std::ptrdiff_t m_full;
   };
}

extern nt::visual_t* visual;
