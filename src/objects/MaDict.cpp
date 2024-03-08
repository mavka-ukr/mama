#include "../mama.h"

namespace mavka::mama {
  void MaDict::Set(const MaCell& key, const MaCell& value) {
    for (auto& item : this->data) {
      if (key.IsSame(item.first)) {
        item.second = value;
        return;
      }
    }
    this->data.emplace_back(key, value);
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
    long index = 0;
    for (const auto& item : this->data) {
      if (key.IsSame(item.first)) {
        this->data.erase(this->data.begin() + index);
        return;
      }
      ++index;
    }
  }

  size_t MaDict::GetSize() const {
    return this->data.size();
  }

  // чародія_отримати
  MaCell MaDict_MagGetElementNativeDiiaFn(MaMa* M,
                                          MaObject* native_o,
                                          MaArgs* args,
                                          const MaLocation& location) {
    const auto key = args->Get(0, "ключ");
    return native_o->AsNative()->GetMe()->AsDict()->Get(key);
  }

  // чародія_покласти
  MaCell MaDict_MagSetElementNativeDiiaFn(MaMa* M,
                                          MaObject* native_o,
                                          MaArgs* args,
                                          const MaLocation& location) {
    const auto key = args->Get(0, "ключ");
    const auto value = args->Get(1, "значення");
    native_o->AsNative()->GetMe()->AsDict()->Set(key, value);
    return MaCell::Empty();
  }

  MaCell MaDictGetHandler(MaMa* M, MaObject* me, const std::string& name) {
    if (name == "розмір") {
      return MaCell::Integer(me->AsDict()->GetSize());
    }
    return me->GetPropertyDirectOrEmpty(name);
  }

  MaObject* MaDict::Create(MaMa* M) {
    const auto dict = new MaDict();
    const auto dict_o =
        MaObject::Instance(M, MA_OBJECT_DICT, M->dict_structure_object, dict);
    dict_o->get = MaDictGetHandler;
    dict_o->SetProperty(
        MAG_GET_ELEMENT,
        MaNative::Create(M, MAG_GET_ELEMENT, MaDict_MagGetElementNativeDiiaFn,
                         dict_o));
    dict_o->SetProperty(
        MAG_SET_ELEMENT,
        MaNative::Create(M, MAG_SET_ELEMENT, MaDict_MagSetElementNativeDiiaFn,
                         dict_o));
    return dict_o;
  }

  void MaDict::Init(MaMa* M) {
    const auto dict_structure_object = MaStructure::Create(M, "словник");
    M->global_scope->SetSubject("словник", dict_structure_object);
    M->dict_structure_object = dict_structure_object;
  }
} // namespace mavka::mama