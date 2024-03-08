#include "../mama.h"

namespace mavka::mama {
  void MaList::Append(const MaCell& cell) {
    this->data.push_back(cell);
  }

  void MaList::SetAt(size_t index, const MaCell& cell) {
    if (index >= 0) {
      if (index >= this->data.size()) {
        // todo: looks bad
        this->data.resize(index + 1);
      }
      this->data[index] = cell;
    }
  }

  MaCell MaList::GetAt(size_t index) const {
    if (index >= 0 && index < this->data.size()) {
      return this->data[index];
    }
    return MaCell::Empty();
  }

  size_t MaList::GetSize() const {
    return this->data.size();
  }

  bool MaList::Contains(const MaCell& cell) {
    for (const auto& item : this->data) {
      if (cell.IsSame(item)) {
        return true;
      }
    }
    return false;
  }

  // чародія_перебір
  MaCell MaList_MagIteratorNativeFn(MaMa* M,
                                    MaObject* native_o,
                                    MaArgs* args,
                                    const MaLocation& location) {
    const auto list_o = native_o->AsNative()->me;

    const auto iterator_o = MaObject::Empty(M);
    iterator_o->SetProperty("_список", list_o);

    if (list_o->AsList()->GetSize() == 0) {
      iterator_o->SetProperty("завершено", MaCell::Yes());
    } else if (list_o->AsList()->GetSize() == 1) {
      iterator_o->SetProperty("завершено", MaCell::No());
      iterator_o->SetProperty("значення", list_o->AsList()->GetAt(0));
      const auto next_native_o = MaNative::Create(
          M, "далі",
          [](MaMa* M, MaObject* native_o, MaArgs* args,
             const MaLocation& location) {
            const auto iterator_object = native_o->AsNative()->GetMe();
            iterator_object->SetProperty("завершено", MaCell::Yes());
            return MaCell::Empty();
          },
          iterator_o);
      iterator_o->SetProperty("далі", next_native_o);
    } else {
      iterator_o->SetProperty("завершено", MaCell::No());
      iterator_o->SetProperty("значення", list_o->AsList()->GetAt(0));
      iterator_o->SetProperty("_індекс", MaCell::Number(1));
      const auto next_native_o = MaNative::Create(
          M, "далі",
          [](MaMa* M, MaObject* native_o, MaArgs* args,
             const MaLocation& location) {
            const auto iterator_o = native_o->AsNative()->GetMe();
            const auto i = iterator_o->GetProperty("_індекс").AsInteger();
            const auto list =
                iterator_o->GetProperty("_список").AsObject()->AsList();
            if (i < list->GetSize()) {
              iterator_o->SetProperty("завершено", MaCell::No());
              iterator_o->SetProperty("значення", list->GetAt(i));
              iterator_o->SetProperty("_індекс", MaCell::Integer(i + 1));
            } else {
              iterator_o->SetProperty("завершено", MaCell::Yes());
            }
            return MaCell::Empty();
          },
          iterator_o);
      iterator_o->SetProperty("далі", next_native_o);
    }
    return MaCell::Object(iterator_o);
  }

  // чародія_отримати
  MaCell MaList_MagGetElementNativeDiiaFn(MaMa* M,
                                          MaObject* native_o,
                                          MaArgs* args,
                                          const MaLocation& location) {
    const auto key = args->Get(0, "ключ");
    if (key.IsNumber()) {
      return native_o->AsNative()->GetMe()->AsList()->GetAt(key.AsInteger());
    }
    return MaCell::Empty();
  }

  // чародія_покласти
  MaCell MaList_MagSetElementNativeDiiaFn(MaMa* M,
                                          MaObject* native_o,
                                          MaArgs* args,
                                          const MaLocation& location) {
    const auto key = args->Get(0, "ключ");
    if (!key.IsNumber()) {
      // maybe return error
      return MaCell::Empty();
    }
    const auto value = args->Get(1, "значення");
    native_o->AsNative()->GetMe()->AsList()->SetAt(key.AsInteger(), value);
    return MaCell::Empty();
  }

  // додати
  MaCell MaList_AppendNativeDiiaFn(MaMa* M,
                                   MaObject* native_o,
                                   MaArgs* args,
                                   const MaLocation& location) {
    const auto cell = args->Get(0, "значення");
    native_o->AsNative()->GetMe()->AsList()->Append(cell);
    return MaCell::Integer(native_o->AsNative()->GetMe()->AsList()->GetSize());
  }

  // чародія_містить
  MaCell MaList_MagContainsNativeDiiaFn(MaMa* M,
                                        MaObject* native_o,
                                        MaArgs* args,
                                        const MaLocation& location) {
    const auto cell = args->Get(0, "значення");
    if (native_o->AsNative()->GetMe()->AsList()->Contains(cell)) {
      return MaCell::Yes();
    } else {
      return MaCell::No();
    }
  }

  MaCell MaListGetHandler(MaMa* M, MaObject* me, const std::string& name) {
    if (name == "довжина") {
      return MaCell::Integer(me->AsList()->GetSize());
    }
    return me->GetPropertyDirectOrEmpty(name);
  }

  MaObject* MaList::Create(MaMa* M) {
    const auto list = new MaList();
    const auto list_o =
        MaObject::Instance(M, MA_OBJECT_LIST, M->list_structure_object, list);
    list_o->get = MaListGetHandler;
    list_o->SetProperty(
        MAG_ITERATOR,
        MaNative::Create(M, MAG_ITERATOR, MaList_MagIteratorNativeFn, list_o));
    list_o->SetProperty(
        MAG_GET_ELEMENT,
        MaNative::Create(M, MAG_GET_ELEMENT, MaList_MagGetElementNativeDiiaFn,
                         list_o));
    list_o->SetProperty(
        MAG_SET_ELEMENT,
        MaNative::Create(M, MAG_SET_ELEMENT, MaList_MagSetElementNativeDiiaFn,
                         list_o));
    list_o->SetProperty(
        "додати",
        MaNative::Create(M, "додати", MaList_AppendNativeDiiaFn, list_o));
    list_o->SetProperty(
        MAG_CONTAINS, MaNative::Create(M, MAG_CONTAINS,
                                       MaList_MagContainsNativeDiiaFn, list_o));
    return list_o;
  }

  void MaList::Init(MaMa* M) {
    const auto list_structure_object = MaStructure::Create(M, "список");
    M->global_scope->SetSubject("список", list_structure_object);
    M->list_structure_object = list_structure_object;
  }
} // namespace mavka::mama