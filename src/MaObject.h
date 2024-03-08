#ifndef MA_OBJECT_H
#define MA_OBJECT_H

class MaText;
class MaList;
class MaDict;
class MaDiia;
class MaStructure;
class MaNative;
class MaModule;
struct MaValue;

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
  tsl::ordered_map<std::string, MaValue> properties;
  std::function<
      void(MaMa* M, MaObject* o, const std::string& name, MaValue value)>
      set;
  std::function<MaValue(MaMa* M, MaObject* o, const std::string& name)> get;
  std::function<
      MaValue(MaMa* M, MaObject* o, MaArgs* args, MaLocation location)>
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

  bool HasProperty(MaMa* M, const std::string& name);
  void SetProperty(MaMa* M, const std::string& name, const MaValue& value);
  void SetProperty(MaMa* M, const std::string& name, MaObject* value);
  MaValue GetProperty(MaMa* M, const std::string& name);
  MaValue GetPropertyDirect(MaMa* M, const std::string& name);
};

enum MaValueType : uint8_t {
  MaValueTypeEmpty = 0,
  MaValueTypeNumber = 1,
  MaValueTypeYes = 2,
  MaValueTypeNo = 3,
  MaValueTypeObject = 4,
  MaValueTypeArgs = 5,
  MaValueTypeError = 6,
};

struct MaValue {
  MaValueType type;
  union {
    MaObject* object;
    double number;
    MaArgs* args;
    MaError* error;
  } v;

  std::string GetName() const;
  MaValue Call(MaMa* M, MaArgs* args, const MaLocation& location) const;
  MaValue Call(MaMa* M,
               const std::vector<MaValue>& args,
               const MaLocation& location) const;
  MaValue Call(MaMa* M,
               const std::unordered_map<std::string, MaValue>& args,
               const MaLocation& location) const;
  bool IsSame(const MaValue& other) const;

  [[always_inline]] inline bool IsEmpty() const {
    return this->type == MaValueTypeEmpty;
  };
  [[always_inline]] inline bool IsNumber() const {
    return this->type == MaValueTypeNumber;
  };
  [[always_inline]] inline bool IsYes() const {
    return this->type == MaValueTypeYes;
  };
  [[always_inline]] inline bool IsNo() const {
    return this->type == MaValueTypeNo;
  };
  [[always_inline]] inline bool IsObject() const {
    return this->type == MaValueTypeObject;
  };
  [[always_inline]] inline bool IsObjectText() const {
    return this->v.object->IsText();
  };
  [[always_inline]] inline bool IsArgs() const {
    return this->type == MaValueTypeArgs;
  };
  [[always_inline]] inline bool IsError() const {
    return this->type == MaValueTypeError;
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

  [[always_inline]] inline static MaValue Empty() {
    return MaValue{MaValueTypeEmpty};
  };
  [[always_inline]] inline static MaValue Number(double value) {
    return MaValue{MaValueTypeNumber, {.number = value}};
  };
  [[always_inline]] inline static MaValue Integer(long value) {
    return MaValue{MaValueTypeNumber, {.number = static_cast<double>(value)}};
  };
  [[always_inline]] inline static MaValue Yes() {
    return MaValue{MaValueTypeYes};
  };
  [[always_inline]] inline static MaValue No() {
    return MaValue{MaValueTypeNo};
  };
  [[always_inline]] inline static MaValue Object(MaObject* value) {
    return MaValue{MaValueTypeObject, {.object = value}};
  };
  [[always_inline]] inline static MaValue Args(MaArgs* value) {
    return MaValue{MaValueTypeArgs, {.args = value}};
  };
  [[always_inline]] inline static MaValue Error(MaError* value) {
    return MaValue{MaValueTypeError, {.error = value}};
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
  std::vector<MaValue> data;

  static void Init(MaMa* M);
  static MaObject* Create(MaMa* M);

  void Append(const MaValue& cell);
  void SetAt(size_t index, const MaValue& cell);
  MaValue GetAt(size_t index) const;
  size_t GetSize() const;
  bool Contains(const MaValue& cell);
};

class MaDict final {
 public:
  std::vector<std::pair<MaValue, MaValue>> data;

  static void Init(MaMa* M);
  static MaObject* Create(MaMa* M);

  void Set(const MaValue& key, const MaValue& value);
  MaValue Get(const MaValue& key) const;
  void Remove(const MaValue& key);
  size_t GetSize() const;
};

class MaDiiaParam final {
 public:
  std::string name;
  MaValue default_value;
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

typedef MaValue NativeFn(MaMa* M,
                         MaObject* native_o,
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
