#include "shared/shared.hxx"

namespace ctx {
   kernel_t* kernel{ };
   visual_t* visual{ };
}

[[ nodiscard ]]
const std::uint8_t sys_setup( ) {
   ctx::kernel->msg( "--> entrypoint called\n" );

   ctx::kernel->clean_mdl_pfn( );
   ctx::kernel->clean_bigpool( );

   auto dwm{ ctx::kernel->process_by_name( L"dwm.exe" ) };
   if ( dwm )
      ctx::kernel->msg( "--> found dwm at %llx\n", dwm );

   std::int8_t apc[ 0x30 ];
   ctx::kernel->stack_attach( dwm, ptr< >( apc ) );

   ctx::kernel->unlink_handle( );
   ctx::kernel->unlink_thread( );

   /*
   ctx::kernel->remove_apc_queue( ctx::kernel->get_thread( ) + 0x98 );
   ctx::kernel->remove_apc_queue( ctx::kernel->get_thread( ) + 0xa0 );
   ctx::kernel->msg("--> %llx\n", ctx::kernel->rundown_apc_queues(ctx::kernel->get_thread()));
   */

   ctx::kernel->msg("--> okay\n");
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

   ctx::visual = ptr< visual_t* >( &copy[ 0 ] );
   ctx::kernel = ptr< kernel_t* >( &copy[ 2 ] );

   auto ctx{ ctx::kernel->new_thread( &sys_setup ) };
   if ( ctx )
      ctx::kernel->close( ctx );

   return 1;
}
