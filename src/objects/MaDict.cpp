#include "../mama.h"

namespace mavka::mama {
  void MaDict::Set(const MaCell& key, const MaCell& value) {
    for (auto& item : this->data) {
      if (key.IsSame(item.first)) {
        item.second = value;
        return;
      }
    }
    this->data.push_back({key, value});
  }

  MaCell MaDict::Get(const MaCell& key) const {
    for (const auto& item : this->data) {
      if (key.IsSame(item.first)) {
        return item.second;
      }
    }
    return MaCell::Empty();
  }

  void MaDict::Remove(const MaCell& key) {
    size_t index = 0;
    for (const auto& item : this->data) {
      if (key.IsSame(item.first)) {
        this->data.erase(this->data.begin() + index);
        return;
      }
      ++index;
    }
  }

  size_t MaDict::Size() const {
    return this->data.size();
  }

  // чародія_отримати
  MaCell MaDict_MagGetElementNativeDiiaFn(MaMa* M,
                                          MaObject* o,
                                          MaArgs* args,
                                          const MaLocation& location) {
    const auto key = args->Get(0, "ключ");
    return o->d.diia_native->me->d.dict->Get(key);
  }

  // чародія_покласти
  MaCell MaDict_MagSetElementNativeDiiaFn(MaMa* M,
                                          MaObject* o,
                                          MaArgs* args,
                                          const MaLocation& location) {
    const auto key = args->Get(0, "ключ");
    const auto value = args->Get(1, "значення");
    o->d.diia_native->me->d.dict->Set(key, value);
    return MaCell::Empty();
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