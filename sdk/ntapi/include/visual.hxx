#pragma once

namespace nt {
   struct visual_t {
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

      const std::ptrdiff_t m_base;
      const std::ptrdiff_t m_full;
   };
}

extern nt::visual_t* visual;
