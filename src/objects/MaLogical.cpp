#include "../mama.h"

namespace mavka::mama {
  MaValue MaLogical_Structure_MagCallNativeDiiaFn(MaMa* M,
                                                  MaObject* scope,
                                                  MaObject* diiaObject,
                                                  MaObject* args,
                                                  size_t li) {
    const auto value = args->getArg(M, "0", "значення");
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
    return MaValue::Error(
        MaError::Create(MaValue::Object(MaObject::CreateText(
                            M, "Неможливо перетворити на логічне.")),
                        li));
  }

  void InitLogical(MaMa* M) {
    const auto logicalStructureObject = MaObject::CreateStructure(M, "логічне");
    M->global_scope->setProperty(M, "логічне", logicalStructureObject);
    M->logical_structure_object = logicalStructureObject;
    logicalStructureObject->setProperty(
        M, MAG_CALL,
        MaObject::CreateDiiaNativeFn(M, MAG_CALL,
                                     MaLogical_Structure_MagCallNativeDiiaFn,
                                     logicalStructureObject));
  }
} // namespace mavka::mama