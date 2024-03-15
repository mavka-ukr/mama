#include "mama.h"

namespace mavka::mama {
  MaValue MaValue::Call(MaMa* M,
                        MaArgs* args,
                        const MaLocation& location) const {
    if (this->isObject()) {
      return this->asObject()->call(M, args, location);
    }
    return MaValue::Error(MaError::Create(
        M, "Неможливо викликати обʼєкт структури \"" + this->GetName() + "\".",
        location));
  };

  MaValue MaValue::Call(MaMa* M,
                        const std::vector<MaValue>& args,
                        const MaLocation& location) const {
    return this->Call(M, new MaArgs(MA_ARGS_TYPE_POSITIONED, {}, args),
                      location);
  }

  MaValue MaValue::Call(MaMa* M,
                        const std::unordered_map<std::string, MaValue>& args,
                        const MaLocation& location) const {
    return this->Call(M, new MaArgs(MA_ARGS_TYPE_NAMED, args, {}), location);
  }

  bool MaValue::isEqual(MaMa* M, const MaValue& other) const {
    if (other.isEmpty()) {
      if (this->isEmpty()) {
        return true;
      }
      return false;
    }
    if (other.isNumber()) {
      if (this->isNumber()) {
        return this->asNumber() == other.asNumber();
      }
      return false;
    }
    if (other.isYes()) {
      if (this->isYes()) {
        return true;
      }
      return false;
    }
    if (other.isNo()) {
      if (this->isNo()) {
        return true;
      }
      return false;
    }
    if (other.isObject()) {
      if (this->isObject()) {
        const auto thisObject = this->asObject();
        const auto otherObject = other.asObject();
        if (thisObject == otherObject) {
          return true;
        }
        if (thisObject->isText(M) && otherObject->isText(M)) {
          return thisObject->d.text->data == otherObject->d.text->data;
        }
      }
      return false;
    }
    return false;
  }
} // namespace mavka::mama