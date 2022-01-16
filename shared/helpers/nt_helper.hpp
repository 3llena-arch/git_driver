#pragma once

struct nt_helper_t {
	uint64_t m_mdl;
	uint64_t m_ctx;

	auto init(
		uint64_t ctx_memory,
		uint64_t ctx_kernel
	) {
		m_mdl = ctx_memory;
		m_ctx = ctx_kernel;
	}

	auto create_thread(
		auto call,
		uint64_t handle = 0
	) {
		auto cast = reinterpret_cast <uint64_t> ( call );
		if ( !m_ctx )
			return os->status_error;

		reinterpret_cast <uint32_t( __stdcall* )(
			uint64_t* thread,
			uint32_t access,
			uint64_t attributes,
			uint64_t process,
			uint64_t client,
			uint64_t routine,
			uint64_t context
		)> ( m_ctx + 0x6eb360 )( &handle, 0, 0, 0, 0, cast, 0 );

		if ( !handle )
			return os->status_warning;

		reinterpret_cast <uint32_t( __stdcall* )(
			uint64_t handle
		)> ( m_ctx + 0x1bd9d0 )( handle );

		return os->status_okay;
	}
} __nt_helper;

extern nt_helper_t* nt;
