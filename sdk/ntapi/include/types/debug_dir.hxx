#pragma once

namespace nt {
   struct debug_dir_t {
      std::uint8_t m_pad0[ 0x14 ];
      std::uint32_t m_ptr;
      std::uint8_t m_pad1[ 0x4 ];
   };
}
