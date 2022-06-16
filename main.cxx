#include "sdk/sdk.hxx"

nt::kernel_t* kernel{ };
nt::visual_t* visual{ };

[[ nodiscard ]]
const std::uint8_t sys_main(
   const std::ptrdiff_t* data
) {
   static std::ptrdiff_t copy[ ] = {
      data[ 0 ], // ntos
      data[ 1 ], // pmdl
      data[ 2 ], // syms
      data[ 3 ], // copy
      data[ 4 ], // base
      data[ 5 ]  // full
   };

   kernel = ptr< nt::kernel_t* >( &copy[ 0 ] );
   visual = ptr< nt::visual_t* >( &copy[ 4 ] );

   return 1;
}
