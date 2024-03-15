#include "../mama.h"

namespace mavka::mama {
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

  MaValue MaObject::getProperty(MaMa* M, const std::string& name) {
    if (this->isStructure(M)) {
      if (name == "назва") {
        return MaValue::Object(
            MaText::Create(M, this->asStructure()->getName()));
      }
    }
    if (this->isText(M)) {
      if (name == "довжина") {
        return MaValue::Integer(this->asText()->getLength());
      }
    }
    if (this->isList(M)) {
      if (name == "довжина") {
        return MaValue::Integer(this->asList()->getLength());
      }
    }
    if (this->isDict(M)) {
      if (name == "розмір") {
        return MaValue::Integer(this->asDict()->getSize());
      }
    }
    if (this->properties.contains(name)) {
      return this->properties[name];
    }
    return MaValue::Empty();
  }

  MaValue MaObject::call(mavka::mama::MaMa* M,
                         mavka::mama::MaArgs* args,
                         const mavka::mama::MaLocation& location) {
    const auto magicDiia = this->getProperty(M, MAG_CALL);
    if (!magicDiia.isEmpty()) {
      return magicDiia.call(M, args, location);
    }
    const auto frame = new MaFrame(M->call_stack.top()->scope, this,
                                   M->call_stack.top()->module, location);
    FRAME_PUSH(frame);
    if (this->isDiia(M)) {
      const auto diia = this->asDiia();
      if (diia->fn) {
        const auto result = diia->fn(M, this, args, location);
        if (!result.isError()) {
          FRAME_POP();
        }
        return result;
      } else {
        const auto diia_scope = new MaScope(diia->scope);
        frame->scope = diia_scope;
        if (diia->getMe()) {
          frame->scope->setSubject("я", diia->getMe());
        }
        for (int i = 0; i < diia->getParams().size(); ++i) {
          const auto& param = this->d.diia->params[i];
          const auto arg_value = args->Get(i, param.name, param.default_value);
          frame->scope->setSubject(param.name, arg_value);
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
        const auto arg_value = args->Get(i, param.name, param.default_value);
        object->setProperty(M, param.name, arg_value);
      }
      FRAME_POP();
      return MaValue::Object(object);
    }
    FRAME_POP();
    return MaValue::Error(MaError::Create(
        MaValue::Object(MaText::Create(M, "Неможливо викликати.")),
        M->call_stack.top()->module, location));
  }

  MaValue MaObject::callMagWithValue(MaMa* M,
                                     const MaValue& value,
                                     const MaLocation& location,
                                     const std::string& name) {
    this->retain();
    const auto magicDiia = this->getProperty(M, name);
    if (magicDiia.isObject()) {
      magicDiia.asObject()->retain();
    }
    if (value.isObject()) {
      value.asObject()->retain();
    }
    const auto result = magicDiia.call(M, {value}, location);
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
                                        const MaLocation& location,
                                        const std::string& name) {
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

  MaValue MaObject::isGreater(MaMa* M,
                              const MaValue& value,
                              const MaLocation& location) {
    return this->callMagWithValue(M, value, location, MAG_GREATER);
  }

  MaValue MaObject::isGreaterOrEqual(MaMa* M,
                                     const MaValue& value,
                                     const MaLocation& location) {
    return this->callMagWithValue(M, value, location, MAG_GREATER_EQUAL);
  }

  MaValue MaObject::isLesser(MaMa* M,
                             const MaValue& value,
                             const MaLocation& location) {
    return this->callMagWithValue(M, value, location, MAG_LESSER);
  }

  MaValue MaObject::isLesserOrEqual(MaMa* M,
                                    const MaValue& value,
                                    const MaLocation& location) {
    return this->callMagWithValue(M, value, location, MAG_LESSER_EQUAL);
  }

  MaValue MaObject::contains(MaMa* M,
                             const MaValue& value,
                             const MaLocation& location) {
    return this->callMagWithValue(M, value, location, MAG_CONTAINS);
  }

  MaValue MaObject::doNegative(MaMa* M, const MaLocation& location) {
    return this->callMagWithoutValue(M, location, MAG_NEGATIVE);
  }

  MaValue MaObject::doPositive(MaMa* M, const MaLocation& location) {
    return this->callMagWithoutValue(M, location, MAG_POSITIVE);
  }

  MaValue MaObject::doBNot(MaMa* M, const MaLocation& location) {
    return this->callMagWithoutValue(M, location, MAG_BW_NOT);
  }

  MaValue MaObject::doAdd(MaMa* M,
                          const MaValue& value,
                          const MaLocation& location) {
    return this->callMagWithValue(M, value, location, MAG_ADD);
  }

  MaValue MaObject::doSub(MaMa* M,
                          const MaValue& value,
                          const MaLocation& location) {
    return this->callMagWithValue(M, value, location, MAG_SUB);
  }

  MaValue MaObject::doMul(MaMa* M,
                          const MaValue& value,
                          const MaLocation& location) {
    return this->callMagWithValue(M, value, location, MAG_MUL);
  }

  MaValue MaObject::doDiv(MaMa* M,
                          const MaValue& value,
                          const MaLocation& location) {
    return this->callMagWithValue(M, value, location, MAG_DIV);
  }

  MaValue MaObject::doMod(MaMa* M,
                          const MaValue& value,
                          const MaLocation& location) {
    return this->callMagWithValue(M, value, location, MAG_MOD);
  }

  MaValue MaObject::doDivDiv(MaMa* M,
                             const MaValue& value,
                             const MaLocation& location) {
    return this->callMagWithValue(M, value, location, MAG_DIVDIV);
  }

  MaValue MaObject::doPow(MaMa* M,
                          const MaValue& value,
                          const MaLocation& location) {
    return this->callMagWithValue(M, value, location, MAG_POW);
  }

  MaValue MaObject::doXor(MaMa* M,
                          const MaValue& value,
                          const MaLocation& location) {
    return this->callMagWithValue(M, value, location, MAG_BW_XOR);
  }

  MaValue MaObject::doBor(MaMa* M,
                          const MaValue& value,
                          const MaLocation& location) {
    return this->callMagWithValue(M, value, location, MAG_BW_OR);
  }

  MaValue MaObject::doBand(MaMa* M,
                           const MaValue& value,
                           const MaLocation& location) {
    return this->callMagWithValue(M, value, location, MAG_BW_AND);
  }

  MaValue MaObject::doShl(MaMa* M,
                          const MaValue& value,
                          const MaLocation& location) {
    return this->callMagWithValue(M, value, location, MAG_BW_SHIFT_LEFT);
  }

  MaValue MaObject::doShr(MaMa* M,
                          const MaValue& value,
                          const MaLocation& location) {
    return this->callMagWithValue(M, value, location, MAG_BW_SHIFT_RIGHT);
  }

  void MaObject::Init(MaMa* M) {
    const auto object_structure_object = MaStructure::Create(M, "обʼєкт");
    M->global_scope->setSubject("обʼєкт", object_structure_object);
    M->object_structure_object = object_structure_object;
  }

  MaObject* MaObject::Instance(MaMa* M, MaObject* structure_object, void* d) {
#if MAMA_GC_DEBUG
    std::cout << "MaObject::Instance " << (void*)d << std::endl;
#endif
    const auto object = new MaObject();
    object->type = structure_object;
    object->d.ptr = d;
    for (const auto& method : structure_object->d.structure->methods) {
      const auto bound_diia_object = method->d.diia->Bind(M, object);
      object->setProperty(M, method->d.diia->name,
                          MaValue::Object(bound_diia_object));
    }
    return object;
  }

  MaObject* MaObject::Empty(MaMa* M) {
    return MaObject::Instance(M, M->object_structure_object, nullptr);
  }
} // namespace mavka::mama