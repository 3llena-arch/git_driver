#include "shared/shared.hpp"

os_helper_t* os = &__os_helper;
io_helper_t* io = &__io_helper;
nt_helper_t* nt = &__nt_helper;

auto call( ) {
	nt->m_thread = nt->virtual_fn <uint64_t( * )( )> ( 0x31e40 )( );
	nt->m_src_pe = nt->virtual_fn <uint64_t( * )( )> ( 0x67de0 )( );

	/*
	uint64_t m_src_pe;
	uint64_t m_dst_pe;

	uint64_t m_cid_table;
	uint64_t m_cid_entry;
	*/

	io->print("current thread: %llx\n", nt->m_thread);

	// current pspcidtable entry
	// current pfn 
	// target process
	// cidtable = nt + 0x574530
	return os->status_okay;
}

auto main(
	uint64_t ctx_memory,
	uint64_t ctx_kernel
) -> uint32_t {
	if ( !ctx_memory || !ctx_kernel )
		return os->status_error;

	io->init( ctx_memory, ctx_kernel );
	nt->init( ctx_memory, ctx_kernel );

	return nt->create_thread( &call );
}
