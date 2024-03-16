#include "../mama.h"

namespace mavka::mama {
  MaObject* MaDiia::Create(MaMa* M,
                           const std::string& name,
                           MaCode* code,
                           MaObject* me) {
    const auto diia = new MaDiia();
    diia->name = name;
    diia->code = code;
    diia->me = me;
    const auto diia_object =
        MaObject::Instance(M, M->diia_structure_object, diia);
    return diia_object;
  }

  MaObject* MaDiia::Create(MaMa* M,
                           const std::string& name,
                           const std::function<NativeFn>& fn,
                           MaObject* me) {
    const auto diia = new MaDiia();
    diia->name = name;
    diia->fn = fn;
    diia->me = me;
    const auto diia_object =
        MaObject::Instance(M, M->diia_structure_object, diia);
    return diia_object;
  }

  MaObject* MaDiia::Bind(MaMa* M, MaObject* object) {
    const auto diia_object = MaDiia::Create(M, this->name, this->code, object);
    diia_object->d.diia->scope = this->scope;
    diia_object->d.diia->params = this->params;
    return diia_object;
  }

  void MaDiia::Init(MaMa* M) {
    const auto diia_structure_object = MaStructure::Create(M, "Дія");
    M->global_scope->setProperty(M, "Дія", diia_structure_object);
    M->diia_structure_object = diia_structure_object;
  }
} // namespace mavka::mama