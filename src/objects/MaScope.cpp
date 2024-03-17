#include "../mama.h"

namespace mavka::mama {
  bool MaObject::scopeHasOuter() const {
    return this->scopeGetOuter() != nullptr;
  }

  MaObject* MaObject::scopeGetOuter() const {
    return this->scopeOuter;
  }

  void MaObject::scopeSetOuter(MaObject* outer) {
    this->scopeOuter = outer;
  }

  MaObject* MaObject::CreateScope(MaMa* M) {
    const auto scopeObject = MaObject::Instance(M, M->scope_structure_object);
    return scopeObject;
  }
} // namespace mavka::mama