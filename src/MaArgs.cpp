#include "mama.h"

namespace mavka::mama {
  void MaArgs::set(const std::string& name, const MaValue& value) {
    this->named.insert_or_assign(name, value);
  }

  void MaArgs::push(const MaValue& value) {
    this->positioned.push_back(value);
  }

  MaValue MaArgs::get(size_t index,
                      const std::string& name,
                      const MaValue& default_value) {
    return this->type == MA_ARGS_TYPE_NAMED
               ? (this->named.contains(name) ? this->named[name]
                                             : default_value)
               : (this->positioned.size() > index ? this->positioned[index]
                                                  : default_value);
  }

  MaValue MaArgs::get(size_t index, const std::string& name) {
    return this->type == MA_ARGS_TYPE_NAMED
               ? (this->named.contains(name) ? this->named[name]
                                             : MaValue::Empty())
               : (this->positioned.size() > index ? this->positioned[index]
                                                  : MaValue::Empty());
  }
} // namespace mavka::mama