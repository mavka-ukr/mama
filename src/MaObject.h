#ifndef MA_OBJECT_H
#define MA_OBJECT_H

struct MaValue;
struct MaDiiaParam;

typedef MaValue NativeFn(MaMa* M,
                         MaObject* scope,
                         MaObject* diiaObject,
                         MaObject* args,
                         size_t li);

struct MaObject {
  int ref_count;
  MaObject* structure;
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

  void retain();
  void release();

  MaObject* getStructure() const;
  bool is(MaMa* M, MaObject* object) const;
  MaValue callMagWithValue(MaMa* M,
                           MaObject* scope,
                           const MaValue& value,
                           size_t li,
                           const std::string& name);
  MaValue callMagWithoutValue(MaMa* M,
                              MaObject* scope,
                              size_t li,
                              const std::string& name);

  void setProperty(MaMa* M, const std::string& name, const MaValue& value);
  void setProperty(MaMa* M, const std::string& name, MaObject* value);
  bool hasProperty(MaMa* M, const std::string& name);
  MaValue getProperty(MaMa* M, const std::string& name);
  MaValue getArg(MaMa* M, const std::string& index, const std::string& name);
  MaValue getArg(MaMa* M,
                 const std::string& index,
                 const std::string& name,
                 const MaValue& defaultValue);

  MaValue call(MaMa* M,
               mavka::mama::MaObject* scope,
               MaObject* args,
               size_t li);

  std::string getPrettyString(MaMa* M);

  static void Init(MaMa* M);
  static MaObject* Instance(MaMa* M, MaObject* structureObject);
  static MaObject* Empty(MaMa* M);

  // scope
  MaObject* scopeOuter;
  MaObject* scopeModule;
  bool scopeHasOuter() const;
  MaObject* scopeGetOuter() const;
  void scopeSetOuter(MaObject* outer);
  MaObject* scopeGetModule() const;
  void scopeSetModule(MaObject* module);
  static MaObject* CreateScope(MaMa* M, MaObject* outerScope, MaObject* module);

  // text
  std::string textData;
  size_t textGetLength() const;
  std::string textSubstr(size_t start, size_t length) const;
  static MaObject* CreateText(MaMa* M, const std::string& value);

  // list
  std::vector<MaValue> listData;
  size_t listGetLength() const;
  void listAppend(MaMa* M, const MaValue& cell);
  MaValue listGetAt(MaMa* M, long index) const;
  void listSetAt(MaMa* M, long index, const MaValue& value);
  bool listContains(MaMa* M, const MaValue& cell);
  static MaObject* CreateList(MaMa* M);

  // dict
  std::vector<std::pair<MaValue, MaValue>> dictData;
  MaValue dictGetAt(MaMa* M, const MaValue& key) const;
  void dictSetAt(MaMa* M, const MaValue& key, const MaValue& value);
  size_t dictGetSize() const;
  static MaObject* CreateDict(MaMa* M);

  // diia
  std::string diiaName;
  MaCode* diiaCode;
  std::function<NativeFn> diiaNativeFn;
  MaObject* diiaBoundObject;
  MaObject* diiaOuterScope;
  std::vector<MaDiiaParam> diiaParams;
  std::unordered_map<std::string, std::string> diiaParamIndicesMap;
  std::string diiaGetName() const;
  void diiaSetName(const std::string& name);
  MaCode* diiaGetCode() const;
  void diiaSetCode(MaCode* code);
  bool diiaHasNativeFn() const;
  std::function<NativeFn> diiaGetNativeFn() const;
  void diiaSetNativeFn(const std::function<NativeFn>& fn);
  bool diiaHasBoundObject() const;
  MaObject* diiaGetBoundObject() const;
  void diiaSetBoundObject(MaObject* object);
  bool diiaHasOuterScope() const;
  MaObject* diiaGetOuterScope() const;
  void diiaSetOuterScope(MaObject* outerScopeObject);
  std::vector<MaDiiaParam> diiaGetParams() const;
  void diiaSetParams(const std::vector<MaDiiaParam>& diiaParams);
  void diiaPushParam(const MaDiiaParam& diiaParam);
  std::unordered_map<std::string, std::string> diiaGetParamIndicesMap() const;
  void diiaSetParamIndicesMap(
      const std::unordered_map<std::string, std::string>& paramIndicesMap);
  MaObject* diiaBind(MaMa* M, MaObject* diiaObject);
  static MaObject* CreateDiia(MaMa* M,
                              const std::string& diia_o,
                              MaCode* code,
                              MaObject* me);
  static MaObject* CreateDiiaNativeFn(MaMa* M,
                                      const std::string& diia_o,
                                      const std::function<NativeFn>& fn,
                                      MaObject* me);

  // bytes
  std::vector<uint8_t> bytesData;
  static MaObject* CreateBytes(MaMa* M, const std::vector<uint8_t>& data);

  // structure
  std::string structureName;
  std::vector<MaDiiaParam> structureParams;
  std::vector<MaObject*> structureMethods;
  std::string structureGetName() const;
  void structureSetName(const std::string& name);
  std::vector<MaDiiaParam> structureGetParams();
  void structurePushParam(const MaDiiaParam& param);
  std::vector<MaObject*> structureGetMethods();
  void structurePushMethod(MaObject* method);
  static MaObject* CreateStructure(MaMa* M, const std::string& name);

  // module
  std::string moduleName;
  MaCode* moduleCode;
  MaObject* moduleRoot;
  std::string moduleGetName() const;
  void moduleSetName(const std::string& name);
  MaObject* moduleGetRoot() const;
  void moduleSetRoot(MaObject* root);
  MaCode* moduleGetCode() const;
  void moduleSetCode(MaCode* code);
  static MaObject* CreateModule(MaMa* M, const std::string& name);
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

  MaValue call(MaMa* M, MaObject* scope, MaObject* args, size_t li) const;
  bool isEqual(MaMa* M, const MaValue& other) const;
  MaValue is(MaMa* M, const MaValue& value, size_t li) const;
  MaValue isGreater(MaMa* M,
                    MaObject* scope,
                    const MaValue& value,
                    size_t li) const;
  MaValue isGreaterOrEqual(MaMa* M,
                           MaObject* scope,
                           const MaValue& value,
                           size_t li) const;
  MaValue isLesser(MaMa* M,
                   MaObject* scope,
                   const MaValue& value,
                   size_t li) const;
  MaValue isLesserOrEqual(MaMa* M,
                          MaObject* scope,
                          const MaValue& value,
                          size_t li) const;
  MaValue contains(MaMa* M,
                   MaObject* scope,
                   const MaValue& value,
                   size_t li) const;
  MaValue doNot(MaMa* M, MaObject* scope, size_t li) const;
  MaValue doNegative(MaMa* M, MaObject* scope, size_t li) const;
  MaValue doPositive(MaMa* M, MaObject* scope, size_t li) const;
  MaValue doBNot(MaMa* M, MaObject* scope, size_t li) const;
  MaValue doAdd(MaMa* M,
                MaObject* scope,
                const MaValue& value,
                size_t li) const;
  MaValue doSub(MaMa* M,
                MaObject* scope,
                const MaValue& value,
                size_t li) const;
  MaValue doMul(MaMa* M,
                MaObject* scope,
                const MaValue& value,
                size_t li) const;
  MaValue doDiv(MaMa* M,
                MaObject* scope,
                const MaValue& value,
                size_t li) const;
  MaValue doMod(MaMa* M,
                MaObject* scope,
                const MaValue& value,
                size_t li) const;
  MaValue doDivDiv(MaMa* M,
                   MaObject* scope,
                   const MaValue& value,
                   size_t li) const;
  MaValue doPow(MaMa* M,
                MaObject* scope,
                const MaValue& value,
                size_t li) const;
  MaValue doXor(MaMa* M,
                MaObject* scope,
                const MaValue& value,
                size_t li) const;
  MaValue doBor(MaMa* M,
                MaObject* scope,
                const MaValue& value,
                size_t li) const;
  MaValue doBand(MaMa* M,
                 MaObject* scope,
                 const MaValue& value,
                 size_t li) const;
  MaValue doShl(MaMa* M,
                MaObject* scope,
                const MaValue& value,
                size_t li) const;
  MaValue doShr(MaMa* M,
                MaObject* scope,
                const MaValue& value,
                size_t li) const;

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

struct MaDiiaParam {
  std::string name;
  MaValue default_value;
};

void InitText(MaMa* M);
void InitList(MaMa* M);
void InitDict(MaMa* M);
void InitDiia(MaMa* M);
void InitBytes(MaMa* M);
void InitStructure(MaMa* M);
void InitStructure2(MaMa* M);
void InitModule(MaMa* M);
void InitNumber(MaMa* M);
void InitLogical(MaMa* M);

#endif // MA_OBJECT_H
