#include "mama.h"

namespace mavka::mama {
  void MaArgs::Set(const std::string& name, MaCell value) {
    this->named.insert({name, value});
  }

  void MaArgs::Push(MaCell value) {
    this->positioned.push_back(value);
  }

  MaCell MaArgs::Get(size_t index,
                     const std::string& name,
                     const MaCell& default_value) {
    return this->type == MA_ARGS_TYPE_NAMED
               ? (this->named.contains(name) ? this->named[name]
                                             : default_value)
               : (this->positioned.size() > index ? this->positioned[index]
                                                  : default_value);
  }

  MaCell MaArgs::Get(size_t index, const std::string& name) {
    return this->type == MA_ARGS_TYPE_NAMED
               ? (this->named.contains(name) ? this->named[name]
                                             : MaCell::Empty())
               : (this->positioned.size() > index ? this->positioned[index]
                                                  : MaCell::Empty());
  }
} // namespace mavka::mama