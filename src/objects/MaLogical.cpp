#include "../mama.h"

namespace mavka::mama {
  void InitLogical(MaMa* M) {
    const auto logical_structure_object = MaStructure::Create(M, "логічне");
    M->global_scope->SetSubject("логічне",
                                MA_MAKE_OBJECT(logical_structure_object));
    M->logical_structure_object = logical_structure_object;
  }
} // namespace mavka::mama