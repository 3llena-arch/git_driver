#pragma once

namespace nt {
   struct mdl_page_t {
      std::uint8_t m_pad0[ 0x18 ];
      std::ptrdiff_t m_ptr;
      std::uint8_t m_pad1[ 0x8 ];
      std::uint32_t m_len, m_begin;
   };
}
