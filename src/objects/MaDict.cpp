#include "../mama.h"

namespace mavka::mama {
  void MaDict::setAt(MaMa* M, const MaValue& key, const MaValue& value) {
    if (key.isObject()) {
      key.asObject()->retain();
    }
    if (value.isObject()) {
      value.asObject()->retain();
    }
    for (auto& item : this->data) {
      if (key.isEqual(M, item.first)) {
        if (item.second.isObject()) {
          //          item.second.asObject()->release();
        }
        item.second = value;
        return;
      }
    }
    this->data.emplace_back(key, value);
  }

  MaValue MaDict::getAt(MaMa* M, const MaValue& key) const {
    for (const auto& item : this->data) {
      if (key.isEqual(M, item.first)) {
        return item.second;
      }
    }
    return MaValue::Empty();
  }

  size_t MaDict::getSize() const {
    return this->data.size();
  }

  // чародія_отримати
  MaValue MaDict_MagGetElementNativeDiiaFn(MaMa* M,
                                           MaObject* native_o,
                                           MaObject* args,
                                           const MaLocation& location) {
    const auto key = args->getArg(M, "0", "ключ");
    return native_o->asDiia()->getMe()->asDict()->getAt(M, key);
  }

  // чародія_покласти
  MaValue MaDict_MagSetElementNativeDiiaFn(MaMa* M,
                                           MaObject* native_o,
                                           MaObject* args,
                                           const MaLocation& location) {
    const auto key = args->getArg(M, "0", "ключ");
    const auto value = args->getArg(M, "1", "значення");
    native_o->asDiia()->getMe()->asDict()->setAt(M, key, value);
    return MaValue::Empty();
  }

  MaObject* MaDict::Create(MaMa* M) {
    const auto dict = new MaDict();
    const auto dict_o = MaObject::Instance(M, M->dict_structure_object, dict);
    dict_o->setProperty(
        M, MAG_GET_ELEMENT,
        MaDiia::Create(M, MAG_GET_ELEMENT, MaDict_MagGetElementNativeDiiaFn,
                       dict_o));
    dict_o->setProperty(
        M, MAG_SET_ELEMENT,
        MaDiia::Create(M, MAG_SET_ELEMENT, MaDict_MagSetElementNativeDiiaFn,
                       dict_o));
    return dict_o;
  }

  void MaDict::Init(MaMa* M) {
    const auto dict_structure_object = MaStructure::Create(M, "словник");
    M->global_scope->setProperty(M, "словник", dict_structure_object);
    M->dict_structure_object = dict_structure_object;
  }
} // namespace mavka::mama