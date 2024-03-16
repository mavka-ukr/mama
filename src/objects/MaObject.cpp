#include "../mama.h"

namespace mavka::mama {
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

  MaStructure* MaObject::asStructure() const {
    return this->d.structure;
  }

  MaDiia* MaObject::asDiia() const {
    return this->d.diia;
  }

  MaModule* MaObject::asModule() const {
    return this->d.module;
  }

  MaBytes* MaObject::asBytes() const {
    return this->d.bytes;
  }

  MaText* MaObject::asText() const {
    return this->d.text;
  }

  MaList* MaObject::asList() const {
    return this->d.list;
  }

  MaDict* MaObject::asDict() const {
    return this->d.dict;
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
    this->properties.insert_or_assign(name, value);
  }

  void MaObject::setProperty(MaMa* M,
                             const std::string& name,
                             MaObject* value) {
    this->properties.insert_or_assign(name, MaValue::Object(value));
  }

  bool MaObject::hasProperty(MaMa* M, const std::string& name) {
    if (this->properties.contains(name)) {
      return true;
    }
    if (this->isScope(M)) {
      if (this->d.parent) {
        return this->d.parent->hasProperty(M, name);
      }
    }
    return false;
  }

  MaValue MaObject::getProperty(MaMa* M, const std::string& name) {
    if (this->isScope(M)) {
      if (this->properties.contains(name)) {
        return this->properties[name];
      }
      auto parent_tmp = this->d.parent;
      while (parent_tmp) {
        if (parent_tmp->properties.contains(name)) {
          return parent_tmp->properties[name];
        }
        parent_tmp = parent_tmp->d.parent;
      }
      return MaValue::Empty();
    }
    if (this->isStructure(M)) {
      if (name == "назва") {
        return MaValue::Object(
            MaText::Create(M, this->asStructure()->getName()));
      }
    } else if (this->isText(M)) {
      if (name == "довжина") {
        return MaValue::Integer(this->asText()->getLength());
      }
    } else if (this->isList(M)) {
      if (name == "довжина") {
        return MaValue::Integer(this->asList()->getLength());
      }
    } else if (this->isDict(M)) {
      if (name == "розмір") {
        return MaValue::Integer(this->asDict()->getSize());
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
    const auto frame = new MaFrame(currentScope, this, li);
    FRAME_PUSH(frame);
    if (this->isDiia(M)) {
      const auto diia = this->asDiia();
      const auto diia_scope =
          MaObject::Instance(M, M->scope_structure_object,
                             diia->scope ? diia->scope : currentScope);
      frame->scope = diia_scope;
      if (diia->getMe()) {
        frame->scope->setProperty(M, "я", diia->getMe());
      }
      if (diia->fn) {
        const auto result = diia->fn(M, this, args, li);
        if (!result.isError()) {
          FRAME_POP();
        }
        return result;
      } else {
        for (int i = 0; i < diia->getParams().size(); ++i) {
          const auto& param = this->d.diia->params[i];
          const auto arg_value = args->getArg(M, std::to_string(i), param.name,
                                              param.default_value);
          frame->scope->setProperty(M, param.name, arg_value);
        }
        std::stack<MaValue> stack;
        const auto result = M->run(this->d.diia->code, stack);
        // todo: gc
        if (!result.isError()) {
          FRAME_POP();
        }
        return result;
      }
    }
    if (this->isStructure(M)) {
      // todo: handle spec structures
      const auto object = MaObject::Instance(M, this, nullptr);
      for (int i = 0; i < this->d.structure->params.size(); ++i) {
        const auto& param = this->d.structure->params[i];
        const auto arg_value =
            args->getArg(M, std::to_string(i), param.name, param.default_value);
        object->setProperty(M, param.name, arg_value);
      }
      FRAME_POP();
      return MaValue::Object(object);
    }
    FRAME_POP();
    return MaValue::Error(MaError::Create(
        MaValue::Object(MaText::Create(M, "Неможливо викликати.")), li));
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
    //    args->release();
    if (value.isObject()) {
      //      value.asObject()->release();
    }
    if (magicDiia.isObject()) {
      //      magicDiia.asObject()->release();
    }
    //    this->release();
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
      //      magicDiia.asObject()->release();
    }
    //    this->release();
    return result;
  }

  std::string MaObject::getPrettyString(MaMa* M) {
    if (this->isStructure(M)) {
      const auto structureName = this->asStructure()->getName();
      return "<структура " + structureName + ">";
    }
    if (this->isDiia(M)) {
      const auto diiaName = this->asDiia()->name;
      const auto boundName = this->asDiia()->me
                                 ? this->asDiia()->me->getStructure()
                                       ? this->asDiia()
                                             ->me->getStructure()
                                             ->asStructure()
                                             ->getName()
                                       : ""
                                 : "";
      if (boundName.empty()) {
        return "<дія " + diiaName + ">";
      }
      return "<дія " + boundName + "." + diiaName + ">";
    }
    if (this->isModule(M)) {
      const auto moduleName = this->asModule()->getName();
      return "<модуль " + moduleName + ">";
    }
    if (this->getStructure()) {
      const auto structureName = this->getStructure()->asStructure()->getName();
      return "<обʼєкт " + structureName + ">";
    }
    return "<обʼєкт>";
  }

  void MaObject::Init(MaMa* M) {
    const auto object_structure_object = MaStructure::Create(M, "обʼєкт");
    M->global_scope->setProperty(M, "обʼєкт", object_structure_object);
    M->object_structure_object = object_structure_object;
  }

  MaObject* MaObject::Instance(MaMa* M, MaObject* structure_object, void* d) {
    const auto object = new MaObject();
    object->type = structure_object;
    object->d.ptr = d;
    for (const auto& method : structure_object->d.structure->methods) {
      const auto bound_diia_object = method->d.diia->Bind(M, object);
      object->setProperty(M, method->d.diia->name,
                          MaValue::Object(bound_diia_object));
    }
#if MAMA_GC_DEBUG
    std::cout << "[GC] created " << object->getPrettyString(M) << " "
              << (void*)object << std::endl;
#endif
    return object;
  }

  MaObject* MaObject::Empty(MaMa* M) {
    return MaObject::Instance(M, M->object_structure_object, nullptr);
  }
} // namespace mavka::mama