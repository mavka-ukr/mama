#include "../mama.h"

namespace mavka::mama {
  std::string MaObject::moduleGetName() const {
    return this->moduleName;
  }

  void MaObject::moduleSetName(const std::string& name) {
    this->moduleName = name;
  }

  MaObject* MaObject::moduleGetRoot() const {
    return this->moduleRoot;
  }

  void MaObject::moduleSetRoot(MaObject* root) {
    if (root) {
      root->retain();
    }
    if (this->moduleRoot != nullptr) {
      this->moduleRoot->release();
    }
    this->moduleRoot = root;
  }

  MaCode* MaObject::moduleGetCode() const {
    return this->moduleCode;
  }

  void MaObject::moduleSetCode(MaCode* code) {
    this->moduleCode = code;
  }

  void InitModule(MaMa* M) {
    const auto moduleStructureObject = M->createStructure("Модуль");
    M->global_scope->setProperty(M, "Модуль", moduleStructureObject);
    M->module_structure_object = moduleStructureObject;
  }
} // namespace mavka::mama