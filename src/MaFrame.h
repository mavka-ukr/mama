#ifndef MA_FRAME_H
#define MA_FRAME_H

enum MaArgsType : size_t { MA_ARGS_TYPE_NAMED, MA_ARGS_TYPE_POSITIONED };

struct MaArgs {
  MaArgsType type;
  std::unordered_map<std::string, MaValue> named;
  std::vector<MaValue> positioned;

  void set(const std::string& name, const MaValue& value);
  void push(const MaValue& value);
  MaValue get(size_t index,
              const std::string& name,
              const MaValue& default_value);
  MaValue get(size_t index, const std::string& name);
};

struct MaFrame {
  MaScope* scope = nullptr;
  MaObject* object = nullptr;
  MaObject* module = nullptr;
  MaLocation location;
};

#endif // MA_FRAME_H