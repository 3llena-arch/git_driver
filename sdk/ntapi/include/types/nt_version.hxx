#pragma once

namespace nt {
   struct nt_version_t {
      std::uint8_t m_pad0[ 0xc ];
      std::uint32_t m_build;
      std::uint8_t m_pad1[ 0x104 ];
   };
}
