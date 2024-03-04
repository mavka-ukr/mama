#include "mama.h"

namespace mavka::mama {
  bool MaScope::HasSubject(const std::string& name) {
    if (this->subjects.contains(name)) {
      return true;
    }
    auto parent_temp = this->parent;
    while (parent_temp) {
      if (parent_temp->subjects.contains(name)) {
        return true;
      }
      parent_temp = parent_temp->parent;
    }
    return false;
  }

  bool MaScope::HasLocalSubject(const std::string& name) {
    return this->subjects.contains(name);
  }

  MaCell MaScope::GetSubject(const std::string& name) {
    if (this->subjects.contains(name)) {
      return this->subjects.at(name);
    }
    auto parent = this->parent;
    while (parent) {
      if (parent->subjects.contains(name)) {
        return parent->subjects[name];
      }
      parent = parent->parent;
    }
    return MA_MAKE_EMPTY();
  }

  MaCell MaScope::GetLocalSubject(const std::string& name) {
    if (this->HasLocalSubject(name)) {
      return this->subjects.at(name);
    }
    return MA_MAKE_EMPTY();
  }

  void MaScope::SetSubject(const std::string& name, MaCell value) {
    this->subjects.insert_or_assign(name, value);
  }

  void MaScope::DeleteSubject(const std::string& name) {
    this->subjects.erase(name);
  }
} // namespace mavka::mama