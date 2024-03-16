#include "mama.h"

namespace mavka::mama {
  MaValue MaValue::call(MaMa* M, MaObject* args, size_t li) const {
    if (this->isObject()) {
      return this->asObject()->call(M, args, li);
    }
    return MaValue::Error(MaError::Create(
        M, "Неможливо викликати обʼєкт структури \"" + this->getName() + "\".",
        li));
  };

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

  MaValue MaValue::is(MaMa* M, const MaValue& value, size_t li) const {
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

  MaValue MaValue::isGreater(MaMa* M, const MaValue& value, size_t li) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        if (this->asNumber() > value.asNumber()) {
          return MaValue::Yes();
        } else {
          return MaValue::No();
        }
      }
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, li, MAG_GREATER);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_GREATER, *this, li);
  }

  MaValue MaValue::isGreaterOrEqual(MaMa* M,
                                    const MaValue& value,
                                    size_t li) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        if (this->asNumber() >= value.asNumber()) {
          return MaValue::Yes();
        } else {
          return MaValue::No();
        }
      }
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, li,
                                                MAG_GREATER_EQUAL);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_GREATER_EQUAL, *this, li);
  }

  MaValue MaValue::isLesser(MaMa* M, const MaValue& value, size_t li) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        if (this->asNumber() < value.asNumber()) {
          return MaValue::Yes();
        } else {
          return MaValue::No();
        }
      }
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, li, MAG_LESSER);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_LESSER, *this, li);
  }

  MaValue MaValue::isLesserOrEqual(MaMa* M,
                                   const MaValue& value,
                                   size_t li) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        if (this->asNumber() <= value.asNumber()) {
          return MaValue::Yes();
        } else {
          return MaValue::No();
        }
      }
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, li, MAG_LESSER_EQUAL);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_LESSER_EQUAL, *this, li);
  }

  MaValue MaValue::contains(MaMa* M, const MaValue& value, size_t li) const {
    if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, li, MAG_CONTAINS);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_CONTAINS, *this, li);
  }

  MaValue MaValue::doNot(MaMa* M, size_t li) const {
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

  MaValue MaValue::doNegative(MaMa* M, size_t li) const {
    if (this->isNumber()) {
      return MaValue::Number(-this->asNumber());
    } else if (this->isObject()) {
      return this->asObject()->callMagWithoutValue(M, li, MAG_NEGATIVE);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_NEGATIVE, *this, li);
  }

  MaValue MaValue::doPositive(MaMa* M, size_t li) const {
    if (this->isNumber()) {
      return MaValue::Number(+this->asNumber());
    } else if (this->isObject()) {
      return this->asObject()->callMagWithoutValue(M, li, MAG_POSITIVE);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_POSITIVE, *this, li);
  }

  MaValue MaValue::doBNot(MaMa* M, size_t li) const {
    if (this->isNumber()) {
      return MaValue::Number(
          static_cast<double>(~static_cast<long>(this->asNumber())));
    } else if (this->isObject()) {
      return this->asObject()->callMagWithoutValue(M, li, MAG_BW_NOT);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_BW_NOT, *this, li);
  }

  MaValue MaValue::doAdd(MaMa* M, const MaValue& value, size_t li) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(this->asNumber() + value.asNumber());
      }
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_ADD, *this, li);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, li, MAG_ADD);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_ADD, *this, li);
  }

  MaValue MaValue::doSub(MaMa* M, const MaValue& value, size_t li) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(this->asNumber() - value.asNumber());
      }
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_SUB, *this, li);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, li, MAG_SUB);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_SUB, *this, li);
  }

  MaValue MaValue::doMul(MaMa* M, const MaValue& value, size_t li) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(this->asNumber() * value.asNumber());
      }
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_MUL, *this, li);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, li, MAG_MUL);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_MUL, *this, li);
  }

  MaValue MaValue::doDiv(MaMa* M, const MaValue& value, size_t li) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(this->asNumber() / value.asNumber());
      }
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_DIV, *this, li);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, li, MAG_DIV);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_DIV, *this, li);
  }

  MaValue MaValue::doMod(MaMa* M, const MaValue& value, size_t li) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(fmod(this->asNumber(), value.asNumber()));
      }
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_MOD, *this, li);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, li, MAG_MOD);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_MOD, *this, li);
  }

  MaValue MaValue::doDivDiv(MaMa* M, const MaValue& value, size_t li) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(floor(this->asNumber() / value.asNumber()));
      }
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_DIVDIV, *this, li);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, li, MAG_DIVDIV);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_DIVDIV, *this, li);
  }

  MaValue MaValue::doPow(MaMa* M, const MaValue& value, size_t li) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(pow(this->asNumber(), value.asNumber()));
      }
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_POW, *this, li);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, li, MAG_POW);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_POW, *this, li);
  }

  MaValue MaValue::doXor(MaMa* M, const MaValue& value, size_t li) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(
            static_cast<double>(static_cast<long>(this->asNumber()) ^
                                static_cast<long>(value.asNumber())));
      }
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_BW_XOR, *this, li);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, li, MAG_BW_XOR);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_BW_XOR, *this, li);
  }

  MaValue MaValue::doBor(MaMa* M, const MaValue& value, size_t li) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(
            static_cast<double>(static_cast<long>(this->asNumber()) |
                                static_cast<long>(value.asNumber())));
      }
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_BW_OR, *this, li);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, li, MAG_BW_OR);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_BW_OR, *this, li);
  }

  MaValue MaValue::doBand(MaMa* M, const MaValue& value, size_t li) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(
            static_cast<double>(static_cast<long>(this->asNumber()) &
                                static_cast<long>(value.asNumber())));
      }
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_BW_AND, *this, li);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, li, MAG_BW_AND);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_BW_AND, *this, li);
  }

  MaValue MaValue::doShl(MaMa* M, const MaValue& value, size_t li) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(
            static_cast<double>(static_cast<long>(this->asNumber())
                                << static_cast<long>(value.asNumber())));
      }
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_BW_SHIFT_LEFT, *this,
                                                    li);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, li,
                                                MAG_BW_SHIFT_LEFT);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_BW_SHIFT_LEFT, *this, li);
  }

  MaValue MaValue::doShr(MaMa* M, const MaValue& value, size_t li) const {
    if (this->isNumber()) {
      if (value.isNumber()) {
        return MaValue::Number(
            static_cast<double>(static_cast<long>(this->asNumber()) >>
                                static_cast<long>(value.asNumber())));
      }
      return MaValue::ErrorExpectedNumberFirstParam(M, MAG_BW_SHIFT_RIGHT,
                                                    *this, li);
    } else if (this->isObject()) {
      return this->asObject()->callMagWithValue(M, value, li,
                                                MAG_BW_SHIFT_RIGHT);
    }
    return MaValue::ErrorDiiaNotDefinedFor(M, MAG_BW_SHIFT_RIGHT, *this, li);
  }

  MaValue MaValue::ErrorDiiaNotDefinedFor(MaMa* M,
                                          const std::string& name,
                                          const MaValue& value,
                                          size_t li) {
    return MaValue::Error(MaError::Create(
        M, "Дію \"" + value.getName() + "." + name + "\" не втілено.", li));
  }

  MaValue MaValue::ErrorExpectedNumberFirstParam(MaMa* M,
                                                 const std::string& name,
                                                 const MaValue& value,
                                                 size_t li) {
    return MaValue::Error(
        MaError::Create(M,
                        "Дія \"" + value.getName() + "." + name +
                            "\" очікує першим параметром число.",
                        li));
  }
} // namespace mavka::mama