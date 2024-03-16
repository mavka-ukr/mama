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
      return this->asObject()->callMagWithValue(M, value, location,
                                                MAG_GREATER);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_GREATER, *this, location);
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
      return this->asObject()->callMagWithValue(M, value, location,
                                                MAG_GREATER_EQUAL);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_GREATER_EQUAL, *this,
                                           location);
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
      return this->asObject()->callMagWithValue(M, value, location, MAG_LESSER);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_LESSER, *this, location);
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
      return this->asObject()->callMagWithValue(M, value, location,
                                                MAG_LESSER_EQUAL);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_LESSER_EQUAL, *this,
                                           location);
  }

  MaValue MaValue::contains(MaMa* M,
                            const MaValue& value,
                            const MaLocation& location) const {
    if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, location,
                                                MAG_CONTAINS);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_CONTAINS, *this, location);
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
      return this->asObject()->callMagWithoutValue(M, location, MAG_NEGATIVE);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_NEGATIVE, *this, location);
  }

  MaValue MaValue::doPositive(MaMa* M, const MaLocation& location) const {
    if (this->isNumber()) {
      return MaValue::Number(+this->asNumber());
    } else if (this->isObject()) {
      return this->asObject()->callMagWithoutValue(M, location, MAG_POSITIVE);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_POSITIVE, *this, location);
  }

  MaValue MaValue::doBNot(MaMa* M, const MaLocation& location) const {
    if (this->isNumber()) {
      return MaValue::Number(
          static_cast<double>(~static_cast<long>(this->asNumber())));
    } else if (this->isObject()) {
      return this->asObject()->callMagWithoutValue(M, location, MAG_BW_NOT);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_BW_NOT, *this, location);
  }

  MaValue MaValue::doAdd(MaMa* M,
                         const MaValue& value,
                         const MaLocation& location) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(this->asNumber() + value.asNumber());
      }
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_ADD, *this,
                                                    location);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, location, MAG_ADD);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_ADD, *this, location);
  }

  MaValue MaValue::doSub(MaMa* M,
                         const MaValue& value,
                         const MaLocation& location) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(this->asNumber() - value.asNumber());
      }
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_SUB, *this,
                                                    location);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, location, MAG_SUB);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_SUB, *this, location);
  }

  MaValue MaValue::doMul(MaMa* M,
                         const MaValue& value,
                         const MaLocation& location) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(this->asNumber() * value.asNumber());
      }
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_MUL, *this,
                                                    location);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, location, MAG_MUL);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_MUL, *this, location);
  }

  MaValue MaValue::doDiv(MaMa* M,
                         const MaValue& value,
                         const MaLocation& location) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(this->asNumber() / value.asNumber());
      }
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_DIV, *this,
                                                    location);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, location, MAG_DIV);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_DIV, *this, location);
  }

  MaValue MaValue::doMod(MaMa* M,
                         const MaValue& value,
                         const MaLocation& location) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(fmod(this->asNumber(), value.asNumber()));
      }
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_MOD, *this,
                                                    location);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, location, MAG_MOD);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_MOD, *this, location);
  }

  MaValue MaValue::doDivDiv(MaMa* M,
                            const MaValue& value,
                            const MaLocation& location) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(floor(this->asNumber() / value.asNumber()));
      }
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_DIVDIV, *this,
                                                    location);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, location, MAG_DIVDIV);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_DIVDIV, *this, location);
  }

  MaValue MaValue::doPow(MaMa* M,
                         const MaValue& value,
                         const MaLocation& location) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(pow(this->asNumber(), value.asNumber()));
      }
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_POW, *this,
                                                    location);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, location, MAG_POW);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_POW, *this, location);
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
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_BW_XOR, *this,
                                                    location);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, location, MAG_BW_XOR);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_BW_XOR, *this, location);
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
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_BW_OR, *this,
                                                    location);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, location, MAG_BW_OR);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_BW_OR, *this, location);
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
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_BW_AND, *this,
                                                    location);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, location, MAG_BW_AND);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_BW_AND, *this, location);
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
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_BW_SHIFT_LEFT, *this,
                                                    location);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, location,
                                                MAG_BW_SHIFT_LEFT);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_BW_SHIFT_LEFT, *this,
                                           location);
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
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_BW_SHIFT_RIGHT,
                                                    *this, location);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, location,
                                                MAG_BW_SHIFT_RIGHT);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_BW_SHIFT_RIGHT, *this,
                                           location);
  }

  MaValue MaValue::ErrorDiiaNotDefinedFor(MaMa* M,
                                          const std::string& name,
                                          const MaValue& value,
                                          const MaLocation& location) {
    return MaValue::Error(MaError::Create(
        M, "Дію \"" + value.getName() + "." + name + "\" не втілено.",
        location));
  }

  MaValue MaValue::ErrorExpectedNumberFirstParam(MaMa* M,
                                                 const std::string& name,
                                                 const MaValue& value,
                                                 const MaLocation& location) {
    return MaValue::Error(
        MaError::Create(M,
                        "Дія \"" + value.getName() + "." + name +
                            "\" очікує першим параметром число.",
                        location));
  }
} // namespace mavka::mama