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
    const auto main_module_object = MaModule::Create(M, "мавка");
    const auto main_frame =
        new MaFrame(M->global_scope, main_module_object, main_module_object);
    FRAME_PUSH(main_frame);
    return M;
  }
} // namespace mavka::mama