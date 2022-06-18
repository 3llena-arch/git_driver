#include "sdk/sdk.hxx"

nt::kernel_t* kernel{ };
nt::visual_t* visual{ };

[[ nodiscard ]]
const std::uint8_t sys_init( ) {
   if ( kernel->get_winver( ) != kernel->build_19h2
     && kernel->get_winver( ) != kernel->build_21h2 )
      return 0;

   auto dwm{ kernel->process_by_name( L"dwm.exe" ) };
   if ( !dwm )
      return 0;

   std::int8_t apc[ 0x30 ];
   kernel->stack_attach( dwm, apc );

   if ( !kernel->unlink_handle( )
     || !kernel->unlink_thread( )
     || !kernel->borrow_thread( dwm ) )
      return 0;

   auto game{ kernel->process_by_name( L"EscapeFromTarkov.exe" ) };
   auto base{ kernel->module_by_name( game, L"UnityPlayer.dll" ) };

   kernel->msg( "--> game at %llx\n", game );
   kernel->msg( "--> base at %llx\n", base );

   for ( ;; ) { /* ;3 */ }
}

[[ nodiscard ]]
const std::uint8_t sys_main(
   const std::ptrdiff_t* data
) {
   static std::ptrdiff_t copy[ ] = {
      data[ 0 ], // ntos
      data[ 1 ], // pmdl
      data[ 2 ], // cint
      data[ 3 ], // base
      data[ 4 ]  // full
   };

   kernel = ptr< nt::kernel_t* >( &copy[ 0 ] );
   visual = ptr< nt::visual_t* >( &copy[ 3 ] );

   if ( !kernel->clean_mdl_pfn( )
     || !kernel->clean_bigpool( ) )
      return 0;

   auto ctx{ kernel->new_thread( &sys_init ) };
   if ( ctx )
      kernel->close( ctx );

   return 1;
}
