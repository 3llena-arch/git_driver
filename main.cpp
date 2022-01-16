#include "shared/shared.hpp"

os_helper_t* os = &__os_helper;
log_helper_t* log = &__log_helper;

auto main(
	uint64_t ctx_memory,
	uint64_t ctx_kernel
) -> uint32_t {
	log->m_mdl = ctx_memory;
	log->m_ctx = ctx_kernel;

	log->print( "Hello, world!\n" );

	return os->status_okay;
}
