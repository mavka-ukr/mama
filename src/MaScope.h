#ifndef MA_SCOPE_H
#define MA_SCOPE_H

class MaScope final {
 public:
  MaScope* parent;
  std::unordered_map<std::string, MaValue> subjects;

  bool HasSubject(const std::string& name);
  bool HasLocalSubject(const std::string& name);
  MaValue GetSubject(const std::string& name);
  MaValue GetLocalSubject(const std::string& name);
  void SetSubject(const std::string& name, MaValue value);
  void SetSubject(const std::string& name, MaObject* object);
  void DeleteSubject(const std::string& name);
};

#endif // MA_SCOPE_H