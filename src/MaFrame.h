#ifndef MA_FRAME_H
#define MA_FRAME_H

enum MaArgsType : size_t { MA_ARGS_TYPE_NAMED, MA_ARGS_TYPE_POSITIONED };

struct MaArgs {
  MaArgsType type;
  std::unordered_map<std::string, MaCell> named;
  std::vector<MaCell> positioned;
};

#define MA_ARGS_SET(a, name, value) (a).v.args->named.insert({(name), (value)});
#define MA_ARGS_PUSH(a, value) (a).v.args->positioned.push_back((value));
#define MA_ARGS_GET(args, index, name, default_value)                       \
  ((args)->type == MA_ARGS_TYPE_NAMED                                       \
       ? ((args)->named.contains((name)) ? (args)->named[(name)]            \
                                         : (default_value))                 \
       : ((args)->positioned.size() > (index) ? (args)->positioned[(index)] \
                                              : (default_value)))

struct MaFrame {
  MaScope* scope = nullptr;
  MaObject* object = nullptr;
  MaObject* module = nullptr;
  MaLocation location;
  std::stack<MaCell> stack;
};

#endif // MA_FRAME_H