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
   if ( !game )
      return 0;

   if ( !visual->set_tree_affinity( )
     || !visual->validate_affinity( ) )
      return 0;

   for ( ;; ) {
      auto hdc{ visual->get_user_dc( 0 ) };
      if ( !hdc )
         continue;

      if ( visual->get_key_state( 0x20 ) & 0x8000 )
         for ( auto i = 0; i < 400; i++ )
            for ( auto j = 0; j < 400; j++ )
               visual->set_pixel( hdc, i, j, visual->rgb( i, j, 255 ) ),
               visual->invalidate_wnd( visual->get_valid_wnd( visual->get_dc_wnd( hdc ) ) );

      visual->release_dc( hdc );
   }
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
