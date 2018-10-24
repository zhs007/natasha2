#include "../include/utils.h"
#include <fstream>
#include <streambuf>
#include <string>

namespace natasha {

// clearGameResultInfo
void clearGameResultInfo(::natashapb::GameResultInfo& gri) {
  gri.Clear();

  // gri.set_typegameresult(::natashapb::NO_WIN);
  gri.set_mul(1);
}

// clearSpinResult
void clearSpinResult(::natashapb::SpinResult& sr) { sr.Clear(); }

// setGameCtrlID
void setGameCtrlID(::natashapb::GameCtrlID& dest,
                   const ::natashapb::GameCtrlID& parent, CtrlID curCtrlID,
                   ::natashapb::GAMEMODTYPE curGameMod) {
  assert(parent.ctrlid() > 0);
  assert(parent.gamemod() != ::natashapb::NULL_MOD);

  if (parent.baseid() > 0) {
    dest.set_baseid(parent.baseid());
    dest.set_parentid(parent.ctrlid());
  } else {
    dest.set_baseid(parent.ctrlid());
    dest.set_parentid(parent.ctrlid());
  }

  dest.set_ctrlid(curCtrlID);
  dest.set_gamemod(curGameMod);
}

}  // namespace natasha