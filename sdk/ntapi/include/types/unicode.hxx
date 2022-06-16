#pragma once

namespace nt {
   struct unicode_t {
      std::uint16_t m_length, m_capacity;
      std::wstring_t m_buffer;
   };
}
