
#pragma once

 #include <common/defines.hpp>

struct OwnableStore {
   address _OWNER_;
   address _NEW_OWNER_;
   ROXELIB_SERIALIZE(OwnableStore, (_OWNER_)(_NEW_OWNER_))
};

struct ReentrancyGuardStore {
   bool _ENTERED_;
   ROXELIB_SERIALIZE(ReentrancyGuardStore, (_ENTERED_))
};
