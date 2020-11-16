#pragma once
#include "murmurhash.hpp"
#include <roxe/crypto.hpp>
#include <roxe/roxe.hpp>



uint64_t get_hash_key(roxe::checksum256 hash) {
  const uint64_t *p64 = reinterpret_cast<const uint64_t *>(hash.extract_as_byte_array().data());
  return p64[0] ^ p64[1] ^ p64[2] ^ p64[3];
}

uint32_t random(void *seed, size_t len) {
  roxe::checksum256 rand256;
  rand256 = roxe::sha256(static_cast<const char *>(seed), len);
  auto res = murmur_hash2(reinterpret_cast<const char *>(&rand256),
                          sizeof(rand256) / sizeof(char));
  return res;
}
