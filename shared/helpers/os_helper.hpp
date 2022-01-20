#pragma once

struct os_helper_t {
   uint64_t m_mdl;
   uint64_t m_ctx;

   auto init(
      data_t* data
   ) {
      m_mdl = data->m_memory;
      m_ctx = data->m_kernel;
   }

   enum state_t : uint32_t {
      status_okay,
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

   template <typename type_t>
   auto call_fn(
      uint64_t address
   ) {
      return reinterpret_cast <type_t> 
         ( m_ctx + address );
   }

   auto print(
      string_t out,
      ...
   ) {
      if ( !m_ctx )
         return status_error;

      va_list va_out;
      __va_start( &va_out, out );
   
      call_fn <uint32_t( __stdcall* )(
         uint32_t id, 
         uint32_t flag, 
         string_t out,
         va_list va
      )> ( 0x11db80 )( 0, 0, out, va_out );

      va_out = static_cast <va_list> ( 0 );
      return status_okay;
   }
} __os_helper;

extern os_helper_t* os;
