#include "../mama.h"

namespace mavka::mama {
  MaValue MaObject::dictGetAt(MaMa* M, const MaValue& key) const {
    for (const auto& item : this->dictData) {
      if (key.isEqual(M, item.first)) {
        return item.second;
      }
    }
    return MaValue::Empty();
  }

  void MaObject::dictSetAt(MaMa* M, const MaValue& key, const MaValue& value) {
    if (key.isObject()) {
      key.asObject()->retain();
    }
    if (value.isObject()) {
      value.asObject()->retain();
    }
    for (auto& item : this->dictData) {
      if (key.isEqual(M, item.first)) {
        if (item.second.isObject()) {
          item.second.asObject()->release();
        }
        item.second = value;
        return;
      }
    }
    this->dictData.emplace_back(key, value);
  }

  size_t MaObject::dictGetSize() const {
    return this->dictData.size();
  }

  // чародія_отримати
  MaValue MaDict_MagGetElementNativeDiiaFn(MaMa* M,
                                           MaObject* scope,
                                           MaObject* diiaObject,
                                           MaObject* args,
                                           size_t li) {
    const auto key = args->getArg(M, "0", "ключ");
    return diiaObject->diiaGetBoundObject()->dictGetAt(M, key);
  }

  // чародія_покласти
  MaValue MaDict_MagSetElementNativeDiiaFn(MaMa* M,
                                           MaObject* scope,
                                           MaObject* diiaObject,
                                           MaObject* args,
                                           size_t li) {
    const auto key = args->getArg(M, "0", "ключ");
    const auto value = args->getArg(M, "1", "значення");
    diiaObject->diiaGetBoundObject()->dictSetAt(M, key, value);
    return MaValue::Empty();
  }

  void InitDict(MaMa* M) {
    const auto dictStructureObject = M->createStructure("словник");
    dictStructureObject->indestructible = true;
    M->global_scope->setProperty(M, "словник", dictStructureObject);
    M->dict_structure_object = dictStructureObject;
  }
} // namespace mavka::mama