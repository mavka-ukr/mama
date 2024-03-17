#include "../mama.h"

namespace mavka::mama {
  MaValue MaNumber_Structure_MagCallNativeDiiaFn(MaMa* M,
                                                 MaObject* scope,
                                                 MaObject* diiaObject,
                                                 MaObject* args,
                                                 size_t li) {
    const auto arg_value_v = args->getArg(M, "0", "значення");
    if (arg_value_v.isEmpty()) {
      return MaValue::Number(0);
    }
    if (arg_value_v.isNumber()) {
      return arg_value_v;
    }
    if (arg_value_v.isYes()) {
      return MaValue::Number(1);
    }
    if (arg_value_v.isNo()) {
      return MaValue::Number(0);
    }
    if (arg_value_v.isObject()) {
      return arg_value_v.asObject()
          ->getProperty(M, MAG_NUMBER)
          .call(M, scope, {}, li);
    }
    return MaValue::Error(
        MaError::Create(MaValue::Object(MaObject::CreateText(
                            M, "Неможливо перетворити на число.")),
                        li));
  }

  void InitNumber(MaMa* M) {
    const auto number_structure_object = MaObject::CreateStructure(M, "число");
    M->global_scope->setProperty(M, "число", number_structure_object);
    M->number_structure_object = number_structure_object;
    number_structure_object->setProperty(
        M, MAG_CALL,
        MaObject::CreateDiiaNativeFn(M, MAG_CALL,
                                     MaNumber_Structure_MagCallNativeDiiaFn,
                                     number_structure_object));
  }
} // namespace mavka::mama