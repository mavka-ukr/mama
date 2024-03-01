#include "../mama.h"

namespace mavka::mama {
  MaCell number_structure_object_mag_call_diia_native_fn(MaMa* M,
                                                         MaObject* o,
                                                         MaArgs* args) {
    const auto cell = MA_ARGS_GET(args, 0, "значення", MA_MAKE_EMPTY());
    if (IS_EMPTY(cell)) {
      RETURN_NUMBER(0);
    }
    if (IS_NUMBER(cell)) {
      RETURN(cell);
    }
    if (IS_YES(cell)) {
      RETURN_NUMBER(1);
    }
    if (IS_NO(cell)) {
      RETURN_NUMBER(0);
    }
    if (IS_OBJECT(cell)) {
      if (OBJECT_HAS(cell.v.object, MAG_NUMBER)) {
        return ma_call_handler(M, cell.v.object->properties[MAG_NUMBER], {},
                               {});
      }
    }
    M->throw_cell =
        MA_MAKE_OBJECT(MaText::Create(M, "Неможливо перетворити на число."));
    throw MaException();
  }

  void InitNumber(MaMa* M) {
    const auto number_structure_object = MaStructure::Create(M, "число");
    M->global_scope->set_variable("число",
                                  MA_MAKE_OBJECT(number_structure_object));
    M->number_structure_object = number_structure_object;
    ma_object_set(
        number_structure_object, MAG_CALL,
        MA_MAKE_OBJECT(MaDiiaNative::Create(
            M, MAG_CALL, number_structure_object_mag_call_diia_native_fn,
            number_structure_object)));
  }
} // namespace mavka::mama