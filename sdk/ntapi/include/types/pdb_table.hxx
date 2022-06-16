#pragma once

namespace nt {
   struct pdb_table_t {
      std::uint16_t m_magic;
      std::uint8_t m_pad0[ 0x2 ];
      std::uint32_t m_data1;
      std::uint16_t m_data2;
      std::uint16_t m_data3;
      std::uint8_t m_data4[ 0x8 ];
      std::uint32_t m_age;
      char m_name[ 0xff ];
   };
}
