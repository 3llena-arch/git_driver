#pragma once

struct nt_helper_t {
	uint64_t m_mdl;
	uint64_t m_ctx;

	uint64_t m_src_thread;
	uint64_t m_gui_thread;
	
	uint64_t m_src_pe;
	uint64_t m_dst_pe;

	uint64_t m_cid_table;
	uint64_t m_cid_entry;

	auto init(
		uint64_t ctx_memory,
		uint64_t ctx_kernel
	) {
		m_mdl = ctx_memory;
		m_ctx = ctx_kernel;
	}

	template <typename type_t>
	auto call_fn(
		uint64_t address
	) {
		return reinterpret_cast <type_t> 
			( m_ctx + address );
	}

	auto create_thread(
		auto call,
		uint64_t handle = 0
	) {
		auto cast = reinterpret_cast <uint64_t> ( call );
		if ( !m_ctx )
			return os->status_error;

		call_fn <uint32_t( __stdcall* )(
			uint64_t* thread,
			uint32_t access,
			uint64_t attributes,
			uint64_t process,
			uint64_t client,
			uint64_t routine,
			uint64_t context
		)> ( 0x6eb360 )( &handle, 0, 0, 0, 0, cast, 0 );

		if ( !handle )
			return os->status_warning;

		call_fn <uint32_t( __stdcall* )(
			uint64_t handle
		)> ( 0x1bd9d0 )( handle );
		
		return os->status_okay;
	}

	auto query_gui_thread(
		uint64_t& thread
	) {
		// todo
		return os->status_okay;
	}

	auto query_process(
		string_t name,
		uint64_t& process
	) {
		if ( !m_ctx || !name )
			return os->status_error;

		while ( !process ) {
			// todo
			break;
		}

		return os->status_okay;
	}

	auto query_cid_table(
		uint64_t& table
	) {
		if ( !m_ctx )
			return os->status_error;

		table = *reinterpret_cast <uint64_t*> 
			( m_ctx + 0x574530 );
		
		return os->status_okay;
	}

	auto query_cid_entry(
		uint64_t& entry
	) {
		if ( !m_ctx || !m_src_thread || !m_cid_table )
			return os->status_error;

		auto id = *reinterpret_cast <uint64_t*>
			( m_src_thread + 0x650 );
		if ( !id )
			return os->status_error;

		entry = call_fn <uint64_t( __fastcall* )(
			uint64_t table,
			uint64_t id
		)> ( 0x5f13e0 )( m_cid_table, id );

		return os->status_okay;
	}

} __nt_helper;

extern nt_helper_t* nt;
