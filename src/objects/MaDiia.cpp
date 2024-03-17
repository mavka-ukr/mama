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
    diiaObject->diiaSetBoundObject(me);
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

  void MaObject::diiaSetBoundObject(MaObject* object) {
    if (object) {
      object->retain();
    }
    if (this->diiaBoundObject != nullptr) {
      this->diiaBoundObject->release();
    }
    this->diiaBoundObject = object;
  }

  bool MaObject::diiaHasOuterScope() const {
    return this->diiaOuterScope != nullptr;
  }

  MaObject* MaObject::diiaGetOuterScope() const {
    return this->diiaOuterScope;
  }

  void MaObject::diiaSetOuterScope(MaObject* outerScopeObject) {
    if (outerScopeObject) {
      outerScopeObject->retain();
    }
    if (this->diiaOuterScope != nullptr) {
      this->diiaOuterScope->release();
    }
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

  MaObject* MaObject::diiaBind(MaMa* M, MaObject* diiaObject) {
    const auto boundDiiaObject =
        MaObject::Instance(M, M->diia_structure_object);
    boundDiiaObject->diiaSetName(this->diiaGetName());
    boundDiiaObject->diiaSetCode(this->diiaGetCode());
    boundDiiaObject->diiaSetNativeFn(this->diiaGetNativeFn());
    boundDiiaObject->diiaSetBoundObject(diiaObject);
    boundDiiaObject->diiaSetOuterScope(this->diiaGetOuterScope());
    boundDiiaObject->diiaSetParams(this->diiaGetParams());
    boundDiiaObject->diiaSetParamIndicesMap(this->diiaGetParamIndicesMap());
    return boundDiiaObject;
  }

  void InitDiia(MaMa* M) {
    const auto diiaStructureObject = MaObject::CreateStructure(M, "Дія");
    M->global_scope->setProperty(M, "Дія", diiaStructureObject);
    M->diia_structure_object = diiaStructureObject;
  }
} // namespace mavka::mama