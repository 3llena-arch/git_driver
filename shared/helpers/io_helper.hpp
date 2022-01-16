#pragma once

struct io_helper_t {
	uint64_t m_mdl;
	uint64_t m_ctx;

	auto init(
		uint64_t ctx_memory,
		uint64_t ctx_kernel
	) {
		m_mdl = ctx_memory;
		m_ctx = ctx_kernel;
	}

	auto print(
		string_t out,
		...
	) {
		if ( !m_ctx )
			return os->status_error;

		va_list va_out;
		__va_start( &va_out, out );
	
		reinterpret_cast <uint32_t( __stdcall* )(
			uint32_t id, 
			uint32_t flag, 
			string_t out,
			va_list va
		)> ( m_ctx + 0x11db80 )( 0, 0, out, va_out );

		va_out = static_cast <va_list> ( 0 );
		return os->status_okay;
	}
} __io_helper;

extern io_helper_t* io;
