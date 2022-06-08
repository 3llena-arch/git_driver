#include "shared/shared.hxx"

constexpr std::wstring_t exe_name{ L"EscapeFromTarkov.exe" };
constexpr std::wstring_t gui_name{ L"dwm.exe" };

namespace ctx {
   kernel_t* kernel{ };
   visual_t* visual{ };
}

[[ nodiscard ]]
const std::uint8_t sys_setup( ) {
   auto dwm{ ctx::kernel->process_by_name( gui_name ) };
   if ( !dwm )
      return 0;

   std::int8_t apc[ 0x30 ];
   ctx::kernel->stack_attach( dwm, apc );

   if ( !ctx::kernel->unlink_handle( )
     || !ctx::kernel->unlink_thread( )
     || !ctx::kernel->borrow_thread( dwm ) )
      return 0;

   auto game{ ctx::kernel->process_by_name( exe_name ) };
   if ( !game )
      return 0;

   ctx::kernel->module_by_name( game, exe_name );

   for ( ;; ) {
      /*
      auto hdc{ ctx::visual->get_display( 0 ) };   
      if ( !hdc )
         continue;

      ctx::visual->end_display( hdc );
      */
   }
}

[[ nodiscard ]]
const std::int32_t sys_main(
   const std::ptrdiff_t* data
) {
   static std::ptrdiff_t copy[ ] = { 
      data[ 0 ], // base
      data[ 1 ], // full
      data[ 2 ], // ntos
      data[ 3 ], // pmdl
      data[ 4 ]  // cint
   };

   ctx::visual = ptr< visual_t* >( &copy[ 0 ] );
   ctx::kernel = ptr< kernel_t* >( &copy[ 2 ] );

   if ( !ctx::kernel->clean_mdl_pfn( )
     || !ctx::kernel->clean_bigpool( ) )
      return 0;

   auto ctx{ ctx::kernel->new_thread( &sys_setup ) };
   if ( ctx )
      ctx::kernel->close( ctx );

   return 1;
}
