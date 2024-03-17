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
    this->moduleRoot = root;
  }

  MaCode* MaObject::moduleGetCode() const {
    return this->moduleCode;
  }

  void MaObject::moduleSetCode(MaCode* code) {
    this->moduleCode = code;
  }

  MaObject* MaObject::CreateModule(MaMa* M, const std::string& name) {
    const auto moduleObject = MaObject::Instance(M, M->module_structure_object);
    moduleObject->moduleSetName(name);
    return moduleObject;
  }

  void InitModule(MaMa* M) {
    const auto module_structure_object = MaObject::CreateStructure(M, "Модуль");
    M->global_scope->setProperty(M, "Модуль", module_structure_object);
    M->module_structure_object = module_structure_object;
  }
} // namespace mavka::mama