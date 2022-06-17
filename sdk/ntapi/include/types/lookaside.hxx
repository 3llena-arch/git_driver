#pragma once

namespace nt {
   struct lookaside_t {
      std::uint8_t m_pad0[ 0x24 ];
      std::uint32_t m_type, m_tag, m_size;
      std::uint8_t m_pad1[ 0x30 ];
   };
}
