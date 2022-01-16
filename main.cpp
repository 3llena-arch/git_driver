#include "shared/shared.hpp"

os_helper_t* os = &__os_helper;
io_helper_t* io = &__io_helper;
nt_helper_t* nt = &__nt_helper;

auto call( ) {
	nt->m_src_thread = nt->call_fn <uint64_t(__stdcall* )( )> ( 0x31e40 )( );
	nt->m_src_pe = nt->call_fn <uint64_t( __stdcall* )( )> ( 0x67de0 )( );

	nt->query_process( "EscapeFromTark", nt->m_dst_pe );
	nt->query_gui_thread( nt->m_gui_thread );
	nt->query_cid_table( nt->m_cid_table );
	nt->query_cid_entry( nt->m_cid_entry );

	/*
	struct table_entry_t {
		char m_padding1[0x10];
	};

	auto cid_table = *reinterpret_cast <uint64_t*> ( nt->m_ctx + 0x574530 );
	auto cid_entry = reinterpret_cast <table_entry_t*( __fastcall* )( uint64_t, uint64_t )>
		( nt->m_ctx + 0x5f13e0 )( cid_table, *( uint64_t* )( nt->m_thread + 0x650 ) ); // thread id
	io->print( "CID Entry: %llx\n", cid_entry );

	auto d = *cid_entry;

	reinterpret_cast <uint64_t(__fastcall*)(uint64_t, uint64_t, table_entry_t*)>
		(nt->m_ctx + 0x660360)(cid_table, *(uint64_t*)(nt->m_thread + 0x650), cid_entry); // thread id

	io->print("Destroyed entry\n");

	reinterpret_cast <uint64_t(__fastcall*)(uint64_t, table_entry_t*)>
		(nt->m_ctx + 0x90c814)(cid_table, &d);

	io->print("Created Entry\n");
	*/

	// ExCreateHandle nt + 0x90c814
	// __int64 __fastcall ExCreateHandle(__int64 a1, __int64 a2)
	// HANDLE ExCreateHandle( __inout PHANDLE_TABLE HandleTable, 
	//					      __in PHANDLE_TABLE_ENTRY HandleTableEntry );
	
	// ExDestroyHandle nt + 0x660360
	// __int64 __fastcall ExDestroyHandle(__int64 a1, __int64 a2, __int64 a3)
	//									   handle_table, threadId, pCidEntry

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
