#ifndef MA_OBJECT_H
#define MA_OBJECT_H

class MaText;
class MaList;
class MaDict;
class MaDiia;
class MaStructure;
class MaModule;
class MaBytes;
struct MaValue;

struct MaObject {
  int ref_count;
  MaObject* type;
  union {
    void* ptr;
    MaObject* outer; // scope
    MaStructure* structure;
    MaDiia* diia;
    MaModule* module;
    MaBytes* bytes;
    MaText* text;
    MaList* list;
    MaDict* dict;
  } d;
  tsl::ordered_map<std::string, MaValue> properties;

  ~MaObject();

  bool isScope(MaMa* M) const;
  bool isStructure(MaMa* M) const;
  bool isDiia(MaMa* M) const;
  bool isModule(MaMa* M) const;
  bool isBytes(MaMa* M) const;
  bool isText(MaMa* M) const;
  bool isList(MaMa* M) const;
  bool isDict(MaMa* M) const;

  MaStructure* asStructure() const;
  MaDiia* asDiia() const;
  MaModule* asModule() const;
  MaBytes* asBytes() const;
  MaText* asText() const;
  MaList* asList() const;
  MaDict* asDict() const;

  void retain();
  void release();

  MaObject* getStructure() const;
  bool is(MaMa* M, MaObject* object) const;
  MaValue callMagWithValue(MaMa* M,
                           const MaValue& value,
                           size_t li,
                           const std::string& name);
  MaValue callMagWithoutValue(MaMa* M, size_t li, const std::string& name);

  void setProperty(MaMa* M, const std::string& name, const MaValue& value);
  void setProperty(MaMa* M, const std::string& name, MaObject* value);
  bool hasProperty(MaMa* M, const std::string& name);
  MaValue getProperty(MaMa* M, const std::string& name);
  MaValue getArg(MaMa* M, const std::string& index, const std::string& name);
  MaValue getArg(MaMa* M,
                 const std::string& index,
                 const std::string& name,
                 const MaValue& defaultValue);

  MaValue call(MaMa* M, MaObject* args, size_t li);

  std::string getPrettyString(MaMa* M);

  static void Init(MaMa* M);
  static MaObject* Instance(MaMa* M, MaObject* structure_object, void* d);
  static MaObject* Empty(MaMa* M);
};

enum MaValueType : uint8_t {
  MaValueTypeError = 0,
  MaValueTypeEmpty = 1,
  MaValueTypeNumber = 2,
  MaValueTypeYes = 3,
  MaValueTypeNo = 4,
  MaValueTypeObject = 5,
};

// You must never store MaValue in the heap! It's a value type.
struct MaValue {
  MaValueType type;
  union {
    void* ptr;
    MaObject* object;
    double number;
    MaObject* args;
    MaError* error;
  } v;

  std::string getName() const;

  MaValue call(MaMa* M, MaObject* args, size_t li) const;
  bool isEqual(MaMa* M, const MaValue& other) const;
  MaValue is(MaMa* M, const MaValue& value, size_t li) const;
  MaValue isGreater(MaMa* M, const MaValue& value, size_t li) const;
  MaValue isGreaterOrEqual(MaMa* M, const MaValue& value, size_t li) const;
  MaValue isLesser(MaMa* M, const MaValue& value, size_t li) const;
  MaValue isLesserOrEqual(MaMa* M, const MaValue& value, size_t li) const;
  MaValue contains(MaMa* M, const MaValue& value, size_t li) const;
  MaValue doNot(MaMa* M, size_t li) const;
  MaValue doNegative(MaMa* M, size_t li) const;
  MaValue doPositive(MaMa* M, size_t li) const;
  MaValue doBNot(MaMa* M, size_t li) const;
  MaValue doAdd(MaMa* M, const MaValue& value, size_t li) const;
  MaValue doSub(MaMa* M, const MaValue& value, size_t li) const;
  MaValue doMul(MaMa* M, const MaValue& value, size_t li) const;
  MaValue doDiv(MaMa* M, const MaValue& value, size_t li) const;
  MaValue doMod(MaMa* M, const MaValue& value, size_t li) const;
  MaValue doDivDiv(MaMa* M, const MaValue& value, size_t li) const;
  MaValue doPow(MaMa* M, const MaValue& value, size_t li) const;
  MaValue doXor(MaMa* M, const MaValue& value, size_t li) const;
  MaValue doBor(MaMa* M, const MaValue& value, size_t li) const;
  MaValue doBand(MaMa* M, const MaValue& value, size_t li) const;
  MaValue doShl(MaMa* M, const MaValue& value, size_t li) const;
  MaValue doShr(MaMa* M, const MaValue& value, size_t li) const;

  inline bool isEmpty() const { return this->type == MaValueTypeEmpty; };
  inline bool isNumber() const { return this->type == MaValueTypeNumber; };
  inline bool isYes() const { return this->type == MaValueTypeYes; };
  inline bool isNo() const { return this->type == MaValueTypeNo; };
  inline bool isObject() const { return this->type == MaValueTypeObject; };
  inline bool isError() const { return this->type == MaValueTypeError; };

  inline double asNumber() const { return this->v.number; };
  inline long asInteger() const { return static_cast<long>(this->v.number); };
  inline MaObject* asObject() const { return this->v.object; };
  inline MaError* asError() const { return this->v.error; };
  inline MaText* asText() const { return this->v.object->asText(); };
  inline MaList* asList() const { return this->v.object->asList(); };
  inline MaDict* asDict() const { return this->v.object->asDict(); };

  inline static MaValue Empty() { return MaValue{MaValueTypeEmpty}; };
  inline static MaValue Number(double value) {
    return MaValue{MaValueTypeNumber, {.number = value}};
  };
  inline static MaValue Integer(long value) {
    return MaValue{MaValueTypeNumber, {.number = static_cast<double>(value)}};
  };
  inline static MaValue Yes() { return MaValue{MaValueTypeYes}; };
  inline static MaValue No() { return MaValue{MaValueTypeNo}; };
  inline static MaValue Object(MaObject* value) {
    return MaValue{MaValueTypeObject, {.object = value}};
  };
  inline static MaValue Error(MaError* value) {
    return MaValue{MaValueTypeError, {.error = value}};
  };
  static MaValue ErrorDiiaNotDefinedFor(MaMa* M,
                                        const std::string& name,
                                        const MaValue& value,
                                        size_t li);
  static MaValue ErrorExpectedNumberFirstParam(MaMa* M,
                                               const std::string& name,
                                               const MaValue& value,
                                               size_t li);
};

class MaText final {
 public:
  std::string data;

  size_t getLength() const;

  static void Init(MaMa* M);
  static MaObject* Create(MaMa* M, const std::string& value);

  std::string Substr(size_t start, size_t length) const;
};

class MaList final {
 public:
  std::vector<MaValue> data;

  size_t getLength() const;

  static void Init(MaMa* M);
  static MaObject* Create(MaMa* M);

  void append(MaMa* M, const MaValue& cell);
  void setAt(MaMa* M, size_t index, const MaValue& value);
  MaValue getAt(MaMa* M, size_t index) const;
  bool contains(MaMa* M, const MaValue& cell);
};

class MaDict final {
 public:
  std::vector<std::pair<MaValue, MaValue>> data;

  void setAt(MaMa* M, const MaValue& key, const MaValue& value);
  MaValue getAt(MaMa* M, const MaValue& key) const;
  size_t getSize() const;

  static void Init(MaMa* M);
  static MaObject* Create(MaMa* M);
};

class MaDiiaParam final {
 public:
  std::string name;
  MaValue default_value;
};

typedef MaValue NativeFn(MaMa* M,
                         MaObject* diiaObject,
                         MaObject* args,
                         size_t li);

class MaDiia final {
 public:
  std::string name;
  MaCode* code;
  std::function<NativeFn> fn;
  MaObject* me;
  MaObject* outerScope;
  std::vector<MaDiiaParam> params;
  std::unordered_map<std::string, std::string> param_index_map;
  bool is_module_make_diia;

  inline MaObject* getMe() const { return this->me; }
  inline std::vector<MaDiiaParam> getParams() const { return this->params; }
  inline void pushParam(const MaDiiaParam& param) {
    const auto index = std::to_string(this->params.size());
    this->params.push_back(param);
    this->param_index_map[index] = name;
  }
  inline void setOuterScope(MaObject* os) { this->outerScope = os; }

  static void Init(MaMa* M);
  static MaObject* Create(MaMa* M,
                          const std::string& diia_o,
                          MaCode* code,
                          MaObject* me);
  static MaObject* Create(MaMa* M,
                          const std::string& diia_o,
                          const std::function<NativeFn>& fn,
                          MaObject* me);

  MaObject* Bind(MaMa* M, MaObject* object);
};

class MaBytes final {
 public:
  std::vector<uint8_t> data;

  static void Init(MaMa* M);
  static MaObject* Create(MaMa* M, const std::vector<uint8_t>& data);
};

class MaStructure final {
 public:
  std::string name;
  std::vector<MaDiiaParam> params;
  std::vector<MaObject*> methods;

  std::string getName() const;
  inline std::vector<MaDiiaParam> getParams() const { return this->params; }
  inline void pushParam(const MaDiiaParam& param) {
    this->params.push_back(param);
  }
  inline void pushMethod(MaObject* method) { this->methods.push_back(method); }

  static void Init(MaMa* M);
  static void Init2(MaMa* M);
  static MaObject* Create(MaMa* M, const std::string& name);
};

class MaModule final {
 public:
  std::string name;
  MaCode* code;
  MaObject* root;

  inline std::string getName() const { return this->name; };
  inline MaObject* getRoot() const { return this->root; };
  inline MaCode* getCode() const { return this->code; };

  static void Init(MaMa* M);
  static MaObject* Create(MaMa* M, const std::string& name);
};

void InitNumber(MaMa* M);
void InitLogical(MaMa* M);

#endif // MA_OBJECT_H
