#ifndef MA_INSTRUCTION_H
#define MA_INSTRUCTION_H

typedef enum {
  VPop,
  VRetain,
  VRelease,

  VConstant,
  VNumber,
  VEmpty,
  VYes,
  VNo,

  VArgs,
  VStoreArg,
  VCall,
  VReturn,
  VDiia,
  VDiiaParam,

  VStore,
  VLoad,

  VJump,
  VJumpIfFalse,
  VEJumpIfTrue,
  VEJumpIfFalse,

  VGet,
  VEGet,
  VSet,
  VESetR,
  VEFetchAll,

  VTry,
  VThrow,

  VList,
  VListAppend,
  VDict,
  VDictSet,

  VStruct,
  VStructParam,
  VStructMethod,

  VModule,
  VGive,
  VTake,

  VEq,
  VGt,
  VGe,
  VLt,
  VLe,
  VContains,
  VIs,

  VNot,

  VNegative,
  VPositive,
  VBnot,

  VAdd,
  VSub,
  VMul,
  VDiv,
  VMod,
  VDivDiv,
  VPow,

  VXor,
  VBor,
  VBand,
  VShl,
  VShr,
} MaV;

struct MaStoreInstructionArgs {
  std::string name;
};

struct MaGetInstructionArgs {
  std::string name;
};

struct MaSetInstructionArgs {
  std::string name;
};

struct MaDictSetInstructionArgs {
  std::string key;
};

struct MaLoadInstructionArgs {
  std::string name;
};

struct MaDiiaInstructionArgs {
  MaCode* code;
  std::string name;
};

struct MaDiiaParamInstructionArgs {
  std::string name;
};

struct MaTryInstructionArgs {
  MaCode* try_code;
  MaCode* catch_code;
};

struct MaTryDoneInstructionArgs {
  size_t index;
};

struct MaThrowInstructionArgs {};

struct MaStructInstructionArgs {
  std::string name;
};

struct MaStructParamInstructionArgs {
  std::string name;
};

struct MaStoreArgInstructionArgs {
  std::string name;
};

struct MaModuleInstructionArgs {
  MaCode* code;
  std::string name;
};

struct MaGiveInstructionArgs {
  std::string name;
};

struct MaTakeInstructionArgs {
  std::string repository;
  std::vector<std::string> path_parts;
};

struct MaInstruction {
  MaV v;
  union {
    size_t constant;
    double number;
    MaStoreArgInstructionArgs* storeArg;
    MaDiiaInstructionArgs* diia;
    MaDiiaParamInstructionArgs* diiaParam;
    MaStoreInstructionArgs* store;
    MaLoadInstructionArgs* load;
    size_t jump;
    size_t jumpIfTrue;
    size_t jumpIfFalse;
    MaGetInstructionArgs* get;
    MaSetInstructionArgs* set;
    MaTryInstructionArgs* try_;
    MaTryDoneInstructionArgs* tryDone;
    MaThrowInstructionArgs* throw_;
    MaDictSetInstructionArgs* dictSet;
    MaStructInstructionArgs* struct_;
    MaStructParamInstructionArgs* structParam;
    MaModuleInstructionArgs* module;
    MaGiveInstructionArgs* give;
    MaTakeInstructionArgs* take;
  } data;
  size_t li;

  std::string ToString();

  static MaInstruction pop();
  static MaInstruction retain();
  static MaInstruction release();
  static MaInstruction constant(size_t index);
  static MaInstruction number(double value);
  static MaInstruction empty();
  static MaInstruction yes();
  static MaInstruction no();
  static MaInstruction args();
  static MaInstruction storeArg(const std::string& name);
  static MaInstruction call(size_t li);
  static MaInstruction return_();
  static MaInstruction diia(MaCode* code, const std::string& name);
  static MaInstruction diiaParam(const std::string& name);
  static MaInstruction store(const std::string& name);
  static MaInstruction load(const std::string& name);
  static MaInstruction jump(size_t index);
  static MaInstruction jumpIfFalse(size_t index);
  static MaInstruction get(const std::string& name);
  static MaInstruction eGet(const std::string& name);
  static MaInstruction set(const std::string& name);
  static MaInstruction eFetchAll();
  static MaInstruction try_(MaTryInstructionArgs* args);
  static MaInstruction throw_(size_t li);
  static MaInstruction list();
  static MaInstruction listAppend();
  static MaInstruction dict();
  static MaInstruction dictSet(const std::string& key);
  static MaInstruction struct_(const std::string& name);
  static MaInstruction structParam(const std::string& name);
  static MaInstruction structMethod();
  static MaInstruction module(MaCode* code, const std::string& name);
  static MaInstruction give(const std::string& name);
  static MaInstruction take(const std::string& repository,
                            const std::vector<std::string>& path_parts);
  static MaInstruction eq(size_t li);
  static MaInstruction gt(size_t li);
  static MaInstruction ge(size_t li);
  static MaInstruction lt(size_t li);
  static MaInstruction le(size_t li);
  static MaInstruction contains(size_t li);
  static MaInstruction is(size_t li);
  static MaInstruction not_(size_t li);
  static MaInstruction negative(size_t li);
  static MaInstruction positive(size_t li);
  static MaInstruction bnot(size_t li);
  static MaInstruction add(size_t li);
  static MaInstruction sub(size_t li);
  static MaInstruction mul(size_t li);
  static MaInstruction div(size_t li);
  static MaInstruction mod(size_t li);
  static MaInstruction divdiv(size_t li);
  static MaInstruction pow(size_t li);
  static MaInstruction xor_(size_t li);
  static MaInstruction bor(size_t li);
  static MaInstruction band(size_t li);
  static MaInstruction shl(size_t li);
  static MaInstruction shr(size_t li);
};

#endif // MA_INSTRUCTION_H