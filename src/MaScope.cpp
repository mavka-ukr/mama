#include "mama.h"

namespace mavka::mama {
  bool MaScope::hasSubject(const std::string& name) {
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

  MaValue MaScope::getSubject(const std::string& name) {
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

  void MaScope::setSubject(const std::string& name, MaValue value) {
    if (value.isObject()) {
      value.asObject()->retain();
    }
    auto subject = this->subjects.find(name);
    if (subject != this->subjects.end()) {
      if (subject->second.isObject()) {
        //        subject->second.asObject()->release();
      }
      subject->second = value;
      return;
    } else {
      this->subjects.insert_or_assign(name, value);
    }
  }

  void MaScope::setSubject(const std::string& name, MaObject* object) {
    object->retain();
    auto subject = this->subjects.find(name);
    if (subject != this->subjects.end()) {
      if (subject->second.isObject()) {
        //        subject->second.asObject()->release();
      }
      subject->second = MaValue::Object(object);
      return;
    } else {
      this->subjects.insert_or_assign(name, MaValue::Object(object));
    }
  }
} // namespace mavka::mama