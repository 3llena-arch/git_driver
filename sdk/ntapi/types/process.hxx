#pragma once

namespace nt {
   struct process_t {
      std::uint8_t m_pad0[ 0x28 ];
      std::ptrdiff_t m_dir_table;
      std::uint8_t m_pad1[ 0x250 ];
      std::ptrdiff_t m_user_dir_table;
      std::uint8_t m_pad2[ 0x5f8 ];
   };
}
