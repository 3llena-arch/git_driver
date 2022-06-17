#include "sdk/sdk.hxx"

nt::kernel_t* kernel{ };
nt::visual_t* visual{ };

[[ nodiscard ]]
const std::uint8_t sys_init( ) {
   kernel->msg( "--> hello from thread\n" );
   return 0;
}

[[ nodiscard ]]
const std::uint8_t sys_main(
   const std::ptrdiff_t* data
) {
   static std::ptrdiff_t copy[ ] = {
      data[ 0 ], // ntos
      data[ 1 ], // pmdl
      data[ 2 ], // base
      data[ 3 ]  // full
   };

   kernel = ptr< nt::kernel_t* >( &copy[ 0 ] );
   visual = ptr< nt::visual_t* >( &copy[ 2 ] );

   auto ctx{ kernel->new_thread( &sys_init ) };
   if ( ctx )
      kernel->close( ctx );

   return 1;
}
