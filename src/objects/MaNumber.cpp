#include "../mama.h"

namespace mavka::mama {
  MaCell MaNumber_Structure_MagCallNativeDiiaFn(MaMa* M,
                                                MaObject* o,
                                                MaArgs* args) {
    const auto value_cell = MA_ARGS_GET(args, 0, "значення", MA_MAKE_EMPTY());
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
        return ma_call_handler(M, value_cell.v.object->GetProperty(MAG_NUMBER),
                               {}, {});
      }
    }
    RETURN_ERROR(new MaError(
        MA_MAKE_OBJECT(MaText::Create(M, "Неможливо перетворити на число."))));
  }

  void InitNumber(MaMa* M) {
    const auto number_structure_object = MaStructure::Create(M, "число");
    M->global_scope->SetSubject("число",
                                MA_MAKE_OBJECT(number_structure_object));
    M->number_structure_object = number_structure_object;
    number_structure_object->SetProperty(
        MAG_CALL, MA_MAKE_OBJECT(MaDiiaNative::Create(
                      M, MAG_CALL, MaNumber_Structure_MagCallNativeDiiaFn,
                      number_structure_object)));
  }
} // namespace mavka::mama