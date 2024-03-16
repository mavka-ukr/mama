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
    return MaObject::Instance(M, M->diia_structure_object, diia);
  }

  MaObject* MaDiia::Create(MaMa* M,
                           const std::string& name,
                           const std::function<NativeFn>& fn,
                           MaObject* me) {
    const auto diia = new MaDiia();
    diia->name = name;
    diia->fn = fn;
    diia->me = me;
    return MaObject::Instance(M, M->diia_structure_object, diia);
  }

  MaObject* MaDiia::Bind(MaMa* M, MaObject* object) {
    const auto diia = new MaDiia();
    diia->name = this->name;
    diia->code = this->code;
    diia->fn = this->fn;
    diia->me = object;
    diia->outerScope = this->outerScope;
    diia->params = this->params;
    diia->param_index_map = this->param_index_map;
    return MaObject::Instance(M, M->diia_structure_object, diia);
  }

  void MaDiia::Init(MaMa* M) {
    const auto diia_structure_object = MaStructure::Create(M, "Дія");
    M->global_scope->setProperty(M, "Дія", diia_structure_object);
    M->diia_structure_object = diia_structure_object;
  }
} // namespace mavka::mama