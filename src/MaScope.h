#ifndef MA_SCOPE_H
#define MA_SCOPE_H

class MaScope final {
 public:
  MaScope* parent;
  std::unordered_map<std::string, MaValue> subjects;

  bool hasSubject(const std::string& name);
  MaValue getSubject(const std::string& name);
  void setSubject(const std::string& name, MaValue value);
  void setSubject(const std::string& name, MaObject* object);
};

#endif // MA_SCOPE_H