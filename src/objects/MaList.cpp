#include "../mama.h"

namespace mavka::mama {
  void MaList::Append(const MaValue& cell) {
    cell.Retain();
    this->data.push_back(cell);
  }

  void MaList::SetAt(size_t index, const MaValue& cell) {
    cell.Retain();
    if (index >= 0) {
      if (index >= this->data.size()) {
        // todo: looks bad
        this->data.resize(index + 1);
      }
      this->data[index] = cell;
    }
  }

  MaValue MaList::GetAt(size_t index) const {
    if (index >= 0 && index < this->data.size()) {
      return this->data[index];
    }
    return MaValue::Empty();
  }

  size_t MaList::GetSize() const {
    return this->data.size();
  }

  bool MaList::Contains(const MaValue& cell) {
    for (const auto& item : this->data) {
      if (cell.IsSame(item)) {
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
    const auto list_o = native_o->AsNative()->me;

    const auto iterator_o = MaObject::Empty(M);
    iterator_o->SetProperty(M, "_список", list_o);

    if (list_o->AsList()->GetSize() == 0) {
      iterator_o->SetProperty(M, "завершено", MaValue::Yes());
    } else if (list_o->AsList()->GetSize() == 1) {
      iterator_o->SetProperty(M, "завершено", MaValue::No());
      iterator_o->SetProperty(M, "значення", list_o->AsList()->GetAt(0));
      const auto next_native_o = MaNative::Create(
          M, "далі",
          [](MaMa* M, MaObject* native_o, MaArgs* args,
             const MaLocation& location) {
            const auto iterator_object = native_o->AsNative()->GetMe();
            iterator_object->SetProperty(M, "завершено", MaValue::Yes());
            return MaValue::Empty();
          },
          iterator_o);
      iterator_o->SetProperty(M, "далі", next_native_o);
    } else {
      iterator_o->SetProperty(M, "завершено", MaValue::No());
      iterator_o->SetProperty(M, "значення", list_o->AsList()->GetAt(0));
      iterator_o->SetProperty(M, "_індекс", MaValue::Number(1));
      const auto next_native_o = MaNative::Create(
          M, "далі",
          [](MaMa* M, MaObject* native_o, MaArgs* args,
             const MaLocation& location) {
            const auto iterator_o = native_o->AsNative()->GetMe();
            const auto i = iterator_o->GetPropertyStrong(M, "_індекс").AsInteger();
            const auto list =
                iterator_o->GetPropertyStrong(M, "_список").AsObject()->AsList();
            if (i < list->GetSize()) {
              iterator_o->SetProperty(M, "завершено", MaValue::No());
              iterator_o->SetProperty(M, "значення", list->GetAt(i));
              iterator_o->SetProperty(M, "_індекс", MaValue::Integer(i + 1));
            } else {
              iterator_o->SetProperty(M, "завершено", MaValue::Yes());
            }
            return MaValue::Empty();
          },
          iterator_o);
      iterator_o->SetProperty(M, "далі", next_native_o);
    }
    return MaValue::Object(iterator_o);
  }

  // чародія_отримати
  MaValue MaList_MagGetElementNativeDiiaFn(MaMa* M,
                                           MaObject* native_o,
                                           MaArgs* args,
                                           const MaLocation& location) {
    const auto key = args->Get(0, "ключ");
    if (key.IsNumber()) {
      return native_o->AsNative()->GetMe()->AsList()->GetAt(key.AsInteger());
    }
    return MaValue::Empty();
  }

  // чародія_покласти
  MaValue MaList_MagSetElementNativeDiiaFn(MaMa* M,
                                           MaObject* native_o,
                                           MaArgs* args,
                                           const MaLocation& location) {
    const auto key = args->Get(0, "ключ");
    if (!key.IsNumber()) {
      // maybe return error
      return MaValue::Empty();
    }
    const auto value = args->Get(1, "значення");
    native_o->AsNative()->GetMe()->AsList()->SetAt(key.AsInteger(), value);
    return MaValue::Empty();
  }

  // додати
  MaValue MaList_AppendNativeDiiaFn(MaMa* M,
                                    MaObject* native_o,
                                    MaArgs* args,
                                    const MaLocation& location) {
    const auto cell = args->Get(0, "значення");
    native_o->AsNative()->GetMe()->AsList()->Append(cell);
    return MaValue::Integer(native_o->AsNative()->GetMe()->AsList()->GetSize());
  }

  // чародія_містить
  MaValue MaList_MagContainsNativeDiiaFn(MaMa* M,
                                         MaObject* native_o,
                                         MaArgs* args,
                                         const MaLocation& location) {
    const auto cell = args->Get(0, "значення");
    if (native_o->AsNative()->GetMe()->AsList()->Contains(cell)) {
      return MaValue::Yes();
    } else {
      return MaValue::No();
    }
  }

  MaValue MaListGetHandler(MaMa* M, MaObject* o, const std::string& name) {
    if (name == "довжина") {
      return MaValue::Integer(o->AsList()->GetSize());
    }
    return o->GetPropertyStrongDirect(M, name);
  }

  MaObject* MaList::Create(MaMa* M) {
    const auto list = new MaList();
    const auto list_o =
        MaObject::Instance(M, MA_OBJECT_LIST, M->list_structure_object, list);
    list_o->get = MaListGetHandler;
    list_o->SetProperty(
        M, MAG_ITERATOR,
        MaNative::Create(M, MAG_ITERATOR, MaList_MagIteratorNativeFn, list_o));
    list_o->SetProperty(
        M, MAG_GET_ELEMENT,
        MaNative::Create(M, MAG_GET_ELEMENT, MaList_MagGetElementNativeDiiaFn,
                         list_o));
    list_o->SetProperty(
        M, MAG_SET_ELEMENT,
        MaNative::Create(M, MAG_SET_ELEMENT, MaList_MagSetElementNativeDiiaFn,
                         list_o));
    list_o->SetProperty(
        M, "додати",
        MaNative::Create(M, "додати", MaList_AppendNativeDiiaFn, list_o));
    list_o->SetProperty(
        M, MAG_CONTAINS,
        MaNative::Create(M, MAG_CONTAINS, MaList_MagContainsNativeDiiaFn,
                         list_o));
    return list_o;
  }

  MaValue MaList_Structure_MagCallNativeDiiaFn(MaMa* M,
                                               MaObject* native_o,
                                               MaArgs* args,
                                               const MaLocation& location) {
    const auto cell = args->Get(0, "значення");
    if (cell.IsObject()) {
      if (cell.AsObject()->IsList()) {
        return cell;
      }
      return cell.AsObject()->GetPropertyStrong(M, MAG_LIST).Call(M, {}, {});
    }
    return MaValue::Error(new MaError(
        MaValue::Object(MaText::Create(M, "Неможливо перетворити на список.")),
        location));
  }

  void MaList::Init(MaMa* M) {
    const auto list_structure_object = MaStructure::Create(M, "список");
    M->global_scope->SetSubject("список", list_structure_object);
    M->list_structure_object = list_structure_object;
    list_structure_object->SetProperty(
        M, MAG_CALL,
        MaNative::Create(M, MAG_CALL, MaList_Structure_MagCallNativeDiiaFn,
                         list_structure_object));
  }
} // namespace mavka::mama