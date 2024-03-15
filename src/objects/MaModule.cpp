#include "../mama.h"

namespace mavka::mama {
  MaObject* MaModule::Create(MaMa* M, const std::string& name) {
    const auto module = new MaModule();
    module->name = name;
    const auto module_object =
        MaObject::Instance(M, M->module_structure_object, module);
    return module_object;
  }

  MaValue MaModule::getProperty(mavka::mama::MaMa* M,
                                mavka::mama::MaObject* object,
                                const std::string& name) {
    if (name == "назва") {
      return MaValue::Object(MaText::Create(M, this->name));
    }
    if (this->properties.contains(name)) {
      return this->properties[name];
    }
    return MaValue::Empty();
  }

  void MaModule::Init(MaMa* M) {
    const auto module_structure_object = MaStructure::Create(M, "Модуль");
    M->global_scope->SetSubject("Модуль", module_structure_object);
    M->module_structure_object = module_structure_object;
  }
} // namespace mavka::mama