#ifndef MA_SCOPE_H
#define MA_SCOPE_H

class MaScope final {
 public:
  MaScope* parent;
  std::map<std::string, MaCell> subjects;

  bool HasSubject(const std::string& name);
  bool HasLocalSubject(const std::string& name);
  MaCell GetSubject(const std::string& name);
  MaCell GetLocalSubject(const std::string& name);
  void SetSubject(const std::string& name, MaCell value);
  void DeleteSubject(const std::string& name);
};

#endif // MA_SCOPE_H