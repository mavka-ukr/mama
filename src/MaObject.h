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
    MaStructure* structure;
    MaDiia* diia;
    MaModule* module;
    MaBytes* bytes;
    MaText* text;
    MaList* list;
    MaDict* dict;
  } d;
  std::unordered_map<std::string, MaValue> properties;

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
                           const MaLocation& location,
                           const std::string& name);
  MaValue callMagWithoutValue(MaMa* M,
                              const MaLocation& location,
                              const std::string& name);
  MaValue isGreater(MaMa* M, const MaValue& value, const MaLocation& location);
  MaValue isGreaterOrEqual(MaMa* M,
                           const MaValue& value,
                           const MaLocation& location);
  MaValue isLesser(MaMa* M, const MaValue& value, const MaLocation& location);
  MaValue isLesserOrEqual(MaMa* M,
                          const MaValue& value,
                          const MaLocation& location);
  MaValue contains(MaMa* M, const MaValue& value, const MaLocation& location);
  MaValue doNegative(MaMa* M, const MaLocation& location);
  MaValue doPositive(MaMa* M, const MaLocation& location);
  MaValue doBNot(MaMa* M, const MaLocation& location);
  MaValue doAdd(MaMa* M, const MaValue& value, const MaLocation& location);
  MaValue doSub(MaMa* M, const MaValue& value, const MaLocation& location);
  MaValue doMul(MaMa* M, const MaValue& value, const MaLocation& location);
  MaValue doDiv(MaMa* M, const MaValue& value, const MaLocation& location);
  MaValue doMod(MaMa* M, const MaValue& value, const MaLocation& location);
  MaValue doDivDiv(MaMa* M, const MaValue& value, const MaLocation& location);
  MaValue doPow(MaMa* M, const MaValue& value, const MaLocation& location);
  MaValue doXor(MaMa* M, const MaValue& value, const MaLocation& location);
  MaValue doBor(MaMa* M, const MaValue& value, const MaLocation& location);
  MaValue doBand(MaMa* M, const MaValue& value, const MaLocation& location);
  MaValue doShl(MaMa* M, const MaValue& value, const MaLocation& location);
  MaValue doShr(MaMa* M, const MaValue& value, const MaLocation& location);

  void setProperty(MaMa* M, const std::string& name, const MaValue& value);
  void setProperty(MaMa* M, const std::string& name, MaObject* value);
  MaValue getProperty(MaMa* M, const std::string& name);

  MaValue call(MaMa* M, MaArgs* args, const MaLocation& location);

  static void Init(MaMa* M);
  static MaObject* Instance(MaMa* M, MaObject* structure_object, void* d);
  static MaObject* Empty(MaMa* M);
};

enum MaValueType : uint8_t {
  MaValueTypeEmpty = 0,
  MaValueTypeNumber,
  MaValueTypeYes,
  MaValueTypeNo,
  MaValueTypeObject,
  MaValueTypeArgs,
  MaValueTypeError,
};

// You must never store MaValue in the heap! It's a value type.
struct MaValue {
  MaValueType type;
  union {
    MaObject* object;
    double number;
    MaArgs* args;
    MaError* error;
  } v;

  std::string GetName() const;

  MaValue call(MaMa* M, MaArgs* args, const MaLocation& location) const;
  MaValue call(MaMa* M,
               const std::vector<MaValue>& args,
               const MaLocation& location) const;
  bool isEqual(MaMa* M, const MaValue& other) const;
  MaValue is(MaMa* M, const MaValue& value, const MaLocation& location) const;
  MaValue isGreater(MaMa* M,
                    const MaValue& value,
                    const MaLocation& location) const;
  MaValue isGreaterOrEqual(MaMa* M,
                           const MaValue& value,
                           const MaLocation& location) const;
  MaValue isLesser(MaMa* M,
                   const MaValue& value,
                   const MaLocation& location) const;
  MaValue isLesserOrEqual(MaMa* M,
                          const MaValue& value,
                          const MaLocation& location) const;
  MaValue contains(MaMa* M,
                   const MaValue& value,
                   const MaLocation& location) const;
  MaValue doNot(MaMa* M, const MaLocation& location) const;
  MaValue doNegative(MaMa* M, const MaLocation& location) const;
  MaValue doPositive(MaMa* M, const MaLocation& location) const;
  MaValue doBNot(MaMa* M, const MaLocation& location) const;
  MaValue doAdd(MaMa* M,
                const MaValue& value,
                const MaLocation& location) const;
  MaValue doSub(MaMa* M,
                const MaValue& value,
                const MaLocation& location) const;
  MaValue doMul(MaMa* M,
                const MaValue& value,
                const MaLocation& location) const;
  MaValue doDiv(MaMa* M,
                const MaValue& value,
                const MaLocation& location) const;
  MaValue doMod(MaMa* M,
                const MaValue& value,
                const MaLocation& location) const;
  MaValue doDivDiv(MaMa* M,
                   const MaValue& value,
                   const MaLocation& location) const;
  MaValue doPow(MaMa* M,
                const MaValue& value,
                const MaLocation& location) const;
  MaValue doXor(MaMa* M,
                const MaValue& value,
                const MaLocation& location) const;
  MaValue doBor(MaMa* M,
                const MaValue& value,
                const MaLocation& location) const;
  MaValue doBand(MaMa* M,
                 const MaValue& value,
                 const MaLocation& location) const;
  MaValue doShl(MaMa* M,
                const MaValue& value,
                const MaLocation& location) const;
  MaValue doShr(MaMa* M,
                const MaValue& value,
                const MaLocation& location) const;

  inline void retain() const {
    if (this->isObject()) {
      this->asObject()->retain();
    }
  };

  inline bool isEmpty() const { return this->type == MaValueTypeEmpty; };
  inline bool isNumber() const { return this->type == MaValueTypeNumber; };
  inline bool isYes() const { return this->type == MaValueTypeYes; };
  inline bool isNo() const { return this->type == MaValueTypeNo; };
  inline bool isObject() const { return this->type == MaValueTypeObject; };
  inline bool isArgs() const { return this->type == MaValueTypeArgs; };
  inline bool isError() const { return this->type == MaValueTypeError; };

  inline double asNumber() const { return this->v.number; };
  inline long asInteger() const { return static_cast<long>(this->v.number); };
  inline MaObject* asObject() const { return this->v.object; };
  inline MaArgs* asArgs() const { return this->v.args; };
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
  inline static MaValue Args(MaArgs* value) {
    return MaValue{MaValueTypeArgs, {.args = value}};
  };
  inline static MaValue Error(MaError* value) {
    return MaValue{MaValueTypeError, {.error = value}};
  };
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
  void setAt(MaMa* M, size_t index, const MaValue& cell);
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
                         MaObject* native_o,
                         MaArgs* args,
                         MaLocation location);

class MaDiia final {
 public:
  std::string name;
  MaCode* code;
  MaObject* me;
  MaScope* scope;
  MaObject* fm;
  std::vector<MaDiiaParam> params;

  // native
  std::function<NativeFn> fn;

  inline MaObject* getMe() const { return this->me; }
  inline std::vector<MaDiiaParam> getParams() const { return this->params; }

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

  static void Init(MaMa* M);
  static void Init2(MaMa* M);
  static MaObject* Create(MaMa* M, const std::string& name);
};

class MaModule final {
 public:
  std::string name;
  MaCode* code;
  bool is_file_module;
  std::unordered_map<std::string, MaValue> properties;

  MaValue getProperty(MaMa* M, MaObject* object, const std::string& name);

  static void Init(MaMa* M);
  static MaObject* Create(MaMa* M, const std::string& name);
};

void InitNumber(MaMa* M);
void InitLogical(MaMa* M);

#endif // MA_OBJECT_H
