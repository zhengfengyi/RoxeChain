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

struct Account2Amt {
  std::map<name, uint64_t> dst2amt; // is token bound to pool

  ROXELIB_SERIALIZE(Account2Amt, (dst2amt))
};

struct BTokenStore {
  std::string names;
  std::string symbol ;
  uint8_t decimals ;
  std::map<name, uint64_t> balance;
  std::map<name, Account2Amt> allowance;
  uint64_t totalSupply;
  ROXELIB_SERIALIZE(BTokenStore, (names)(symbol)(decimals)(balance)(allowance)(totalSupply))
};

struct [[roxe::table("tokenstore"), roxe::contract("eoswap")]] BTokenStorage {
  std::map<namesym, BTokenStore> tokens;
  ROXELIB_SERIALIZE(BTokenStorage, (tokens))
};

typedef roxe::singleton<"tokenstore"_n, BTokenStorage> BTokenStorageSingleton;