#include "../mama.h"

namespace mavka::mama {
  void MaDict::set(const MaCell key, const MaCell value) {
    for (auto& item : this->data) {
      if (key.type == MA_CELL_EMPTY) {
        if (item.first.type == MA_CELL_EMPTY) {
          item.second = value;
          return;
        }
      } else if (key.type == MA_CELL_NUMBER) {
        if (item.first.type == MA_CELL_NUMBER &&
            item.first.v.number == key.v.number) {
          item.second = value;
          return;
        }
      } else if (key.type == MA_CELL_OBJECT) {
        if (item.first.type == MA_CELL_OBJECT) {
          const auto key_object = key.v.object;
          const auto item_object = item.first.v.object;
          if (key_object->type == MA_OBJECT_STRING) {
            if (item_object->type == MA_OBJECT_STRING) {
              if (key_object->d.string->data == item_object->d.string->data) {
                item.second = value;
                return;
              }
            }
          } else {
            if (key_object == item_object) {
              item.second = value;
              return;
            }
          }
        }
      }
    }
    this->data.push_back({key, value});
  }

  MaCell MaDict::get(const MaCell key) const {
    for (const auto& item : this->data) {
      if (key.type == MA_CELL_EMPTY) {
        if (item.first.type == MA_CELL_EMPTY) {
          return item.second;
        }
      } else if (key.type == MA_CELL_NUMBER) {
        if (item.first.type == MA_CELL_NUMBER &&
            item.first.v.number == key.v.number) {
          return item.second;
        }
      } else if (key.type == MA_CELL_OBJECT) {
        if (item.first.type == MA_CELL_OBJECT) {
          const auto key_object = key.v.object;
          const auto item_object = item.first.v.object;
          if (key_object->type == MA_OBJECT_STRING) {
            if (item_object->type == MA_OBJECT_STRING) {
              if (key_object->d.string->data == item_object->d.string->data) {
                return item.second;
              }
            }
          } else {
            if (key_object == item_object) {
              return item.second;
            }
          }
        }
      }
    }
    return MaCell::Empty();
  }

  void MaDict::remove(MaCell key) {
    long index = 0;
    for (const auto& item : this->data) {
      if (key.type == MA_CELL_EMPTY) {
        if (item.first.type == MA_CELL_EMPTY) {
          this->data.erase(this->data.begin() + index);
          return;
        }
      } else if (key.type == MA_CELL_NUMBER) {
        if (item.first.type == MA_CELL_NUMBER &&
            item.first.v.number == key.v.number) {
          this->data.erase(this->data.begin() + index);
          return;
        }
      } else if (key.type == MA_CELL_OBJECT) {
        if (item.first.type == MA_CELL_OBJECT) {
          const auto key_object = key.v.object;
          const auto item_object = item.first.v.object;
          if (key_object->type == MA_OBJECT_STRING) {
            if (item_object->type == MA_OBJECT_STRING) {
              if (key_object->d.string->data == item_object->d.string->data) {
                this->data.erase(this->data.begin() + index);
                return;
              }
            }
          } else {
            if (key_object == item_object) {
              this->data.erase(this->data.begin() + index);
              return;
            }
          }
        }
      }
      ++index;
    }
  }

  size_t MaDict::size() const {
    return this->data.size();
  }

  // чародія_отримати
  MaCell MaDict_MagGetElementNativeDiiaFn(MaMa* M, MaObject* o, MaArgs* args) {
    const auto key = args->Get(0, "ключ");
    RETURN(o->d.diia_native->me->d.dict->get(key));
  }

  // чародія_покласти
  MaCell MaDict_MagSetElementNativeDiiaFn(MaMa* M, MaObject* o, MaArgs* args) {
    const auto key = args->Get(0, "ключ");
    const auto value = args->Get(1, "значення");
    o->d.diia_native->me->d.dict->set(key, value);
    RETURN_EMPTY();
  }

  MaObject* MaDict::Create(MaMa* M) {
    const auto dict = new MaDict();
    const auto dict_object =
        MaObject::Instance(M, MA_OBJECT_DICT, M->dict_structure_object, dict);
    dict_object->SetProperty(
        MAG_GET_ELEMENT,
        MaDiiaNative::Create(M, MAG_GET_ELEMENT,
                             MaDict_MagGetElementNativeDiiaFn, dict_object));
    dict_object->SetProperty(
        MAG_SET_ELEMENT,
        MaDiiaNative::Create(M, MAG_SET_ELEMENT,
                             MaDict_MagSetElementNativeDiiaFn, dict_object));
    return dict_object;
  }

  void MaDict::Init(MaMa* M) {
    const auto dict_structure_object = MaStructure::Create(M, "словник");
    M->global_scope->SetSubject("словник", dict_structure_object);
    M->dict_structure_object = dict_structure_object;
  }
} // namespace mavka::mama