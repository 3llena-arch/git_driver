#pragma once

namespace nt {
   struct nt_headers_t {
      std::uint16_t m_magic;
      std::uint8_t m_pad0[ 0x4e ];
      std::uint32_t m_image_size;
      std::uint8_t m_pad1[ 0x34 ];
      std::uint32_t m_eat_table;
      std::uint8_t m_pad2[ 0x2c ];
      std::uint32_t m_dbg_table;
      std::uint8_t m_pad3[ 0x4c ];
   };
}
