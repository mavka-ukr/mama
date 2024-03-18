#include "../mama.h"

namespace mavka::mama {
  void MaObject::bytesSetData(const std::vector<uint8_t>& data) {
    this->bytesData = data;
  }

  MaValue MaBytes_Structure_MagCallNativeDiiaFn(MaMa* M,
                                                MaObject* scope,
                                                MaObject* diiaObject,
                                                MaObject* args,
                                                size_t li) {
    const auto value = args->getArg(M, "0", "значення");
    if (value.isObject()) {
      if (value.asObject()->isBytes(M)) {
        return value;
      }
      return value.asObject()->callMagWithoutValue(M, scope, li, MAG_BYTES);
    }
    return MaValue::Error(MaError::Create(
        MaValue::Object(M->createText("Неможливо перетворити на байти.")), li));
  }

  void InitBytes(MaMa* M) {
    const auto bytesStructureObject = M->createStructure("байти");
    M->global_scope->setProperty(M, "байти", bytesStructureObject);
    M->bytes_structure_object = bytesStructureObject;
    bytesStructureObject->setProperty(
        M, MAG_CALL,
        M->createNativeDiia(MAG_CALL, MaBytes_Structure_MagCallNativeDiiaFn,
                            bytesStructureObject));
  }
} // namespace mavka::mama