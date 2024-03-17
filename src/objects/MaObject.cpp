#include "../mama.h"

namespace mavka::mama {
  MaObject::~MaObject() {
    for (const auto& [_, value] : this->properties) {
      if (value.isObject()) {
        value.asObject()->release();
      }
    }
    this->properties.clear();
#if MAMA_GC_DEBUG == 1
    std::cout << "[GC] deleted " << (void*)this << std::endl;
#endif
  }

  bool MaObject::isScope(MaMa* M) const {
    return this->type == M->scope_structure_object;
  }

  bool MaObject::isStructure(MaMa* M) const {
    return this->type == M->structure_structure_object;
  }

  bool MaObject::isDiia(MaMa* M) const {
    return this->type == M->diia_structure_object;
  }

  bool MaObject::isModule(MaMa* M) const {
    return this->type == M->module_structure_object;
  }

  bool MaObject::isBytes(MaMa* M) const {
    return this->type == M->bytes_structure_object;
  }

  bool MaObject::isText(MaMa* M) const {
    return this->type == M->text_structure_object;
  }

  bool MaObject::isList(MaMa* M) const {
    return this->type == M->list_structure_object;
  }

  bool MaObject::isDict(MaMa* M) const {
    return this->type == M->dict_structure_object;
  }

  void MaObject::retain() {
    ++this->ref_count;
  }

  void MaObject::release() {
    if (this->ref_count == 0) {
      return;
    }
    --this->ref_count;
    if (this->ref_count == 0) {
      delete this;
    }
  }

  MaObject* MaObject::getStructure() const {
    return this->type;
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
        return MaValue::Object(
            MaObject::CreateText(M, this->structureGetName()));
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
                         mavka::mama::MaObject* args,
                         size_t li) {
    const auto magicDiia = this->getProperty(M, MAG_CALL);
    if (!magicDiia.isEmpty()) {
      return magicDiia.call(M, args, li);
    }
    const auto currentScope =
        M->call_stack.empty() ? M->global_scope : M->call_stack.top()->scope;
    const auto currentModule =
        M->call_stack.empty() ? M->main_module : M->call_stack.top()->module;
    currentScope->retain();
    this->retain();
    currentModule->retain();
    const auto frame = new MaFrame(currentScope, this, currentModule, li);
    FRAME_PUSH(frame);
    if (this->isDiia(M)) {
      const auto diiaScope = MaObject::Instance(M, M->scope_structure_object);
      diiaScope->scopeSetOuter(
          this->diiaHasOuterScope() ? this->diiaGetOuterScope() : currentScope);
      diiaScope->retain();
      frame->getScope()->release();
      frame->scope = diiaScope;
      if (this->diiaHasBoundObject()) {
        frame->scope->setProperty(M, "я", this->diiaGetBoundObject());
      }
      if (this->diiaHasNativeFn()) {
        const auto result = this->diiaGetNativeFn()(M, this, args, li);
        if (!result.isError()) {
          delete frame;
          FRAME_POP();
        }
        return result;
      } else {
        const auto diiaParams = this->diiaGetParams();
        for (int i = 0; i < diiaParams.size(); ++i) {
          const auto& param = diiaParams[i];
          const auto arg_value = args->getArg(M, std::to_string(i), param.name,
                                              param.default_value);
          frame->scope->setProperty(M, param.name, arg_value);
        }
        std::stack<MaValue> stack;
        const auto result = M->run(this->diiaGetCode(), stack);
        // todo: gc
        if (!result.isError()) {
          delete frame;
          FRAME_POP();
        }
        return result;
      }
    }
    if (this->isStructure(M)) {
      // todo: handle spec structures
      const auto instanceObject = MaObject::Instance(M, this);
      const auto structureParams = this->structureGetParams();
      for (int i = 0; i < structureParams.size(); ++i) {
        const auto& param = structureParams[i];
        const auto arg_value =
            args->getArg(M, std::to_string(i), param.name, param.default_value);
        instanceObject->setProperty(M, param.name, arg_value);
      }
      delete frame;
      FRAME_POP();
      return MaValue::Object(instanceObject);
    }
    delete frame;
    FRAME_POP();
    return MaValue::Error(MaError::Create(
        MaValue::Object(MaObject::CreateText(M, "Неможливо викликати.")), li));
  }

  MaValue MaObject::callMagWithValue(MaMa* M,
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
    const auto args = MaObject::Empty(M);
    args->retain();
    args->setProperty(M, "0", value);
    const auto result = magicDiia.call(M, args, li);
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
    const auto result = magicDiia.call(M, {}, {});
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
    const auto object_structure_object = MaObject::CreateStructure(M, "обʼєкт");
    M->global_scope->setProperty(M, "обʼєкт", object_structure_object);
    M->object_structure_object = object_structure_object;
  }

  MaObject* MaObject::Instance(MaMa* M, MaObject* structureObject) {
    const auto object = new MaObject();
    object->type = structureObject;
    const auto structureMethods = structureObject->structureGetMethods();
    for (const auto& method : structureMethods) {
      const auto bound_diia_object = method->diiaBind(M, object);
      object->setProperty(M, method->diiaGetName(),
                          MaValue::Object(bound_diia_object));
    }
#if MAMA_GC_DEBUG
    std::cout << "[GC] created " << object->getPrettyString(M) << " "
              << (void*)object << std::endl;
#endif
    return object;
  }

  MaObject* MaObject::Empty(MaMa* M) {
    return MaObject::Instance(M, M->object_structure_object);
  }
} // namespace mavka::mama