#include "../mama.h"

namespace mavka::mama {
  MaObject::~MaObject() {
    for (const auto& [_, value] : this->properties) {
      if (value.isObject()) {
        value.asObject()->release();
      }
    }
    if (this->scopeOuter) {
      this->scopeOuter->release();
    }
    if (this->scopeModule) {
      this->scopeModule->release();
    }
    if (this->diiaBoundObject) {
      this->diiaBoundObject->release();
    }
    if (this->diiaOuterScope) {
      this->diiaOuterScope->release();
    }
    if (this->moduleRoot) {
      this->moduleRoot->release();
    }
    for (const auto& item : this->listData) {
      if (item.isObject()) {
        item.asObject()->release();
      }
    }
    for (const auto& [key, value] : this->dictData) {
      if (key.isObject()) {
        key.asObject()->release();
      }
      if (value.isObject()) {
        value.asObject()->release();
      }
    }
    this->properties.clear();
    this->bytesData.clear();
    this->listData.clear();
    this->dictData.clear();
    this->textData.clear();
    this->diiaName.clear();
    this->structureName.clear();
    this->moduleName.clear();
#if MAMA_GC_DEBUG == 1
    std::cout << "[GC] deleted " << (void*)this << std::endl;
#endif
  }

  bool MaObject::isScope(MaMa* M) const {
    return this->structure == M->scope_structure_object;
  }

  bool MaObject::isStructure(MaMa* M) const {
    return this->structure == M->structure_structure_object;
  }

  bool MaObject::isDiia(MaMa* M) const {
    return this->structure == M->diia_structure_object;
  }

  bool MaObject::isModule(MaMa* M) const {
    return this->structure == M->module_structure_object;
  }

  bool MaObject::isBytes(MaMa* M) const {
    return this->structure == M->bytes_structure_object;
  }

  bool MaObject::isText(MaMa* M) const {
    return this->structure == M->text_structure_object;
  }

  bool MaObject::isList(MaMa* M) const {
    return this->structure == M->list_structure_object;
  }

  bool MaObject::isDict(MaMa* M) const {
    return this->structure == M->dict_structure_object;
  }

  void MaObject::retain() {
    ++this->ref_count;
  }

  void MaObject::release() {
    if (this->indestructible) {
      return;
    }
    if (this->ref_count == 0) {
      return;
    }
    --this->ref_count;
    if (this->ref_count == 0) {
      delete this;
    }
  }

  MaObject* MaObject::getStructure() const {
    return this->structure;
  }

  bool MaObject::is(MaMa* M, MaObject* object) const {
    if (object == M->object_structure_object) {
      return true;
    }
    return this->getStructure() == object;
  }

  void MaObject::setProperty(MaMa* M,
                             const std::string& name,
                             const MaValue& value) {
    if (value.isObject()) {
      value.asObject()->retain();
    }
    if (this->properties.contains(name)) {
      if (this->properties[name].isObject()) {
        this->properties[name].asObject()->release();
      }
    }
    this->properties.insert_or_assign(name, value);
  }

  void MaObject::setProperty(MaMa* M,
                             const std::string& name,
                             MaObject* value) {
    value->retain();
    if (this->properties.contains(name)) {
      if (this->properties[name].isObject()) {
        this->properties[name].asObject()->release();
      }
    }
    this->properties.insert_or_assign(name, MaValue::Object(value));
  }

  bool MaObject::hasProperty(MaMa* M, const std::string& name) {
    if (this->properties.contains(name)) {
      return true;
    }
    if (this->isScope(M)) {
      if (this->scopeHasOuter()) {
        return this->scopeGetOuter()->hasProperty(M, name);
      }
    }
    if (this->structure) {
      for (const auto& method : this->structure->structureMethods) {
        if (method->diiaName == name) {
          return true;
        }
      }
    }
    return false;
  }

  MaValue MaObject::getProperty(MaMa* M, const std::string& name) {
    if (this->isScope(M)) {
      if (this->properties.contains(name)) {
        return this->properties[name];
      }
      auto parent_tmp = this->scopeGetOuter();
      while (parent_tmp) {
        if (parent_tmp->properties.contains(name)) {
          return parent_tmp->properties[name];
        }
        parent_tmp = parent_tmp->scopeGetOuter();
      }
      return MaValue::Empty();
    }
    if (this->isStructure(M)) {
      if (name == "назва") {
        return MaValue::Object(M->createText(this->structureGetName()));
      }
    } else if (this->isText(M)) {
      if (name == "довжина") {
        return MaValue::Integer(this->textGetLength());
      }
    } else if (this->isList(M)) {
      if (name == "довжина") {
        return MaValue::Integer(this->listGetLength());
      }
    } else if (this->isDict(M)) {
      if (name == "розмір") {
        return MaValue::Integer(this->dictGetSize());
      }
    }
    if (this->properties.contains(name)) {
      return this->properties[name];
    }
    if (this->structure) {
      MaObject* foundMethod = nullptr;
      for (const auto& method : this->structure->structureMethods) {
        if (method->diiaName == name) {
          foundMethod = method;
          break;
        }
      }
      if (foundMethod) {
        const auto boundDiia = foundMethod->diiaBind(M, this);
        this->properties.insert_or_assign(name, MaValue::Object(boundDiia));
        return MaValue::Object(boundDiia);
      }
    }
    return MaValue::Empty();
  }

  MaValue MaObject::getArg(mavka::mama::MaMa* M,
                           const std::string& index,
                           const std::string& name) {
    if (this->properties.contains(index)) {
      return this->properties[index];
    }
    if (this->properties.contains(name)) {
      return this->properties[name];
    }
    return MaValue::Empty();
  }

  MaValue MaObject::getArg(mavka::mama::MaMa* M,
                           const std::string& index,
                           const std::string& name,
                           const mavka::mama::MaValue& defaultValue) {
    if (this->properties.contains(index)) {
      return this->properties[index];
    }
    if (this->properties.contains(name)) {
      return this->properties[name];
    }
    return defaultValue;
  }

  MaValue MaObject::call(mavka::mama::MaMa* M,
                         mavka::mama::MaObject* scope,
                         mavka::mama::MaObject* args,
                         size_t li) {
    const auto magicDiia = this->getProperty(M, MAG_CALL);
    if (!magicDiia.isEmpty()) {
      return magicDiia.call(M, scope, args, li);
    }
    scope->retain();
    this->retain();
    M->call_stack.push(MaFrame(this, li));
    if (this->isDiia(M)) {
      if (this->diiaHasNativeFn()) {
        const auto result = this->diiaGetNativeFn()(M, scope, this, args, li);
        if (!result.isError()) {
          M->call_stack.pop();
          this->release();
          scope->release();
        }
        return result;
      } else {
        const auto diiaScope =
            M->createScope(this->diiaGetOuterScope(), scope->scopeGetModule());
        diiaScope->retain();
        if (this->diiaHasBoundObject()) {
          diiaScope->setProperty(M, "я", this->diiaGetBoundObject());
        }
        const auto diiaParams = this->diiaGetParams();
        for (int i = 0; i < diiaParams.size(); ++i) {
          const auto& param = diiaParams[i];
          const auto arg_value = args->getArg(M, std::to_string(i), param.name,
                                              param.default_value);
          diiaScope->setProperty(M, param.name, arg_value);
        }
        const auto result = M->run(diiaScope, this->diiaGetCode());
        this->release();
        diiaScope->release();
        scope->release();
        if (!result.isError()) {
          M->call_stack.pop();
        }
        return result;
      }
    }
    if (this->isStructure(M)) {
      // todo: handle spec structures
      const auto instanceObject = M->createObject(this);
      const auto structureParams = this->structureGetParams();
      for (int i = 0; i < structureParams.size(); ++i) {
        const auto& param = structureParams[i];
        const auto arg_value =
            args->getArg(M, std::to_string(i), param.name, param.default_value);
        instanceObject->setProperty(M, param.name, arg_value);
      }
      M->call_stack.pop();
      this->release();
      scope->release();
      return MaValue::Object(instanceObject);
    }
    M->call_stack.pop();
    this->release();
    scope->release();
    return MaValue::Error(MaError::Create(
        MaValue::Object(M->createText("Неможливо викликати.")), li));
  }

  MaValue MaObject::callMagWithValue(MaMa* M,
                                     MaObject* scope,
                                     const MaValue& value,
                                     size_t li,
                                     const std::string& name) {
    if (!this->hasProperty(M, name)) {
      return MaValue::ErrorDiiaNotDefinedFor(M, name, MaValue::Object(this),
                                             li);
    }
    this->retain();
    const auto magicDiia = this->getProperty(M, name);
    if (magicDiia.isObject()) {
      magicDiia.asObject()->retain();
    }
    if (value.isObject()) {
      value.asObject()->retain();
    }
    const auto args = M->createObject(M->object_structure_object);
    args->retain();
    args->setProperty(M, "0", value);
    const auto result = magicDiia.call(M, scope, args, li);
    args->release();
    if (value.isObject()) {
      value.asObject()->release();
    }
    if (magicDiia.isObject()) {
      magicDiia.asObject()->release();
    }
    this->release();
    return result;
  }

  MaValue MaObject::callMagWithoutValue(MaMa* M,
                                        MaObject* scope,
                                        size_t li,
                                        const std::string& name) {
    if (!this->hasProperty(M, name)) {
      return MaValue::ErrorDiiaNotDefinedFor(M, name, MaValue::Object(this),
                                             li);
    }
    this->retain();
    const auto magicDiia = this->getProperty(M, name);
    if (magicDiia.isObject()) {
      magicDiia.asObject()->retain();
    }
    const auto result = magicDiia.call(M, scope, {}, {});
    if (magicDiia.isObject()) {
      magicDiia.asObject()->release();
    }
    this->release();
    return result;
  }

  std::string MaObject::getPrettyString(MaMa* M) {
    if (this->isStructure(M)) {
      const auto structureName = this->structureGetName();
      return "<структура " + structureName + ">";
    }
    if (this->isDiia(M)) {
      const auto diiaName = this->diiaGetName();
      const auto boundName = this->diiaHasBoundObject()
                                 ? this->diiaGetBoundObject()->getStructure()
                                       ? this->diiaGetBoundObject()
                                             ->getStructure()
                                             ->structureGetName()
                                       : ""
                                 : "";
      if (boundName.empty()) {
        return "<дія " + diiaName + ">";
      }
      return "<дія " + boundName + "." + diiaName + ">";
    }
    if (this->isModule(M)) {
      const auto moduleName = this->moduleGetName();
      return "<модуль " + moduleName + ">";
    }
    if (this->getStructure()) {
      const auto structureName = this->getStructure()->structureGetName();
      return "<обʼєкт " + structureName + ">";
    }
    return "<обʼєкт>";
  }

  void MaObject::Init(MaMa* M) {
    const auto objectStructureObject = M->createStructure("обʼєкт");
    objectStructureObject->indestructible = true;
    M->global_scope->setProperty(M, "обʼєкт", objectStructureObject);
    M->object_structure_object = objectStructureObject;
  }
} // namespace mavka::mama