#include "../mama.h"

namespace mavka::mama {
  // дізнатись
  MaValue MaStructure_DiscoverNativeDiiaFn(MaMa* M,
                                           MaObject* o,
                                           MaArgs* args,
                                           const MaLocation& location) {
    const auto cell = args->Get(0, "значення");
    if (cell.isEmpty()) {
      return MaValue::Empty();
    }
    if (cell.isNumber()) {
      return MaValue::Object(M->number_structure_object);
    }
    if (cell.isYes() || cell.isNo()) {
      return MaValue::Object(M->logical_structure_object);
    }
    if (cell.isObject()) {
      return MaValue::Object(cell.asObject()->getStructure());
    }
    return MaValue::Empty();
  }

  MaObject* MaStructure::Create(MaMa* M, const std::string& name) {
    const auto structure = new MaStructure();
    structure->name = name;
    const auto structure_o = new MaObject();
    structure_o->type = M->structure_structure_object;
    structure_o->d.structure = structure;
    return structure_o;
  }

  std::string MaStructure::getName() const {
    return this->name;
  }

  void MaStructure::Init(MaMa* M) {
    const auto structure_structure_object = MaStructure::Create(M, "Структура");
    M->structure_structure_object = structure_structure_object;
    M->global_scope->SetSubject("Структура", structure_structure_object);
  }

  void MaStructure::Init2(MaMa* M) {
    M->structure_structure_object->setProperty(
        M, "дізнатись",
        MaDiia::Create(M, "дізнатись", MaStructure_DiscoverNativeDiiaFn,
                       M->structure_structure_object));
  }
} // namespace mavka::mama