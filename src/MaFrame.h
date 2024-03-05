#ifndef MA_FRAME_H
#define MA_FRAME_H

enum MaArgsType : size_t { MA_ARGS_TYPE_NAMED, MA_ARGS_TYPE_POSITIONED };

struct MaArgs {
  MaArgsType type;
  std::unordered_map<std::string, MaCell> named;
  std::vector<MaCell> positioned;

  void Set(const std::string& name, MaCell value);
  void Push(MaCell value);
  MaCell Get(size_t index,
             const std::string& name,
             const MaCell& default_value);
  MaCell Get(size_t index, const std::string& name);
};

struct MaFrame {
  MaScope* scope = nullptr;
  MaObject* object = nullptr;
  MaObject* module = nullptr;
  MaLocation location;
  std::stack<MaCell> stack;
};

#endif // MA_FRAME_H