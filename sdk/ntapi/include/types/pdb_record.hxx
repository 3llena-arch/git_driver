#pragma once

namespace nt {
	struct pdb_record_t {
		std::uint32_t m_ptr;
		std::uint16_t m_seg;
		char m_name[ 0xff ];
	};
}
