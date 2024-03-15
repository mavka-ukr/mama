#include "../mama.h"

namespace mavka::mama {
  MaValue MaNumber_Structure_MagCallNativeDiiaFn(MaMa* M,
                                                 MaObject* o,
                                                 MaArgs* args,
                                                 const MaLocation& location) {
    const auto arg_value_v = args->get(0, "значення");
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
          .call(M, {}, location);
    }
    return MaValue::Error(MaError::Create(
        MaValue::Object(MaText::Create(M, "Неможливо перетворити на число.")),
        M->call_stack.top()->module, location));
  }

  void InitNumber(MaMa* M) {
    const auto number_structure_object = MaStructure::Create(M, "число");
    M->global_scope->setSubject("число", number_structure_object);
    M->number_structure_object = number_structure_object;
    number_structure_object->setProperty(
        M, MAG_CALL,
        MaDiia::Create(M, MAG_CALL, MaNumber_Structure_MagCallNativeDiiaFn,
                       number_structure_object));
  }
} // namespace mavka::mama