#include "../mama.h"

namespace mavka::mama {
  void MaList::append(MaMa* M, const MaValue& cell) {
    if (cell.isObject()) {
      cell.asObject()->retain();
    }
    this->data.push_back(cell);
  }

  void MaList::setAt(MaMa* M, size_t index, const MaValue& value) {
    if (value.isObject()) {
      value.asObject()->retain();
    }
    if (index >= 0) {
      if (index >= this->getLength()) {
        // todo: looks bad
        this->data.resize(index + 1);
      }
      this->data[index] = value;
    }
  }

  MaValue MaList::getAt(MaMa* M, size_t index) const {
    if (index >= 0 && index < this->getLength()) {
      return this->data[index];
    }
    return MaValue::Empty();
  }

  bool MaList::contains(MaMa* M, const MaValue& cell) {
    for (const auto& item : this->data) {
      if (cell.isEqual(M, item)) {
        return true;
      }
    }
    return false;
  }

  // чародія_перебір
  MaValue MaList_MagIteratorNativeFn(MaMa* M,
                                     MaObject* native_o,
                                     MaArgs* args,
                                     const MaLocation& location) {
    const auto list_o = native_o->asDiia()->me;

    const auto iterator_o = MaObject::Empty(M);
    iterator_o->setProperty(M, "_список", list_o);

    if (list_o->asList()->getLength() == 0) {
      iterator_o->setProperty(M, "завершено", MaValue::Yes());
    } else if (list_o->asList()->getLength() == 1) {
      iterator_o->setProperty(M, "завершено", MaValue::No());
      iterator_o->setProperty(M, "значення", list_o->asList()->getAt(M, 0));
      const auto next_native_o = MaDiia::Create(
          M, "далі",
          [](MaMa* M, MaObject* native_o, MaArgs* args,
             const MaLocation& location) {
            const auto iterator_object = native_o->asDiia()->getMe();
            iterator_object->setProperty(M, "завершено", MaValue::Yes());
            return MaValue::Empty();
          },
          iterator_o);
      iterator_o->setProperty(M, "далі", next_native_o);
    } else {
      iterator_o->setProperty(M, "завершено", MaValue::No());
      iterator_o->setProperty(M, "значення", list_o->asList()->getAt(M, 0));
      iterator_o->setProperty(M, "_індекс", MaValue::Number(1));
      const auto next_native_o = MaDiia::Create(
          M, "далі",
          [](MaMa* M, MaObject* native_o, MaArgs* args,
             const MaLocation& location) {
            const auto iterator_o = native_o->asDiia()->getMe();
            const auto i = iterator_o->getProperty(M, "_індекс").asInteger();
            const auto list =
                iterator_o->getProperty(M, "_список").asObject()->asList();
            if (i < list->getLength()) {
              iterator_o->setProperty(M, "завершено", MaValue::No());
              iterator_o->setProperty(M, "значення", list->getAt(M, i));
              iterator_o->setProperty(M, "_індекс", MaValue::Integer(i + 1));
            } else {
              iterator_o->setProperty(M, "завершено", MaValue::Yes());
            }
            return MaValue::Empty();
          },
          iterator_o);
      iterator_o->setProperty(M, "далі", next_native_o);
    }
    return MaValue::Object(iterator_o);
  }

  // чародія_отримати
  MaValue MaList_MagGetElementNativeDiiaFn(MaMa* M,
                                           MaObject* native_o,
                                           MaArgs* args,
                                           const MaLocation& location) {
    const auto key = args->Get(0, "ключ");
    if (key.isNumber()) {
      return native_o->asDiia()->getMe()->asList()->getAt(M, key.asInteger());
    }
    return MaValue::Empty();
  }

  // чародія_покласти
  MaValue MaList_MagSetElementNativeDiiaFn(MaMa* M,
                                           MaObject* native_o,
                                           MaArgs* args,
                                           const MaLocation& location) {
    const auto key = args->Get(0, "ключ");
    if (!key.isNumber()) {
      // maybe return error
      return MaValue::Empty();
    }
    const auto value = args->Get(1, "значення");
    native_o->asDiia()->getMe()->asList()->setAt(M, key.asInteger(), value);
    return MaValue::Empty();
  }

  // додати
  MaValue MaList_AppendNativeDiiaFn(MaMa* M,
                                    MaObject* native_o,
                                    MaArgs* args,
                                    const MaLocation& location) {
    const auto cell = args->Get(0, "значення");
    native_o->asDiia()->getMe()->asList()->append(M, cell);
    return MaValue::Integer(native_o->asDiia()->getMe()->asList()->getLength());
  }

  // чародія_містить
  MaValue MaList_MagContainsNativeDiiaFn(MaMa* M,
                                         MaObject* native_o,
                                         MaArgs* args,
                                         const MaLocation& location) {
    const auto cell = args->Get(0, "значення");
    if (native_o->asDiia()->getMe()->asList()->contains(M, cell)) {
      return MaValue::Yes();
    } else {
      return MaValue::No();
    }
  }

  MaObject* MaList::Create(MaMa* M) {
    const auto list = new MaList();
    const auto list_o = MaObject::Instance(M, M->list_structure_object, list);
    list_o->setProperty(
        M, MAG_ITERATOR,
        MaDiia::Create(M, MAG_ITERATOR, MaList_MagIteratorNativeFn, list_o));
    list_o->setProperty(
        M, MAG_GET_ELEMENT,
        MaDiia::Create(M, MAG_GET_ELEMENT, MaList_MagGetElementNativeDiiaFn,
                       list_o));
    list_o->setProperty(
        M, MAG_SET_ELEMENT,
        MaDiia::Create(M, MAG_SET_ELEMENT, MaList_MagSetElementNativeDiiaFn,
                       list_o));
    list_o->setProperty(
        M, "додати",
        MaDiia::Create(M, "додати", MaList_AppendNativeDiiaFn, list_o));
    list_o->setProperty(M, MAG_CONTAINS,
                        MaDiia::Create(M, MAG_CONTAINS,
                                       MaList_MagContainsNativeDiiaFn, list_o));
    return list_o;
  }

  MaValue MaList_Structure_MagCallNativeDiiaFn(MaMa* M,
                                               MaObject* native_o,
                                               MaArgs* args,
                                               const MaLocation& location) {
    const auto cell = args->Get(0, "значення");
    if (cell.isObject()) {
      if (cell.asObject()->isList(M)) {
        return cell;
      }
      return cell.asObject()->getProperty(M, MAG_LIST).call(M, {}, {});
    }
    return MaValue::Error(MaError::Create(
        MaValue::Object(MaText::Create(M, "Неможливо перетворити на список.")),
        M->call_stack.top()->module, location));
  }

  size_t MaList::getLength() const {
    return this->data.size();
  }

  void MaList::Init(MaMa* M) {
    const auto list_structure_object = MaStructure::Create(M, "список");
    M->global_scope->setSubject("список", list_structure_object);
    M->list_structure_object = list_structure_object;
    list_structure_object->setProperty(
        M, MAG_CALL,
        MaDiia::Create(M, MAG_CALL, MaList_Structure_MagCallNativeDiiaFn,
                       list_structure_object));
  }
} // namespace mavka::mama