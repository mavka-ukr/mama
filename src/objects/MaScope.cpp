#include "../mama.h"

namespace mavka::mama {
  bool MaObject::scopeHasOuter() const {
    return this->scopeOuter != nullptr;
  }

  MaObject* MaObject::scopeGetOuter() const {
    return this->scopeOuter;
  }

  void MaObject::scopeSetOuter(MaObject* outer) {
    if (outer) {
      outer->retain();
    }
    if (this->scopeOuter) {
      this->scopeOuter->release();
    }
    this->scopeOuter = outer;
  }

  MaObject* MaObject::scopeGetModule() const {
    return this->scopeModule;
  }

  void MaObject::scopeSetModule(MaObject* module) {
    if (module) {
      module->retain();
    }
    if (this->scopeModule) {
      this->scopeModule->release();
    }
    this->scopeModule = module;
  }
} // namespace mavka::mama