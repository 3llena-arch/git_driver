#pragma once

struct visual_t {
   [[ nodiscard ]]
   const std::ptrdiff_t get_display(
      const std::ptrdiff_t window
   ) {
      return ptr< std::ptrdiff_t( __fastcall* )(
         std::ptrdiff_t window
      ) >( m_base + 0x3a430 )( window );
   }

   const std::ptrdiff_t end_display(
      const std::ptrdiff_t display
   ) {
      return ptr< std::ptrdiff_t( __fastcall* )(
         std::ptrdiff_t display
      ) >( m_base + 0x3a6c0 )( display );
   }

   std::ptrdiff_t m_base;
   std::ptrdiff_t m_full;
};

extern visual_t* visual;
