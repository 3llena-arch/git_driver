#pragma once

struct os_helper_t {
   enum state_t : uint32_t {
      status_okay = 0,
      status_warning,
      status_error,
   };

   auto to_string(
      state_t os
   ) {
      string_t locale_t[ 3 ] = {
         "status_okay",
         "status_warning",
         "status_error"
      };
      return locale_t[ os ];
   }
} __os_helper;

extern os_helper_t* os;
