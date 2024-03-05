#include "../mama.h"

namespace mavka::mama {
  void MaList::append(const MaCell cell) {
    this->data.push_back(cell);
  }

  void MaList::set(const size_t index, const MaCell cell) {
    if (index >= 0) {
      if (index >= this->data.size()) {
        this->data.resize(index + 1);
      }
      this->data[index] = cell;
    }
  }

  MaCell MaList::get(const size_t index) const {
    if (index >= 0 && index < this->data.size()) {
      return this->data[index];
    }
    return MA_MAKE_EMPTY();
  }

  size_t MaList::size() const {
    return this->data.size();
  }

  bool MaList::contains(MaCell cell) {
    for (const auto& item : this->data) {
      if (item.type == MA_CELL_EMPTY) {
        if (cell.type == MA_CELL_EMPTY) {
          return true;
        }
      } else if (item.type == MA_CELL_NUMBER) {
        if (cell.type == MA_CELL_NUMBER) {
          if (item.v.number == cell.v.number) {
            return true;
          }
        }
      } else if (item.type == MA_CELL_YES) {
        if (cell.type == MA_CELL_YES) {
          return true;
        }
      } else if (item.type == MA_CELL_NO) {
        if (cell.type == MA_CELL_NO) {
          return true;
        }
      } else if (item.type == MA_CELL_OBJECT) {
        if (cell.type == MA_CELL_OBJECT) {
          if (item.v.object->type == MA_OBJECT_STRING) {
            if (cell.v.object->type == MA_OBJECT_STRING) {
              if (item.v.object->d.string->data ==
                  cell.v.object->d.string->data) {
                return true;
              }
            }
          } else {
            if (item.v.object == cell.v.object) {
              return true;
            }
          }
        }
      }
    }
    return false;
  }

  // чародія_перебір
  MaCell MaList_MagIteratorDiiaNativeFn(MaMa* M, MaObject* o, MaArgs* args) {
    const auto list_object = o->d.diia_native->me;
    const auto iterator_object = MaObject::Empty(M);

    iterator_object->SetProperty("_список",
                                 MaCell{MA_CELL_OBJECT, {.object = o}});

    if (list_object->d.list->size() == 0) {
      iterator_object->SetProperty("завершено", MA_MAKE_YES());
    } else if (list_object->d.list->size() == 1) {
      iterator_object->SetProperty("завершено", MA_MAKE_NO());
      iterator_object->SetProperty("значення", list_object->d.list->data[0]);
      const auto next_diia_native_cell = MA_MAKE_OBJECT(MaDiiaNative::Create(
          M, "далі",
          [](MaMa* M, MaObject* iterator_o, MaArgs* args) {
            const auto iterator_object = iterator_o->d.diia_native->me;
            iterator_object->SetProperty("завершено", MA_MAKE_YES());
            return MA_MAKE_EMPTY();
          },
          iterator_object));
      iterator_object->SetProperty("далі", next_diia_native_cell);
    } else {
      iterator_object->SetProperty("завершено", MA_MAKE_NO());
      iterator_object->SetProperty("значення", list_object->d.list->data[0]);
      iterator_object->SetProperty("_індекс", MA_MAKE_NUMBER(1));
      const auto next_diia_native_cell = MA_MAKE_OBJECT(MaDiiaNative::Create(
          M, "далі",
          [](MaMa* M, MaObject* iterator_o, MaArgs* args) {
            const auto iterator_object = iterator_o->d.diia_native->me;
            const auto i = iterator_object->properties["_індекс"].v.number;
            const auto list =
                iterator_o->properties["_список"].v.object->d.list;
            if (i < list->size()) {
              iterator_object->SetProperty("завершено", MA_MAKE_NO());
              iterator_object->SetProperty("значення", list->data[i]);
              iterator_object->SetProperty("_індекс", MA_MAKE_NUMBER(i + 1));
            } else {
              iterator_object->SetProperty("завершено", MA_MAKE_YES());
            }
            return MA_MAKE_EMPTY();
          },
          iterator_object));
      iterator_object->SetProperty("далі", next_diia_native_cell);
    }
    return MA_MAKE_OBJECT(iterator_object);
  }

  // чародія_отримати
  MaCell MaList_MagGetElementNativeDiiaFn(MaMa* M, MaObject* o, MaArgs* args) {
    const auto key = MA_ARGS_GET(args, 0, "ключ", MA_MAKE_EMPTY());
    if (!IS_EMPTY(key)) {
      RETURN(o->d.diia_native->me->d.list->get(key.v.number));
    }
    RETURN_EMPTY();
  }

  // чародія_покласти
  MaCell MaList_MagSetElementNativeDiiaFn(MaMa* M, MaObject* o, MaArgs* args) {
    const auto key = MA_ARGS_GET(args, 0, "ключ", MA_MAKE_EMPTY());
    const auto value = MA_ARGS_GET(args, 1, "значення", MA_MAKE_EMPTY());
    o->d.diia_native->me->d.list->set(key.v.number, value);
    RETURN_EMPTY();
  }

  // додати
  MaCell MaList_AppendNativeDiiaFn(MaMa* M, MaObject* o, MaArgs* args) {
    const auto cell = MA_ARGS_GET(args, 0, "значення", MA_MAKE_EMPTY());
    o->d.diia_native->me->d.list->append(cell);
    RETURN_INTEGER(o->d.diia_native->me->d.list->size());
  }

  // чародія_містить
  MaCell MaList_MagContainsNativeDiiaFn(MaMa* M, MaObject* o, MaArgs* args) {
    const auto cell = MA_ARGS_GET(args, 0, "значення", MA_MAKE_EMPTY());
    if (o->d.diia_native->me->d.list->contains(cell)) {
      RETURN_YES();
    } else {
      RETURN_NO();
    }
  }

  MaCell MaListGetHandler(MaMa* M, MaObject* me, const std::string& name) {
    if (name == "довжина") {
      return MA_MAKE_INTEGER(me->d.list->size());
    }
    if (!me->properties.contains(name)) {
      RETURN_ERROR(new MaError(MA_MAKE_OBJECT(MaText::Create(
          M,
          "Властивість \"" + name + "\" не визначено для типу \"список\"."))));
    }
    return me->properties[name];
  }

  MaObject* MaList::Create(MaMa* M) {
    const auto list = new MaList();
    const auto list_object =
        MaObject::Instance(M, MA_OBJECT_LIST, M->list_structure_object, list);
    list_object->get = MaListGetHandler;
    list_object->SetProperty(
        MAG_ITERATOR,
        MA_MAKE_OBJECT(MaDiiaNative::Create(
            M, MAG_ITERATOR, MaList_MagIteratorDiiaNativeFn, list_object)));
    list_object->SetProperty(
        MAG_GET_ELEMENT, MA_MAKE_OBJECT(MaDiiaNative::Create(
                             M, MAG_GET_ELEMENT,
                             MaList_MagGetElementNativeDiiaFn, list_object)));
    list_object->SetProperty(
        MAG_SET_ELEMENT, MA_MAKE_OBJECT(MaDiiaNative::Create(
                             M, MAG_SET_ELEMENT,
                             MaList_MagSetElementNativeDiiaFn, list_object)));
    list_object->SetProperty(
        "додати", MA_MAKE_OBJECT(MaDiiaNative::Create(
                      M, "додати", MaList_AppendNativeDiiaFn, list_object)));
    list_object->SetProperty(
        MAG_CONTAINS,
        MA_MAKE_OBJECT(MaDiiaNative::Create(
            M, MAG_CONTAINS, MaList_MagContainsNativeDiiaFn, list_object)));
    return list_object;
  }

  void MaList::Init(MaMa* M) {
    const auto list_structure_object = MaStructure::Create(M, "список");
    M->global_scope->SetSubject("список",
                                MA_MAKE_OBJECT(list_structure_object));
    M->list_structure_object = list_structure_object;
  }
} // namespace mavka::mama