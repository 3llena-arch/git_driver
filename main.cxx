#include "shared/shared.hxx"

constexpr std::wstring_t exe_name{ L"EscapeFromTarkov.exe" };
constexpr std::wstring_t gui_name{ L"dwm.exe" };

kernel_t* kernel{ };
visual_t* visual{ };

[[ nodiscard ]]
const std::uint8_t sys_setup( ) {
   auto dwm{ kernel->process_by_name( gui_name ) };
   if ( !dwm )
      return 0;

   std::int8_t apc[ 0x30 ];
   kernel->stack_attach( dwm, apc );

   if ( !kernel->unlink_handle( )
     || !kernel->unlink_thread( )
     || !kernel->borrow_thread( dwm ) )
      return 0;
   
   auto game{ kernel->process_by_name( exe_name ) };
   auto base{ kernel->module_by_name( game, L"UnityPlayer.dll" ) };

   ( base );

   for ( ;; ) { /* :) */ }
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

   visual = ptr< visual_t* >( &copy[ 0 ] );
   kernel = ptr< kernel_t* >( &copy[ 2 ] );

   if ( !kernel->clean_mdl_pfn( )
     || !kernel->clean_bigpool( ) )
      return 0;

   auto ctx{ kernel->new_thread( &sys_setup ) };
   if ( ctx )
      kernel->close( ctx );

   return 1;
}
