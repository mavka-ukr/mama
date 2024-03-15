#include "../mama.h"

namespace mavka::mama {
  MaValue MaLogical_Structure_MagCallNativeDiiaFn(MaMa* M,
                                                  MaObject* native_o,
                                                  MaArgs* args,
                                                  const MaLocation& location) {
    const auto value = args->get(0, "значення");
    if (value.isEmpty()) {
      return MaValue::No();
    }
    if (value.isNumber()) {
      return value.asNumber() == 0 ? MaValue::No() : MaValue::Yes();
    }
    if (value.isYes() || value.isNo()) {
      return value;
    }
    if (value.isObject()) {
      return MaValue::Yes();
    }
    return MaValue::Error(MaError::Create(
        MaValue::Object(MaText::Create(M, "Неможливо перетворити на логічне.")),
        M->call_stack.top()->module, location));
  }

  void InitLogical(MaMa* M) {
    const auto logical_structure_object = MaStructure::Create(M, "логічне");
    M->global_scope->setSubject("логічне", logical_structure_object);
    M->logical_structure_object = logical_structure_object;
    logical_structure_object->setProperty(
        M, MAG_CALL,
        MaDiia::Create(M, MAG_CALL, MaLogical_Structure_MagCallNativeDiiaFn,
                       logical_structure_object));
  }
} // namespace mavka::mama