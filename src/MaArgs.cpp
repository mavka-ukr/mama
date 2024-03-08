#include "mama.h"

namespace mavka::mama {
  void MaArgs::Set(const std::string& name, const MaValue& value) {
    this->named.insert({name, value});
  }

  void MaArgs::Push(const MaValue& value) {
    this->positioned.push_back(value);
  }

  MaValue MaArgs::Get(size_t index,
                      const std::string& name,
                      const MaValue& default_value) {
    return this->type == MA_ARGS_TYPE_NAMED
               ? (this->named.contains(name) ? this->named[name]
                                             : default_value)
               : (this->positioned.size() > index ? this->positioned[index]
                                                  : default_value);
  }

  MaValue MaArgs::Get(size_t index, const std::string& name) {
    return this->type == MA_ARGS_TYPE_NAMED
               ? (this->named.contains(name) ? this->named[name]
                                             : MaValue::Empty())
               : (this->positioned.size() > index ? this->positioned[index]
                                                  : MaValue::Empty());
  }
} // namespace mavka::mama