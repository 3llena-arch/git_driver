#pragma once

struct visual_t {   
   enum font_t : std::uint16_t {
      oem_fixed_font = 0xa,
      ansi_fixed_font,
      ansi_var_font,
      system_font,
      device_default_font,
      system_fixed_font = 0x10,
      default_gui_font
   };

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
      auto ctx{ ptr< std::uint8_t* >( m_base ) };
      if ( !ctx )
         return 0;

      while ( ctx[ 0x07 ] != 0x6c
           || ctx[ 0x0c ] != 0x74
           || ctx[ 0x0f ] != 0x57
           || ctx[ 0x11 ] != 0x81 )
         ctx++;

      return ptr< std::ptrdiff_t( __fastcall* )(
         std::ptrdiff_t window
      ) >( ctx )( window );
   }

   const std::uint8_t end_display(
      const std::ptrdiff_t display
   ) {
      auto ctx{ ptr< std::uint8_t* >( m_base ) };
      if ( !ctx )
         return 0;

      while ( ctx[ 0x28 ] != 0x4f
           || ctx[ 0x29 ] != 0x48
           || ctx[ 0x2d ] != 0xff
           || ctx[ 0x36 ] != 0x00 )
         ctx++;

      return !ptr< std::int32_t( __fastcall* )(
         std::ptrdiff_t display
      ) >( ctx )( display );
   }

   const std::uint8_t set_pixel(
      const std::ptrdiff_t hdc,
      const vec2_t< std::uint32_t >dst,
      const std::uint32_t color
   ) {
      auto ctx{ ptr< std::uint8_t* >( m_full ) };
      if ( !ctx )
         return 0;

      while ( ctx[ 0x47 ] != 0xd8
           || ctx[ 0x48 ] != 0x83
           || ctx[ 0x49 ] != 0xce )
         ctx++;

      return !!ptr< std::int32_t( __fastcall* )(
         std::ptrdiff_t hdc,
         vec2_t< std::uint32_t >dst,
         std::uint32_t color
      ) >( ctx )( hdc, dst, color );
   }

   std::ptrdiff_t m_base;
   std::ptrdiff_t m_full;
};

extern visual_t* visual;
