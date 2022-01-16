#pragma once

// warnings
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

// includes
#include "helpers/os_helper.hpp"
#include "helpers/log_helper.hpp"
