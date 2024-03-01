#include "mama.h"

namespace mavka::mama {
  MaMa* MaMa::Create() {
    const auto M = new MaMa();
    M->global_scope = new MaScope(nullptr);
    MaStructure::Init(M);
    MaObject::Init(M);
    MaDiia::Init(M);
    MaModule::Init(M);
    InitNumber(M);
    InitLogical(M);
    MaText::Init(M);
    MaList::Init(M);
    MaDict::Init(M);
    MaStructure::Init2(M);
    return M;
  }
} // namespace mavka::mama