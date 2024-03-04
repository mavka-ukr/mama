#include "../mama.h"

namespace mavka::mama {
  MaObject* MaModule::Create(MaMa* M, const std::string& name) {
    const auto module = new MaModule();
    module->name = name;
    const auto module_object = MaObject::Instance(
        M, MA_OBJECT_MODULE, M->module_structure_object, module);
    return module_object;
  }

  void MaModule::Init(MaMa* M) {
    const auto module_structure_object = MaStructure::Create(M, "Модуль");
    M->global_scope->SetSubject("Модуль",
                                MA_MAKE_OBJECT(module_structure_object));
    M->module_structure_object = module_structure_object;
  }
} // namespace mavka::mama