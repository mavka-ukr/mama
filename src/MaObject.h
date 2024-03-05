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
  size_t ref_count;
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

  void Retain();
  void Release();

  bool HasProperty(const std::string& name);
  void SetProperty(const std::string& name, const MaCell& value);
  void SetProperty(const std::string& name, MaObject* value);
  MaCell GetProperty(const std::string& name);
  MaCell GetPropertyOrEmpty(const std::string& name);
};

struct MaCell {
  unsigned char type;
  union {
    MaObject* object;
    double number;
    MaArgs* args;
    MaError* error;
  } v;

  std::string GetName() const;
  MaCell Call(MaMa* M, MaArgs* args, const MaLocation& location) const;
  MaCell Call(MaMa* M,
              const std::vector<MaCell>& args,
              const MaLocation& location) const;
  MaCell Call(MaMa* M,
              const std::unordered_map<std::string, MaCell>& args,
              const MaLocation& location) const;

  [[always_inline]] inline bool IsEmpty() const {
    return this->type == MA_CELL_EMPTY;
  };
  [[always_inline]] inline bool IsNumber() const {
    return this->type == MA_CELL_NUMBER;
  };
  [[always_inline]] inline bool IsYes() const {
    return this->type == MA_CELL_YES;
  };
  [[always_inline]] inline bool IsNo() const {
    return this->type == MA_CELL_NO;
  };
  [[always_inline]] inline bool IsObject() const {
    return this->type == MA_CELL_OBJECT;
  };
  [[always_inline]] inline bool IsArgs() const {
    return this->type == MA_CELL_ARGS;
  };
  [[always_inline]] inline bool IsError() const {
    return this->type == MA_CELL_ERROR;
  };

  [[always_inline]] inline static MaCell Empty() {
    return MaCell{MA_CELL_EMPTY};
  };
  [[always_inline]] inline static MaCell Number(double value) {
    return MaCell{MA_CELL_NUMBER, {.number = value}};
  };
  [[always_inline]] inline static MaCell Yes() { return MaCell{MA_CELL_YES}; };
  [[always_inline]] inline static MaCell No() { return MaCell{MA_CELL_NO}; };
  [[always_inline]] inline static MaCell Object(MaObject* value) {
    return MaCell{MA_CELL_OBJECT, {.object = value}};
  };
  [[always_inline]] inline static MaCell Args(MaArgs* value) {
    return MaCell{MA_CELL_ARGS, {.args = value}};
  };
  [[always_inline]] inline static MaCell Error(MaError* value) {
    return MaCell{MA_CELL_ERROR, {.error = value}};
  };
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

void InitNumber(MaMa* M);
void InitLogical(MaMa* M);

#endif // MA_OBJECT_H
