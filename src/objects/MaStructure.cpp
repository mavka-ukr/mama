#include "../mama.h"

namespace mavka::mama {
  // дізнатись
  MaValue MaStructure_DiscoverNativeDiiaFn(MaMa* M,
                                           MaObject* scope,
                                           MaObject* diiaObject,
                                           MaObject* args,
                                           size_t li) {
    const auto cell = args->getArg(M, "0", "значення");
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

  MaObject* MaObject::CreateStructure(MaMa* M, const std::string& name) {
    const auto structureObject = new MaObject();
    structureObject->structure = M->structure_structure_object;
    structureObject->structureSetName(name);
#if MAMA_GC_DEBUG
    std::cout << "[GC] created " << structureObject->getPrettyString(M) << " "
              << (void*)structureObject << std::endl;
#endif
    return structureObject;
  }

  std::string MaObject::structureGetName() const {
    return this->structureName;
  }

  void MaObject::structureSetName(const std::string& name) {
    this->structureName = name;
  }

  std::vector<MaDiiaParam> MaObject::structureGetParams() {
    return this->structureParams;
  }

  void MaObject::structurePushParam(const MaDiiaParam& param) {
    this->structureParams.push_back(param);
  }

  std::vector<MaObject*> MaObject::structureGetMethods() {
    return this->structureMethods;
  }

  void MaObject::structurePushMethod(MaObject* method) {
    this->structureMethods.push_back(method);
  }

  void InitStructure(MaMa* M) {
    const auto structure_structure_object =
        MaObject::CreateStructure(M, "Структура");
    M->structure_structure_object = structure_structure_object;
    M->global_scope->setProperty(M, "Структура", structure_structure_object);
  }

  void InitStructure2(MaMa* M) {
    M->structure_structure_object->setProperty(
        M, "дізнатись",
        MaObject::CreateDiiaNativeFn(M, "дізнатись",
                                     MaStructure_DiscoverNativeDiiaFn,
                                     M->structure_structure_object));
  }
} // namespace mavka::mama