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
void clearSpinResult(::natashapb::SpinResult& sr) {
    sr.Clear();
}

}  // namespace natasha