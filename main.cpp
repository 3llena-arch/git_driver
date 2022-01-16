#include "shared/shared.hpp"

os_helper_t* os = &__os_helper;
io_helper_t* io = &__io_helper;
nt_helper_t* nt = &__nt_helper;

auto call( ) {
	//
}

auto main(
	uint64_t ctx_memory,
	uint64_t ctx_kernel
) -> uint32_t {
	if ( !ctx_memory || !ctx_kernel )
		return os->status_error;

	io->init( ctx_memory, ctx_kernel );
	nt->init( ctx_memory, ctx_kernel );

	nt->create_thread( &call );

	return os->status_okay;
}
