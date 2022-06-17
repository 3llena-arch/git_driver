#pragma once

namespace nt {
   struct export_dir_t {
      std::uint8_t m_pad0[ 0x18 ];
      std::uint32_t m_count, m_ptrs, m_name, m_ords;
   };
}
