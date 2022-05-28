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

   ctx::kernel->unlink_handle( );
   ctx::kernel->unlink_thread( );

   auto pe{ ctx::kernel->process_by_name( L"windbg.exe" ) };
   if ( pe )
      ctx::kernel->msg( "--> found process at %llx\n", pe );

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
