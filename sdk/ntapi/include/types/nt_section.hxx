#pragma once

namespace nt {
   struct nt_section_t {
      char m_name[ 0x8 ];
      std::uint8_t m_pad0[ 0x4 ];
      std::uint32_t m_ptr;
      std::uint8_t m_pad1[ 0x18 ];
   };
}
