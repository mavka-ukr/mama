#include "../mama.h"

namespace mavka::mama {
  MaObject* MaBytes::Create(MaMa* M, const std::vector<uint8_t>& data) {
    const auto bytes = new MaBytes();
    bytes->data = data;
    const auto bytes_object =
        MaObject::Instance(M, M->bytes_structure_object, bytes);
    return bytes_object;
  }

  MaValue MaBytes_Structure_MagCallNativeDiiaFn(MaMa* M,
                                                MaObject* native_o,
                                                MaObject* args,
                                                size_t li) {
    const auto value = args->getArg(M, "0", "значення");
    if (value.isObject()) {
      if (value.asObject()->isBytes(M)) {
        return value;
      }
      return value.asObject()->callMagWithoutValue(M, li, MAG_BYTES);
    }
    return MaValue::Error(MaError::Create(
        MaValue::Object(MaText::Create(M, "Неможливо перетворити на байти.")),
        li));
  }

  void MaBytes::Init(MaMa* M) {
    const auto bytes_structure_object = MaStructure::Create(M, "байти");
    M->global_scope->setProperty(M, "байти", bytes_structure_object);
    M->bytes_structure_object = bytes_structure_object;
    bytes_structure_object->setProperty(
        M, MAG_CALL,
        MaDiia::Create(M, MAG_CALL, MaBytes_Structure_MagCallNativeDiiaFn,
                       bytes_structure_object));
  }
} // namespace mavka::mama