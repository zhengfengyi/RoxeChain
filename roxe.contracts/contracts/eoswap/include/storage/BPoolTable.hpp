// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
#pragma once
#include <common/BType.hpp>

struct Record {
  bool bound;  // is token bound to pool
  uint64_t index;  // private
  uint64_t denorm; // denormalized weight
  uint64_t balance;
  extended_symbol exsym;
  ROXELIB_SERIALIZE(Record, (bound)(index)(denorm)(balance)(exsym))
};

struct  BPoolStore {
  bool mutex;

  name factory;    // BFactory name to push token exitFee to
  name controller; // has CONTROL role
  bool publicSwap;    // true if PUBLIC can call SWAP functions

  // `setSwapFee` and `finalize` require CONTROL
  // `finalize` sets `PUBLIC can SWAP`, `PUBLIC can JOIN`
  uint64_t swapFee;
  bool finalized;

  std::vector<namesym> tokens;
  std::map<namesym, Record> records;
  uint64_t totalWeight;

  ROXELIB_SERIALIZE(BPoolStore, (mutex)(factory)(controller)(publicSwap)(swapFee)(finalized)(tokens)(records)(totalWeight))
};


struct [[roxe::table("poolstore"), roxe::contract("eoswap")]] BPoolStorage {
  std::map<name, BPoolStore> pools;
  ROXELIB_SERIALIZE(BPoolStorage, (pools))
};

typedef roxe::singleton<"poolstore"_n, BPoolStorage> BPoolStorageSingleton;


struct [[roxe::table, roxe::contract("eoswap")]] pool_storage {
   name      pool;
   BPoolStore pools;
   uint64_t  primary_key() const { return pool.value; }
};

typedef roxe::multi_index<"pools"_n, pool_storage> pool_storage_table;

