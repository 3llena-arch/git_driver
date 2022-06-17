#pragma once

namespace nt {
   struct dos_header_t {
      std::uint16_t m_magic;
      std::uint8_t m_pad0[ 0x3a ];
      std::uint32_t m_offset;
   };
}
