#pragma once

struct log_helper_t {
	uint64_t m_ctx;
	uint64_t m_mdl;

	auto print(
		string_t out,
		...
	) {
		if ( !m_ctx )
			os->status_error;

		va_list va_out;
		va_start(va_out, out);
	
		reinterpret_cast <uint32_t( __stdcall* )(
			uint32_t id, 
			uint32_t flag, 
			string_t out,
			va_list va
		)> ( m_ctx + 0x11db80 )( 0, 0, out, va_out );

		va_end(va_out);
		return os->status_okay;
	}
} __log_helper;

extern log_helper_t* log;
