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
      return this->subjects[name];
    }
    auto parent_tmp = this->parent;
    while (parent_tmp) {
      if (parent_tmp->subjects.contains(name)) {
        return parent_tmp->subjects[name];
      }
      parent_tmp = parent_tmp->parent;
    }
    return MaValue::Empty();
  }

  MaValue MaScope::GetLocalSubject(const std::string& name) {
    if (this->HasLocalSubject(name)) {
      return this->subjects[name];
    }
    return MaValue::Empty();
  }

  void MaScope::SetSubject(const std::string& name, MaValue value) {
    value.Retain();
    auto subject = this->subjects.find(name);
    if (subject != this->subjects.end()) {
      subject->second.Release();
      subject->second = value;
      return;
    } else {
      this->subjects.insert_or_assign(name, value);
    }
  }

  void MaScope::SetSubject(const std::string& name, MaObject* object) {
    object->Retain();
    auto subject = this->subjects.find(name);
    if (subject != this->subjects.end()) {
      subject->second.Release();
      subject->second = MaValue::Object(object);
      return;
    } else {
      this->subjects.insert_or_assign(name, MaValue::Object(object));
    }
  }

  void MaScope::DeleteSubject(const std::string& name) {
    auto subject = this->subjects.find(name);
    if (subject != this->subjects.end()) {
      subject->second.Release();
    }
    this->subjects.erase(name);
  }
} // namespace mavka::mama