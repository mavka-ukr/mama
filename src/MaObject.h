#ifndef MA_OBJECT_H
#define MA_OBJECT_H

class MaText;
class MaList;
class MaDict;
class MaDiia;
class MaStructure;
class MaDiiaNative;
class MaModule;
struct MaCell;

struct MaObject {
  unsigned char type;
  union {
    void* ptr;
    MaText* string;
    MaList* list;
    MaDict* dict;
    MaDiia* diia;
    MaStructure* structure;
    MaDiiaNative* diia_native;
    MaModule* module;
  } d;
  MaObject* structure;
  tsl::ordered_map<std::string, MaCell> properties;
  std::function<
      void(MaMa* M, MaObject* o, const std::string& name, MaCell value)>
      set;
  std::function<MaCell(MaMa* M, MaObject* o, const std::string& name)> get;
  std::function<MaCell(MaMa* M, MaObject* o, MaArgs* args, MaLocation location)>
      call;

  static void Init(MaMa* M);

  static MaObject* Instance(MaMa* M,
                            unsigned char type,
                            MaObject* structure_object,
                            void* d);
  static MaObject* Empty(MaMa* M);
};

struct MaCell {
  unsigned char type;
  union {
    MaObject* object;
    double number;
    MaArgs* args;
  } v;

  std::string get_name() const;
};

class MaText final {
 public:
  std::string data;

  static void Init(MaMa* M);
  static MaObject* Create(MaMa* M, const std::string& value);

  size_t length() const;
  std::string substr(size_t start, size_t length) const;
};

class MaList final {
 public:
  std::vector<MaCell> data;

  static void Init(MaMa* M);
  static MaObject* Create(MaMa* M);

  void append(MaCell cell);
  void set(size_t index, MaCell cell);
  MaCell get(size_t index) const;
  size_t size() const;
  bool contains(MaCell cell);
};

class MaDict final {
 public:
  std::vector<std::pair<MaCell, MaCell>> data;

  static void Init(MaMa* M);
  static MaObject* Create(MaMa* M);

  void set(MaCell key, MaCell value);
  MaCell get(MaCell key) const;
  void remove(MaCell key);
  size_t size() const;
};

class MaDiiaParam final {
 public:
  std::string name;
  MaCell default_value;
};

class MaDiia final {
 public:
  std::string name;
  MaCode* code;
  MaObject* me;
  MaScope* scope;
  MaObject* fm;
  std::vector<MaDiiaParam> params;

  static void Init(MaMa* M);
  static MaObject* Create(MaMa* M,
                          const std::string& o,
                          MaCode* code,
                          MaObject* me);

  MaObject* Bind(MaMa* M, MaObject* object);
};

class MaStructure final {
 public:
  std::string name;
  std::vector<MaDiiaParam> params;
  std::vector<MaObject*> methods;

  static void Init(MaMa* M);
  static void Init2(MaMa* M);
  static MaObject* Create(MaMa* M, const std::string& name);
};

typedef MaCell DiiaNativeFn(MaMa* M, MaObject* o, MaArgs* args);

class MaDiiaNative final {
 public:
  std::string name;
  std::function<DiiaNativeFn> fn;
  MaObject* me;

  static MaObject* Create(MaMa* M,
                          const std::string& name,
                          const std::function<DiiaNativeFn>& diia_native_fn,
                          MaObject* me);
};

class MaModule final {
 public:
  std::string name;
  MaCode* code;
  bool is_file_module;

  static void Init(MaMa* M);
  static MaObject* Create(MaMa* M, const std::string& name);
};

inline std::string ma_number_to_string(const double number) {
  std::ostringstream stream;
  stream << number;
  return stream.str();
}

inline void ma_object_set(MaObject* object,
                          const std::string& name,
                          MaCell value) {
  object->properties.insert_or_assign(name, value);
}

inline MaCell ma_object_get(const MaObject* object, const std::string& name) {
  if (object->properties.contains(name)) {
    return object->properties.at(name);
  }
  return MA_MAKE_EMPTY();
}

void InitNumber(MaMa* M);
void InitLogical(MaMa* M);

#endif // MA_OBJECT_H
