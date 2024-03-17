#include "../mama.h"

namespace mavka::mama {
  MaObject* MaObject::CreateDiia(MaMa* M,
                                 const std::string& name,
                                 MaCode* code,
                                 MaObject* me) {
    const auto diiaObject = MaObject::Instance(M, M->diia_structure_object);
    diiaObject->diiaSetName(name);
    diiaObject->diiaSetCode(code);
    diiaObject->diiaSetBoundObject(me);
    return diiaObject;
  }

  MaObject* MaObject::CreateDiiaNativeFn(MaMa* M,
                                         const std::string& name,
                                         const std::function<NativeFn>& fn,
                                         MaObject* me) {
    const auto diiaObject = MaObject::Instance(M, M->diia_structure_object);
    diiaObject->diiaSetName(name);
    diiaObject->diiaSetNativeFn(fn);
    return diiaObject;
  }

  std::string MaObject::diiaGetName() const {
    return this->diiaName;
  }

  void MaObject::diiaSetName(const std::string& name) {
    this->diiaName = name;
  }

  MaCode* MaObject::diiaGetCode() const {
    return this->diiaCode;
  }

  void MaObject::diiaSetCode(MaCode* code) {
    this->diiaCode = code;
  }

  bool MaObject::diiaHasNativeFn() const {
    return this->diiaNativeFn != nullptr;
  }

  std::function<NativeFn> MaObject::diiaGetNativeFn() const {
    return this->diiaNativeFn;
  }

  void MaObject::diiaSetNativeFn(const std::function<NativeFn>& fn) {
    this->diiaNativeFn = fn;
  }

  bool MaObject::diiaHasBoundObject() const {
    return this->diiaBoundObject != nullptr;
  }

  MaObject* MaObject::diiaGetBoundObject() const {
    return this->diiaBoundObject;
  }

  void MaObject::diiaSetBoundObject(MaObject* diiaObject) {
    this->diiaBoundObject = diiaObject;
  }

  bool MaObject::diiaHasOuterScope() const {
    return this->diiaOuterScope != nullptr;
  }

  MaObject* MaObject::diiaGetOuterScope() const {
    return this->diiaOuterScope;
  }

  void MaObject::diiaSetOuterScope(MaObject* outerScopeObject) {
    this->diiaOuterScope = outerScopeObject;
  }

  std::vector<MaDiiaParam> MaObject::diiaGetParams() const {
    return this->diiaParams;
  }

  void MaObject::diiaSetParams(const std::vector<MaDiiaParam>& diiaParams) {
    this->diiaParams = diiaParams;
  }

  void MaObject::diiaPushParam(const MaDiiaParam& diiaParam) {
    this->diiaParams.push_back(diiaParam);
  }

  std::unordered_map<std::string, std::string>
  MaObject::diiaGetParamIndicesMap() const {
    return this->diiaParamIndicesMap;
  }

  void MaObject::diiaSetParamIndicesMap(
      const std::unordered_map<std::string, std::string>& paramIndicesMap) {
    this->diiaParamIndicesMap = paramIndicesMap;
  }

  bool MaObject::diiaGetIsModuleBuilder() const {
    return this->diiaIsModuleBuilder;
  }

  void MaObject::diiaSetIsModuleBuilder(bool isModuleBuilder) {
    this->diiaIsModuleBuilder = isModuleBuilder;
  }

  MaObject* MaObject::diiaBind(MaMa* M, MaObject* diiaObject) {
    const auto boundDiiaObject =
        MaObject::Instance(M, M->diia_structure_object);
    boundDiiaObject->diiaSetName(diiaObject->diiaGetName());
    boundDiiaObject->diiaSetCode(diiaObject->diiaGetCode());
    boundDiiaObject->diiaSetNativeFn(diiaObject->diiaGetNativeFn());
    boundDiiaObject->diiaSetBoundObject(diiaObject);
    boundDiiaObject->diiaSetOuterScope(diiaObject->diiaGetOuterScope());
    boundDiiaObject->diiaSetParams(diiaObject->diiaGetParams());
    boundDiiaObject->diiaSetParamIndicesMap(
        diiaObject->diiaGetParamIndicesMap());
    boundDiiaObject->diiaSetIsModuleBuilder(
        diiaObject->diiaGetIsModuleBuilder());
    return boundDiiaObject;
  }

  void InitDiia(MaMa* M) {
    const auto diia_structure_object = MaObject::CreateStructure(M, "Дія");
    M->global_scope->setProperty(M, "Дія", diia_structure_object);
    M->diia_structure_object = diia_structure_object;
  }
} // namespace mavka::mama