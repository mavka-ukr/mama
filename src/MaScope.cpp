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

  MaValue MaScope::GetSubject(const std::string& name) {
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
    return MaValue::Empty();
  }

  MaValue MaScope::GetLocalSubject(const std::string& name) {
    if (this->HasLocalSubject(name)) {
      return this->subjects.at(name);
    }
    return MaValue::Empty();
  }

  void MaScope::SetSubject(const std::string& name, MaValue value) {
    this->subjects.insert_or_assign(name, value);
  }

  void MaScope::SetSubject(const std::string& name, MaObject* object) {
    this->subjects.insert_or_assign(name, MaValue::Object(object));
  }

  void MaScope::DeleteSubject(const std::string& name) {
    this->subjects.erase(name);
  }
} // namespace mavka::mama