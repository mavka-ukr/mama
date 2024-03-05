#include "../mama.h"

namespace mavka::mama {
  MaCell MaNumber_Structure_MagCallNativeDiiaFn(MaMa* M,
                                                MaObject* o,
                                                MaArgs* args) {
    const auto value_cell = args->Get(0, "значення");
    if (IS_EMPTY(value_cell)) {
      RETURN_NUMBER(0);
    }
    if (IS_NUMBER(value_cell)) {
      RETURN(value_cell);
    }
    if (IS_YES(value_cell)) {
      RETURN_NUMBER(1);
    }
    if (IS_NO(value_cell)) {
      RETURN_NUMBER(0);
    }
    if (IS_OBJECT(value_cell)) {
      if (value_cell.v.object->HasProperty(MAG_NUMBER)) {
        return value_cell.v.object->GetProperty(MAG_NUMBER).Call(M, {}, {});
      }
    }
    RETURN_ERROR(new MaError(
        MaCell::Object(MaText::Create(M, "Неможливо перетворити на число."))));
  }

  void InitNumber(MaMa* M) {
    const auto number_structure_object = MaStructure::Create(M, "число");
    M->global_scope->SetSubject("число",
                                MaCell::Object(number_structure_object));
    M->number_structure_object = number_structure_object;
    number_structure_object->SetProperty(
        MAG_CALL, MaCell::Object(MaDiiaNative::Create(
                      M, MAG_CALL, MaNumber_Structure_MagCallNativeDiiaFn,
                      number_structure_object)));
  }
} // namespace mavka::mama