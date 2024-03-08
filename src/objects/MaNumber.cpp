#include "../mama.h"

namespace mavka::mama {
  MaCell MaNumber_Structure_MagCallNativeDiiaFn(MaMa* M,
                                                MaObject* o,
                                                MaArgs* args,
                                                const MaLocation& location) {
    const auto value_cell = args->Get(0, "значення");
    if (value_cell.IsEmpty()) {
      return MaCell::Number(0);
    }
    if (value_cell.IsNumber()) {
      return value_cell;
    }
    if (value_cell.IsYes()) {
      return MaCell::Number(1);
    }
    if (value_cell.IsNo()) {
      return MaCell::Number(0);
    }
    if (value_cell.IsObject()) {
      if (value_cell.AsObject()->HasProperty(MAG_NUMBER)) {
        return value_cell.AsObject()
            ->GetProperty(MAG_NUMBER)
            .Call(M, {}, location);
      }
    }
    return MaCell::Error(new MaError(
        MaCell::Object(MaText::Create(M, "Неможливо перетворити на число.")),
        location));
  }

  void InitNumber(MaMa* M) {
    const auto number_structure_object = MaStructure::Create(M, "число");
    M->global_scope->SetSubject("число", number_structure_object);
    M->number_structure_object = number_structure_object;
    number_structure_object->SetProperty(
        MAG_CALL,
        MaNative::Create(M, MAG_CALL, MaNumber_Structure_MagCallNativeDiiaFn,
                         number_structure_object));
  }
} // namespace mavka::mama