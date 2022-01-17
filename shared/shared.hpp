#pragma once

// warnings
#pragma warning( disable : 4244 )
#pragma warning( disable : 4100 )
#pragma warning( disable : 4083 )
#pragma warning( disable : 4005 )

// c includes
#include <cstdarg>
#include <cstring>

// numbers
using uint64_t = unsigned long long;
using uint32_t = unsigned int;
using uint16_t = unsigned short;
using uint8_t = unsigned char;

// strings
using wstring_t = wchar_t*;
using string_t = char*;

// transfer
struct data_t {
   uint64_t m_gui_base;
   uint64_t m_gui_full;
   uint64_t m_memory;
   uint64_t m_kernel;
};

// includes
#include "helpers/os_helper.hpp"
#include "helpers/io_helper.hpp"
#include "helpers/nt_helper.hpp"
