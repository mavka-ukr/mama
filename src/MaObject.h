#ifndef MA_OBJECT_H
#define MA_OBJECT_H

class MaText;
class MaList;
class MaDict;
class MaDiia;
class MaStructure;
class MaNative;
class MaModule;
struct MaCell;

struct MaObject {
  size_t ref_count;
  unsigned char type;
  union {
    void* ptr;
    MaText* text;
    MaList* list;
    MaDict* dict;
    MaDiia* diia;
    MaNative* native;
    MaStructure* structure;
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

  [[always_inline]] inline bool IsText() const {
    return this->type == MA_OBJECT_STRING;
  };
  [[always_inline]] inline bool IsList() const {
    return this->type == MA_OBJECT_LIST;
  };
  [[always_inline]] inline bool IsDict() const {
    return this->type == MA_OBJECT_DICT;
  };
  [[always_inline]] inline bool IsDiia() const {
    return this->type == MA_OBJECT_DIIA;
  };
  [[always_inline]] inline bool IsNative() const {
    return this->type == MA_OBJECT_NATIVE;
  };
  [[always_inline]] inline bool IsStructure() const {
    return this->type == MA_OBJECT_STRUCTURE;
  };
  [[always_inline]] inline bool IsModule() const {
    return this->type == MA_OBJECT_MODULE;
  };
  [[always_inline]] inline MaText* AsText() const { return this->d.text; };
  [[always_inline]] inline MaList* AsList() const { return this->d.list; };
  [[always_inline]] inline MaDict* AsDict() const { return this->d.dict; };
  [[always_inline]] inline MaDiia* AsDiia() const { return this->d.diia; };
  [[always_inline]] inline MaNative* AsNative() const {
    return this->d.native;
  };
  [[always_inline]] inline MaStructure* AsStructure() const {
    return this->d.structure;
  };
  [[always_inline]] inline MaModule* AsModule() const {
    return this->d.module;
  };

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
  MaCell GetPropertyDirect(const std::string& name);
  MaCell GetPropertyDirectOrEmpty(const std::string& name);
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
  bool IsSame(const MaCell& other) const;

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
  [[always_inline]] inline bool IsObjectText() const {
    return this->v.object->IsText();
  };
  [[always_inline]] inline bool IsArgs() const {
    return this->type == MA_CELL_ARGS;
  };
  [[always_inline]] inline bool IsError() const {
    return this->type == MA_CELL_ERROR;
  };
  [[always_inline]] inline double AsNumber() const { return this->v.number; };
  [[always_inline]] inline long AsInteger() const {
    return static_cast<long>(this->v.number);
  };
  [[always_inline]] inline MaObject* AsObject() const {
    return this->v.object;
  };
  [[always_inline]] inline MaArgs* AsArgs() const { return this->v.args; };
  [[always_inline]] inline MaError* AsError() const { return this->v.error; };
  [[always_inline]] inline MaText* AsText() const {
    return this->v.object->AsText();
  };
  [[always_inline]] inline MaList* AsList() const {
    return this->v.object->AsList();
  };
  [[always_inline]] inline MaDict* AsDict() const {
    return this->v.object->AsDict();
  };

  [[always_inline]] inline static MaCell Empty() {
    return MaCell{MA_CELL_EMPTY};
  };
  [[always_inline]] inline static MaCell Number(double value) {
    return MaCell{MA_CELL_NUMBER, {.number = value}};
  };
  [[always_inline]] inline static MaCell Integer(long value) {
    return MaCell{MA_CELL_NUMBER, {.number = static_cast<double>(value)}};
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

  size_t GetLength() const;
  std::string Substr(size_t start, size_t length) const;
};

class MaList final {
 public:
  std::vector<MaCell> data;

  static void Init(MaMa* M);
  static MaObject* Create(MaMa* M);

  void Append(const MaCell& cell);
  void SetAt(size_t index, const MaCell& cell);
  MaCell GetAt(size_t index) const;
  size_t GetSize() const;
  bool Contains(const MaCell& cell);
};

class MaDict final {
 public:
  std::vector<std::pair<MaCell, MaCell>> data;

  static void Init(MaMa* M);
  static MaObject* Create(MaMa* M);

  void Set(const MaCell& key, const MaCell& value);
  MaCell Get(const MaCell& key) const;
  void Remove(const MaCell& key);
  size_t GetSize() const;
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

  inline MaObject* GetMe() const { return this->me; }
  inline std::vector<MaDiiaParam> GetParams() const { return this->params; }

  static void Init(MaMa* M);
  static MaObject* Create(MaMa* M,
                          const std::string& diia_o,
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

typedef MaCell NativeFn(MaMa* M,
                        MaObject* o,
                        MaArgs* args,
                        MaLocation location);

class MaNative final {
 public:
  std::string name;
  std::function<NativeFn> fn;
  MaObject* me;

  inline MaObject* GetMe() const { return this->me; }

  static MaObject* Create(MaMa* M,
                          const std::string& native_o,
                          const std::function<NativeFn>& native_fn,
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
