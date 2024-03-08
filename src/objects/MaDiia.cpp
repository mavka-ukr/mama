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
        MaObject::Instance(M, MA_OBJECT_DIIA, M->diia_structure_object, diia);
    diia_object->call = [](MaMa* M, MaObject* diia_o, MaArgs* args,
                           MaLocation location) {
      const auto diia_scope = new MaScope(diia_o->AsDiia()->scope);
      READ_TOP_FRAME();
      frame->scope = diia_scope;
      if (diia_o->AsDiia()->GetMe()) {
        frame->scope->SetSubject("я", diia_o->AsDiia()->GetMe());
      }
      for (int i = 0; i < diia_o->AsDiia()->GetParams().size(); ++i) {
        const auto& param = diia_o->d.diia->params[i];
        const auto arg_value = args->Get(i, param.name, param.default_value);
        frame->scope->SetSubject(param.name, arg_value);
      }
      const auto result = M->Run(diia_o->d.diia->code);
      // todo: gc
      return result;
    };
    return diia_object;
  }

  MaObject* MaDiia::Bind(MaMa* M, MaObject* object) {
    const auto diia_object = MaDiia::Create(M, this->name, this->code, object);
    diia_object->d.diia->scope = this->scope;
    diia_object->d.diia->fm = this->fm;
    diia_object->d.diia->params = this->params;
    return diia_object;
  }

  MaObject* MaNative::Create(MaMa* M,
                             const std::string& name,
                             const std::function<NativeFn>& native_fn,
                             MaObject* me) {
    const auto native = new MaNative();
    native->name = name;
    native->fn = native_fn;
    native->me = me;
    const auto native_object = MaObject::Instance(
        M, MA_OBJECT_NATIVE, M->diia_structure_object, native);
    native_object->call = native->fn;
    return native_object;
  }

  void MaDiia::Init(MaMa* M) {
    const auto diia_structure_object = MaStructure::Create(M, "Дія");
    M->global_scope->SetSubject("Дія", diia_structure_object);
    M->diia_structure_object = diia_structure_object;
  }
} // namespace mavka::mama