/**
 *  @file
 *  @copyright defined in actc/LICENSE
 */
#pragma once

#include <fc/crypto/sha256.hpp>

struct hello;

namespace actc {

   class net_plugin_impl;
   struct handshake_message;

   namespace chain_apis {
      class read_only;
   }

namespace chain {

   struct chain_id_type : public fc::sha256 {
      using fc::sha256::sha256;

      template<typename T>
      inline friend T& operator<<( T& ds, const chain_id_type& cid ) {
        ds.write( cid.data(), cid.data_size() );
        return ds;
      }

      template<typename T>
      inline friend T& operator>>( T& ds, chain_id_type& cid ) {
        ds.read( cid.data(), cid.data_size() );
        return ds;
      }

      void reflector_init()const;

      private:
         chain_id_type() = default;

         // Some exceptions are unfortunately necessary:
         template<typename T>
         friend T fc::variant::as()const;

         friend class actc::chain_apis::read_only;

         friend class actc::net_plugin_impl;
         friend struct actc::handshake_message;
   };

} }  // namespace actc::chain

namespace fc {
  class variant;
  void to_variant(const actc::chain::chain_id_type& cid, fc::variant& v);
  void from_variant(const fc::variant& v, actc::chain::chain_id_type& cid);
} // fc
