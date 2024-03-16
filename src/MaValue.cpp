#include "mama.h"

namespace mavka::mama {
  MaValue MaValue::call(MaMa* M,
                        MaObject* args,
                        const MaLocation& location) const {
    if (this->isObject()) {
      return this->asObject()->call(M, args, location);
    }
    return MaValue::Error(MaError::Create(
        M, "Неможливо викликати обʼєкт структури \"" + this->getName() + "\".",
        location));
  };

  MaValue MaValue::call(MaMa* M,
                        const std::vector<MaValue>& args,
                        const MaLocation& location) const {
    const auto argsObject = MaObject::Empty(M);
    for (size_t i = 0; i < args.size(); i++) {
      argsObject->setProperty(M, std::to_string(i), args[i]);
    }
    return this->call(M, argsObject, location);
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

  MaValue MaValue::is(MaMa* M,
                      const MaValue& value,
                      const MaLocation& location) const {
    if (this->isEmpty()) {
      if (value.isEmpty()) {
        return MaValue::Yes();
      } else {
        return MaValue::No();
      }
    }
    if (this->isNumber()) {
      if (value.isObject() && value.asObject() == M->number_structure_object) {
        return MaValue::Yes();
      } else {
        return MaValue::No();
      }
    }
    if (this->isYes() || this->isNo()) {
      if (value.isObject() && value.asObject() == M->logical_structure_object) {
        return MaValue::Yes();
      } else {
        return MaValue::No();
      }
    }
    if (this->isObject() && value.isObject()) {
      if (this->asObject()->is(M, value.asObject())) {
        return MaValue::Yes();
      } else {
        return MaValue::No();
      }
    }
    return MaValue::No();
  }

  MaValue MaValue::isGreater(MaMa* M,
                             const MaValue& value,
                             const MaLocation& location) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        if (this->asNumber() > value.asNumber()) {
          return MaValue::Yes();
        } else {
          return MaValue::No();
        }
      }
    } else if (this->isObject()) {
      return this->asObject()->isGreater(M, value, location);
    }
    return MaValue::No();
  }

  MaValue MaValue::isGreaterOrEqual(MaMa* M,
                                    const MaValue& value,
                                    const MaLocation& location) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        if (this->asNumber() >= value.asNumber()) {
          return MaValue::Yes();
        } else {
          return MaValue::No();
        }
      }
    } else if (this->isObject()) {
      return this->asObject()->isGreaterOrEqual(M, value, location);
    }
    return MaValue::No();
  }

  MaValue MaValue::isLesser(MaMa* M,
                            const MaValue& value,
                            const MaLocation& location) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        if (this->asNumber() < value.asNumber()) {
          return MaValue::Yes();
        } else {
          return MaValue::No();
        }
      }
    } else if (this->isObject()) {
      return this->asObject()->isLesser(M, value, location);
    }
    return MaValue::No();
  }

  MaValue MaValue::isLesserOrEqual(MaMa* M,
                                   const MaValue& value,
                                   const MaLocation& location) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        if (this->asNumber() <= value.asNumber()) {
          return MaValue::Yes();
        } else {
          return MaValue::No();
        }
      }
    } else if (this->isObject()) {
      return this->asObject()->isLesserOrEqual(M, value, location);
    }
    return MaValue::No();
  }

  MaValue MaValue::contains(MaMa* M,
                            const MaValue& value,
                            const MaLocation& location) const {
    if (this->isObject()) {
      return this->asObject()->contains(M, value, location);
    }
    return MaValue::No();
  }

  MaValue MaValue::doNot(MaMa* M, const MaLocation& location) const {
    if (this->isEmpty()) {
      return MaValue::Yes();
    } else if (this->isNumber()) {
      if (this->asNumber() == 0.0) {
        return MaValue::Yes();
      } else {
        return MaValue::No();
      }
    } else if (this->isYes()) {
      return MaValue::No();
    } else if (this->isNo()) {
      return MaValue::Yes();
    } else {
      return MaValue::No();
    }
  }

  MaValue MaValue::doNegative(MaMa* M, const MaLocation& location) const {
    if (this->isNumber()) {
      return MaValue::Number(-this->asNumber());
    } else if (this->isObject()) {
      return this->asObject()->doNegative(M, location);
    }
    // todo: error
    return MaValue::Empty();
  }

  MaValue MaValue::doPositive(MaMa* M, const MaLocation& location) const {
    if (this->isNumber()) {
      return MaValue::Number(+this->asNumber());
    } else if (this->isObject()) {
      return this->asObject()->doPositive(M, location);
    }
    // todo: error
    return MaValue::Empty();
  }

  MaValue MaValue::doBNot(MaMa* M, const MaLocation& location) const {
    if (this->isNumber()) {
      return MaValue::Number(
          static_cast<double>(~static_cast<long>(this->asNumber())));
    } else if (this->isObject()) {
      return this->asObject()->doBNot(M, location);
    }
    // todo: error
    return MaValue::Empty();
  }

  MaValue MaValue::doAdd(MaMa* M,
                         const MaValue& value,
                         const MaLocation& location) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(this->asNumber() + value.asNumber());
      }
      // todo: error
      return MaValue::Empty();
    } else if (this->isObject()) {
      return this->asObject()->doAdd(M, value, location);
    }
    // todo: error
    return MaValue::Empty();
  }

  MaValue MaValue::doSub(MaMa* M,
                         const MaValue& value,
                         const MaLocation& location) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(this->asNumber() - value.asNumber());
      }
      // todo: error
      return MaValue::Empty();
    } else if (this->isObject()) {
      return this->asObject()->doSub(M, value, location);
    }
    // todo: error
    return MaValue::Empty();
  }

  MaValue MaValue::doMul(MaMa* M,
                         const MaValue& value,
                         const MaLocation& location) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(this->asNumber() * value.asNumber());
      }
      // todo: error
      return MaValue::Empty();
    } else if (this->isObject()) {
      return this->asObject()->doMul(M, value, location);
    }
    // todo: error
    return MaValue::Empty();
  }

  MaValue MaValue::doDiv(MaMa* M,
                         const MaValue& value,
                         const MaLocation& location) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(this->asNumber() / value.asNumber());
      }
      // todo: error
      return MaValue::Empty();
    } else if (this->isObject()) {
      return this->asObject()->doDiv(M, value, location);
    }
    // todo: error
    return MaValue::Empty();
  }

  MaValue MaValue::doMod(MaMa* M,
                         const MaValue& value,
                         const MaLocation& location) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(fmod(this->asNumber(), value.asNumber()));
      }
      // todo: error
      return MaValue::Empty();
    } else if (this->isObject()) {
      return this->asObject()->doMod(M, value, location);
    }
    // todo: error
    return MaValue::Empty();
  }

  MaValue MaValue::doDivDiv(MaMa* M,
                            const MaValue& value,
                            const MaLocation& location) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(floor(this->asNumber() / value.asNumber()));
      }
      // todo: error
      return MaValue::Empty();
    } else if (this->isObject()) {
      return this->asObject()->doDivDiv(M, value, location);
    }
    // todo: error
    return MaValue::Empty();
  }

  MaValue MaValue::doPow(MaMa* M,
                         const MaValue& value,
                         const MaLocation& location) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(pow(this->asNumber(), value.asNumber()));
      }
      // todo: error
      return MaValue::Empty();
    } else if (this->isObject()) {
      return this->asObject()->doPow(M, value, location);
    }
    // todo: error
    return MaValue::Empty();
  }

  MaValue MaValue::doXor(MaMa* M,
                         const MaValue& value,
                         const MaLocation& location) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(
            static_cast<double>(static_cast<long>(this->asNumber()) ^
                                static_cast<long>(value.asNumber())));
      }
      // todo: error
      return MaValue::Empty();
    } else if (this->isObject()) {
      return this->asObject()->doXor(M, value, location);
    }
    // todo: error
    return MaValue::Empty();
  }

  MaValue MaValue::doBor(MaMa* M,
                         const MaValue& value,
                         const MaLocation& location) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(
            static_cast<double>(static_cast<long>(this->asNumber()) |
                                static_cast<long>(value.asNumber())));
      }
      // todo: error
      return MaValue::Empty();
    } else if (this->isObject()) {
      return this->asObject()->doBor(M, value, location);
    }
    // todo: error
    return MaValue::Empty();
  }

  MaValue MaValue::doBand(MaMa* M,
                          const MaValue& value,
                          const MaLocation& location) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(
            static_cast<double>(static_cast<long>(this->asNumber()) &
                                static_cast<long>(value.asNumber())));
      }
      // todo: error
      return MaValue::Empty();
    } else if (this->isObject()) {
      return this->asObject()->doBand(M, value, location);
    }
    // todo: error
    return MaValue::Empty();
  }

  MaValue MaValue::doShl(MaMa* M,
                         const MaValue& value,
                         const MaLocation& location) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(
            static_cast<double>(static_cast<long>(this->asNumber())
                                << static_cast<long>(value.asNumber())));
      }
      // todo: error
      return MaValue::Empty();
    } else if (this->isObject()) {
      return this->asObject()->doShl(M, value, location);
    }
    // todo: error
    return MaValue::Empty();
  }

  MaValue MaValue::doShr(MaMa* M,
                         const MaValue& value,
                         const MaLocation& location) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(
            static_cast<double>(static_cast<long>(this->asNumber()) >>
                                static_cast<long>(value.asNumber())));
      }
      // todo: error
      return MaValue::Empty();
    } else if (this->isObject()) {
      return this->asObject()->doShr(M, value, location);
    }
    // todo: error
    return MaValue::Empty();
  }
} // namespace mavka::mama