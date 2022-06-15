#pragma once

namespace nt {
   struct pdb_record_t {
      std::uint16_t m_length;
      std::uint8_t m_pad0[ 0x6 ];
      std::uint32_t m_ptr;
      std::uint16_t m_seg;
      char m_name[ 0xff ];
   };
}
