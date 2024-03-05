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
    diia_object->call = [](MaMa* M, MaObject* o, MaArgs* args,
                           MaLocation location) {
      const auto diia_scope = new MaScope(o->d.diia->scope);
      READ_TOP_FRAME();
      frame->scope = diia_scope;
      if (o->d.diia->me) {
        frame->scope->SetSubject("я", MaCell::Object(o->d.diia->me));
      }
      for (int i = 0; i < o->d.diia->params.size(); ++i) {
        const auto& param = o->d.diia->params[i];
        const auto arg_value = args->Get(i, param.name, param.default_value);
        frame->scope->SetSubject(param.name, arg_value);
      }
      const auto result = ma_run(M, o->d.diia->code);
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

  MaObject* MaDiiaNative::Create(
      MaMa* M,
      const std::string& name,
      const std::function<DiiaNativeFn>& diia_native_fn,
      MaObject* me) {
    const auto diia_native = new MaDiiaNative();
    diia_native->name = name;
    diia_native->fn = diia_native_fn;
    diia_native->me = me;
    const auto diia_native_object = MaObject::Instance(
        M, MA_OBJECT_DIIA_NATIVE, M->diia_structure_object, diia_native);
    diia_native_object->call = [](MaMa* M, MaObject* o, MaArgs* args,
                                  MaLocation location) {
      return o->d.diia_native->fn(M, o, args);
    };
    return diia_native_object;
  }

  void MaDiia::Init(MaMa* M) {
    const auto diia_structure_object = MaStructure::Create(M, "Дія");
    M->global_scope->SetSubject("Дія", MaCell::Object(diia_structure_object));
    M->diia_structure_object = diia_structure_object;
  }
} // namespace mavka::mama