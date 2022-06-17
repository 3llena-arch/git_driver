#include "sdk/sdk.hxx"

constexpr std::wstring_t exe_name{ L"Calculator.exe" };
constexpr std::wstring_t gui_name{ L"dwm.exe" };

nt::kernel_t* kernel{ };
nt::visual_t* visual{ };

[[ nodiscard ]]
const std::uint8_t sys_init( ) {
   if ( kernel->get_winver( ) != kernel->build_19h2
     && kernel->get_winver( ) != kernel->build_21h2 )
      return 0;

   kernel->msg( "--> winver: %d\n", kernel->get_winver( ) );
   kernel->msg( "--> system: %llx\n", kernel->get_system( ) );
   kernel->msg( "--> cr3: %llx\n", kernel->get_cr3( kernel->get_system( ) ) );

   return 0;
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
