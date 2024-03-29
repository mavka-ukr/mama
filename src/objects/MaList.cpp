#include "../mama.h"

namespace mavka::mama {
  void MaObject::listAppend(MaMa* M, const MaValue& cell) {
    if (cell.isObject()) {
      cell.asObject()->retain();
    }
    this->listData.push_back(cell);
  }

  MaValue MaObject::listGetAt(MaMa* M, long index) const {
    if (index >= 0 && index < this->listGetLength()) {
      return this->listData[index];
    }
    return MaValue::Empty();
  }

  void MaObject::listSetAt(MaMa* M, long index, const MaValue& value) {
    if (value.isObject()) {
      value.asObject()->retain();
    }
    if (index >= 0) {
      if (index >= this->listGetLength()) {
        // todo: looks bad
        this->listData.resize(index + 1);
      } else {
        const auto old = this->listData[index];
        if (old.isObject()) {
          old.asObject()->release();
        }
      }
      this->listData[index] = value;
    }
  }

  bool MaObject::listContains(MaMa* M, const MaValue& cell) {
    for (const auto& item : this->listData) {
      if (cell.isEqual(M, item)) {
        return true;
      }
    }
    return false;
  }

  // чародія_перебір
  MaValue MaList_MagIteratorNativeFn(MaMa* M,
                                     MaObject* scope,
                                     MaObject* diiaObject,
                                     MaObject* args,
                                     size_t li) {
    const auto listObject = diiaObject->diiaGetBoundObject();

    const auto iteratorObject = M->createObject(M->object_structure_object);
    iteratorObject->setProperty(M, "_список", listObject);

    if (listObject->listGetLength() == 0) {
      iteratorObject->setProperty(M, "завершено", MaValue::Yes());
    } else if (listObject->listGetLength() == 1) {
      iteratorObject->setProperty(M, "завершено", MaValue::No());
      iteratorObject->setProperty(M, "значення", listObject->listGetAt(M, 0));
      const auto nextDiiaObject = M->createNativeDiia(
          "далі",
          [](MaMa* M, MaObject* scope, MaObject* diiaObject, MaObject* args,
             size_t li) {
            const auto iteratorObject = diiaObject->diiaGetBoundObject();
            iteratorObject->setProperty(M, "завершено", MaValue::Yes());
            return MaValue::Empty();
          },
          iteratorObject);
      iteratorObject->setProperty(M, "далі", nextDiiaObject);
    } else {
      iteratorObject->setProperty(M, "завершено", MaValue::No());
      iteratorObject->setProperty(M, "значення", listObject->listGetAt(M, 0));
      iteratorObject->setProperty(M, "_індекс", MaValue::Number(1));
      const auto nextDiiaObject = M->createNativeDiia(
          "далі",
          [](MaMa* M, MaObject* scope, MaObject* diiaObject, MaObject* args,
             size_t li) {
            const auto iteratorObject = diiaObject->diiaGetBoundObject();
            const auto i =
                iteratorObject->getProperty(M, "_індекс").asInteger();
            const auto listObject =
                iteratorObject->getProperty(M, "_список").asObject();
            if (i < listObject->listGetLength()) {
              iteratorObject->setProperty(M, "завершено", MaValue::No());
              iteratorObject->setProperty(M, "значення",
                                          listObject->listGetAt(M, i));
              iteratorObject->setProperty(M, "_індекс",
                                          MaValue::Integer(i + 1));
            } else {
              iteratorObject->setProperty(M, "завершено", MaValue::Yes());
            }
            return MaValue::Empty();
          },
          iteratorObject);
      iteratorObject->setProperty(M, "далі", nextDiiaObject);
    }
    return MaValue::Object(iteratorObject);
  }

  // чародія_отримати
  MaValue MaList_MagGetElementNativeDiiaFn(MaMa* M,
                                           MaObject* scope,
                                           MaObject* diiaObject,
                                           MaObject* args,
                                           size_t li) {
    const auto key = args->getArg(M, "0", "ключ");
    if (key.isNumber()) {
      return diiaObject->diiaGetBoundObject()->listGetAt(M, key.asInteger());
    }
    return MaValue::Empty();
  }

  // чародія_покласти
  MaValue MaList_MagSetElementNativeDiiaFn(MaMa* M,
                                           MaObject* scope,
                                           MaObject* diiaObject,
                                           MaObject* args,
                                           size_t li) {
    const auto key = args->getArg(M, "0", "ключ");
    if (!key.isNumber()) {
      // maybe return error
      return MaValue::Empty();
    }
    const auto value = args->getArg(M, "1", "значення");
    diiaObject->diiaGetBoundObject()->listSetAt(M, key.asInteger(), value);
    return MaValue::Empty();
  }

  // додати
  MaValue MaList_AppendNativeDiiaFn(MaMa* M,
                                    MaObject* scope,
                                    MaObject* diiaObject,
                                    MaObject* args,
                                    size_t li) {
    const auto cell = args->getArg(M, "0", "значення");
    diiaObject->diiaGetBoundObject()->listAppend(M, cell);
    return MaValue::Integer(diiaObject->diiaGetBoundObject()->listGetLength());
  }

  // чародія_містить
  MaValue MaList_MagContainsNativeDiiaFn(MaMa* M,
                                         MaObject* scope,
                                         MaObject* diiaObject,
                                         MaObject* args,
                                         size_t li) {
    const auto cell = args->getArg(M, "0", "значення");
    if (diiaObject->diiaGetBoundObject()->listContains(M, cell)) {
      return MaValue::Yes();
    } else {
      return MaValue::No();
    }
  }

  MaValue MaList_Structure_MagCallNativeDiiaFn(MaMa* M,
                                               MaObject* scope,
                                               MaObject* native_o,
                                               MaObject* args,
                                               size_t li) {
    const auto cell = args->getArg(M, "0", "значення");
    if (cell.isObject()) {
      if (cell.asObject()->isList(M)) {
        return cell;
      }
      return cell.asObject()->getProperty(M, MAG_LIST).call(M, scope, {}, {});
    }
    return MaValue::Error(MaError::Create(
        MaValue::Object(M->createText("Неможливо перетворити на список.")),
        li));
  }

  size_t MaObject::listGetLength() const {
    return this->listData.size();
  }

  void InitList(MaMa* M) {
    const auto listStructureObject = M->createStructure("список");
    listStructureObject->indestructible = true;
    M->global_scope->setProperty(M, "список", listStructureObject);
    M->list_structure_object = listStructureObject;
    listStructureObject->setProperty(
        M, MAG_CALL,
        M->createNativeDiia(MAG_CALL, MaList_Structure_MagCallNativeDiiaFn,
                            listStructureObject));
  }
} // namespace mavka::mama