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
                                                MaArgs* args,
                                                const MaLocation& location) {
    const auto value = args->get(0, "значення");
    if (value.isObject()) {
      if (value.asObject()->isBytes(M)) {
        return value;
      }
      return value.asObject()->callMagWithoutValue(M, location, MAG_BYTES);
    }
    return MaValue::Error(MaError::Create(
        MaValue::Object(MaText::Create(M, "Неможливо перетворити на байти.")),
        M->call_stack.top()->module, location));
  }

  void MaBytes::Init(MaMa* M) {
    const auto bytes_structure_object = MaStructure::Create(M, "байти");
    M->global_scope->setSubject("байти", bytes_structure_object);
    M->bytes_structure_object = bytes_structure_object;
    bytes_structure_object->setProperty(
        M, MAG_CALL,
        MaDiia::Create(M, MAG_CALL, MaBytes_Structure_MagCallNativeDiiaFn,
                       bytes_structure_object));
  }
} // namespace mavka::mama