#include "../mama.h"

namespace mavka::mama {
  MaObject* MaObject::CreateBytes(MaMa* M, const std::vector<uint8_t>& data) {
    const auto bytesObject = MaObject::Instance(M, M->bytes_structure_object);
    bytesObject->bytesData = data;
    return bytesObject;
  }

  MaValue MaBytes_Structure_MagCallNativeDiiaFn(MaMa* M,
                                                MaObject* diiaObject,
                                                MaObject* args,
                                                size_t li) {
    const auto value = args->getArg(M, "0", "значення");
    if (value.isObject()) {
      if (value.asObject()->isBytes(M)) {
        return value;
      }
      return value.asObject()->callMagWithoutValue(M, li, MAG_BYTES);
    }
    return MaValue::Error(
        MaError::Create(MaValue::Object(MaObject::CreateText(
                            M, "Неможливо перетворити на байти.")),
                        li));
  }

  void InitBytes(MaMa* M) {
    const auto bytes_structure_object = MaObject::CreateStructure(M, "байти");
    M->global_scope->setProperty(M, "байти", bytes_structure_object);
    M->bytes_structure_object = bytes_structure_object;
    bytes_structure_object->setProperty(
        M, MAG_CALL,
        MaObject::CreateDiiaNativeFn(M, MAG_CALL,
                                     MaBytes_Structure_MagCallNativeDiiaFn,
                                     bytes_structure_object));
  }
} // namespace mavka::mama