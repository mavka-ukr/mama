#include "../mama.h"

namespace mavka::mama {
  MaCell MaStructure_GetHandler(MaMa* M, MaObject* o, const std::string& name) {
    if (name == "назва") {
      return MaCell::Object(MaText::Create(M, o->d.structure->name));
    }
    return o->GetPropertyOrEmpty(name);
  }

  // дізнатись
  MaCell MaStructure_DiscoverNativeDiiaFn(MaMa* M,
                                          MaObject* o,
                                          MaArgs* args,
                                          const MaLocation& location) {
    const auto cell = args->Get(0, "значення");
    if (IS_EMPTY(cell)) {
      RETURN_EMPTY();
    }
    if (IS_NUMBER(cell)) {
      RETURN_OBJECT(M->number_structure_object);
    }
    if (IS_YES(cell) || IS_NO(cell)) {
      RETURN_OBJECT(M->logical_structure_object);
    }
    RETURN_OBJECT(cell.v.object->structure);
  }

  MaObject* MaStructure::Create(MaMa* M, const std::string& name) {
    const auto structure = new MaStructure();
    structure->name = name;
    const auto structure_object = new MaObject();
    structure_object->type = MA_OBJECT_STRUCTURE;
    structure_object->d.structure = structure;
    structure_object->structure = structure_object;
    structure_object->get = MaStructure_GetHandler;
    structure_object->call = [](MaMa* M, MaObject* o, MaArgs* args,
                                MaLocation location) {
      const auto object = MaObject::Instance(M, MA_OBJECT, o, nullptr);
      for (int i = 0; i < o->d.structure->params.size(); ++i) {
        const auto& param = o->d.structure->params[i];
        const auto arg_value = args->Get(i, param.name, param.default_value);
        object->SetProperty(param.name, arg_value);
      }
      return MaCell::Object(object);
    };
    return structure_object;
  }

  void MaStructure::Init(MaMa* M) {
    const auto structure_structure_object = MaStructure::Create(M, "Структура");
    M->structure_structure_object = structure_structure_object;
    M->global_scope->SetSubject("Структура", structure_structure_object);
    structure_structure_object->structure = structure_structure_object;
  }

  void MaStructure::Init2(MaMa* M) {
    M->structure_structure_object->SetProperty(
        "дізнатись",
        MaDiiaNative::Create(M, "дізнатись", MaStructure_DiscoverNativeDiiaFn,
                             M->structure_structure_object));
  }
} // namespace mavka::mama