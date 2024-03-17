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

  MaObject* MaObject::scopeGetModule() const {
    return this->scopeModule;
  }

  void MaObject::scopeSetModule(MaObject* module) {
    this->scopeModule = module;
  }

  MaObject* MaObject::CreateScope(MaMa* M,
                                  MaObject* outerScope,
                                  MaObject* module) {
    const auto scopeObject = MaObject::Instance(M, M->scope_structure_object);
    scopeObject->scopeSetOuter(outerScope);
    scopeObject->scopeSetModule(module);
    return scopeObject;
  }
} // namespace mavka::mama