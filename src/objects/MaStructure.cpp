#include "../mama.h"

namespace mavka::mama {
  MaCell MaStructure_GetHandler(MaMa* M, MaObject* o, const std::string& name) {
    if (name == "назва") {
      return MA_MAKE_OBJECT(MaText::Create(M, o->d.structure->name));
    }
    if (!o->properties.contains(name)) {
      M->throw_cell = MA_MAKE_OBJECT(MaText::Create(
          M,
          "Властивість \"" + name + "\" не визначено для типу \"Структура\"."));
      throw MaException();
    }
    return o->properties[name];
  }

  // дізнатись
  MaCell MaStructure_DiscoverNativeDiiaFn(MaMa* M, MaObject* o, MaArgs* args) {
    const auto cell = MA_ARGS_GET(args, 0, "значення", MA_MAKE_EMPTY());
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
        const auto arg_value =
            MA_ARGS_GET(args, i, param.name, param.default_value);
        object->SetProperty(param.name, arg_value);
      }
      return MA_MAKE_OBJECT(object);
    };
    return structure_object;
  }

  void MaStructure::Init(MaMa* M) {
    const auto structure_structure_object = MaStructure::Create(M, "Структура");
    M->structure_structure_object = structure_structure_object;
    M->global_scope->SetSubject("Структура",
                                MA_MAKE_OBJECT(structure_structure_object));
    structure_structure_object->structure = structure_structure_object;
  }

  void MaStructure::Init2(MaMa* M) {
    M->structure_structure_object->SetProperty(
        "дізнатись", MA_MAKE_OBJECT(MaDiiaNative::Create(
                         M, "дізнатись", MaStructure_DiscoverNativeDiiaFn,
                         M->structure_structure_object)));
  }
} // namespace mavka::mama