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
      data[ 2 ], // copy
      data[ 3 ], // base
      data[ 4 ]  // full
   };

   kernel = ptr< nt::kernel_t* >( &copy[ 0 ] );
   visual = ptr< nt::visual_t* >( &copy[ 3 ] );

   return 1;
}
