#include "../mama.h"

namespace mavka::mama {
  MaValue MaNumber_Structure_MagCallNativeDiiaFn(MaMa* M,
                                                 MaObject* o,
                                                 MaArgs* args,
                                                 const MaLocation& location) {
    const auto arg_value_v = args->Get(0, "значення");
    if (arg_value_v.IsEmpty()) {
      return MaValue::Number(0);
    }
    if (arg_value_v.IsNumber()) {
      return arg_value_v;
    }
    if (arg_value_v.IsYes()) {
      return MaValue::Number(1);
    }
    if (arg_value_v.IsNo()) {
      return MaValue::Number(0);
    }
    if (arg_value_v.IsObject()) {
      if (arg_value_v.AsObject()->HasProperty(M, MAG_NUMBER)) {
        return arg_value_v.AsObject()
            ->GetProperty(M, MAG_NUMBER)
            .Call(M, {}, location);
      }
    }
    return MaValue::Error(new MaError(
        MaValue::Object(MaText::Create(M, "Неможливо перетворити на число.")),
        location));
  }

  void InitNumber(MaMa* M) {
    const auto number_structure_object = MaStructure::Create(M, "число");
    M->global_scope->SetSubject("число", number_structure_object);
    M->number_structure_object = number_structure_object;
    number_structure_object->SetProperty(M,
        MAG_CALL,
        MaNative::Create(M, MAG_CALL, MaNumber_Structure_MagCallNativeDiiaFn,
                         number_structure_object));
  }
} // namespace mavka::mama