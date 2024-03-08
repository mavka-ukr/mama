#include "mama.h"

namespace mavka::mama {
  MaValue MaValue::Call(MaMa* M,
                        MaArgs* args,
                        const MaLocation& location) const {
    auto cell = *this;
  repeat:
    if (cell.IsObject()) {
      const auto object = cell.v.object;

      if (object->HasProperty(M, MAG_CALL)) {
        cell = object->GetProperty(M, MAG_CALL);
        goto repeat;
      } else if (object->call) {
        FRAME_PUSH(new MaFrame(M->frame_stack.top()->scope, object,
                               M->frame_stack.top()->module, location));
        const auto result = object->call(M, object, args, location);
        FRAME_POP();
        return result;
      }
    }
    DO_RETURN_CANNOT_CALL_CELL_ERROR(cell, location);
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

  bool MaValue::IsSame(const MaValue& other) const {
    if (other.IsEmpty()) {
      if (this->IsEmpty()) {
        return true;
      }
      return false;
    }
    if (other.IsNumber()) {
      if (this->IsNumber()) {
        return this->AsNumber() == other.AsNumber();
      }
      return false;
    }
    if (other.IsYes()) {
      if (this->IsYes()) {
        return true;
      }
      return false;
    }
    if (other.IsNo()) {
      if (this->IsNo()) {
        return true;
      }
      return false;
    }
    if (other.IsObject()) {
      if (this->IsObject()) {
        const auto thisObject = this->AsObject();
        const auto otherObject = other.AsObject();
        if (thisObject == otherObject) {
          return true;
        }
        if (thisObject->IsText() && otherObject->IsText()) {
          return thisObject->d.text->data == otherObject->d.text->data;
        }
      }
      return false;
    }
    return false;
  }
} // namespace mavka::mama