#include "shared/shared.hpp"

os_helper_t* os = &__os_helper;
io_helper_t* io = &__io_helper;
nt_helper_t* nt = &__nt_helper;

auto main(
	uint64_t ctx_memory,
	uint64_t ctx_kernel
) -> uint32_t {
	io->m_mdl = ctx_memory;
	io->m_ctx = ctx_kernel;

	io->print( "hello\n" );
	io->print( "%c\n", os->to_string( os->status_okay ) );

	return os->status_okay;
}
