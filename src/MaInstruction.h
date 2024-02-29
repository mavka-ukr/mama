#ifndef MA_INSTRUCTION_H
#define MA_INSTRUCTION_H

typedef enum {
  VPop,

  VConstant,
  VNumber,
  VEmpty,
  VYes,
  VNo,

  VArgs,
  VPushArg,
  VStoreArg,
  VCall,
  VReturn,
  VDiia,
  VDiiaParam,

  VStore,
  VLoad,

  VJump,
  VJumpIfTrue,
  VJumpIfFalse,
  VEJumpIfTrue,
  VEJumpIfFalse,

  VGet,
  VSet,
  VESetR,

  VTry,
  VTryDone,
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
  VModuleDone,
  VKeepModule,
  VLoadModule,
  VModuleLoad,
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

struct MaArgsInstructionArgs {
  MaArgsType args_type;
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

struct MaThrowInstructionArgs {
  MaLocation location;
};

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
  bool relative;
  std::vector<std::string> path_parts;
};

struct MaModuleLoadInstructionArgs {
  std::string name;
  std::string as;
};

struct MaInstruction {
  MaV v;
  union {
    size_t constant;
    double number;
    MaArgsType args_type;
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
    MaModuleLoadInstructionArgs* moduleLoad;
    MaTakeInstructionArgs* take;
  } data;
  MaLocation location;

  std::string to_string();

  static MaInstruction pop();
  static MaInstruction constant(size_t index);
  static MaInstruction number(double value);
  static MaInstruction empty();
  static MaInstruction yes();
  static MaInstruction no();
  static MaInstruction args(MaArgsType args_type);
  static MaInstruction pushArg();
  static MaInstruction storeArg(const std::string& name);
  static MaInstruction call(MaLocation location);
  static MaInstruction return_();
  static MaInstruction diia(MaCode* code, const std::string& name);
  static MaInstruction diiaParam(const std::string& name);
  static MaInstruction store(const std::string& name);
  static MaInstruction load(const std::string& name);
  static MaInstruction jump(size_t index);
  static MaInstruction jumpIfTrue(size_t index);
  static MaInstruction jumpIfFalse(size_t index);
  static MaInstruction get(const std::string& name);
  static MaInstruction set(const std::string& name);
  static MaInstruction try_(MaTryInstructionArgs* args);
  static MaInstruction throw_(MaLocation location);
  static MaInstruction list();
  static MaInstruction listAppend();
  static MaInstruction dict();
  static MaInstruction dictSet(const std::string& key);
  static MaInstruction struct_(const std::string& name);
  static MaInstruction structParam(const std::string& name);
  static MaInstruction structMethod();
  static MaInstruction module(MaCode* code, const std::string& name);
  static MaInstruction give(const std::string& name);
  static MaInstruction moduleDone();
  static MaInstruction keepModule();
  static MaInstruction loadModule();
  static MaInstruction moduleLoad(const std::string& name,
                                  const std::string& as);
  static MaInstruction take(const std::string& repository,
                            bool relative,
                            const std::vector<std::string>& path_parts);
  static MaInstruction eq();
  static MaInstruction gt();
  static MaInstruction ge();
  static MaInstruction lt();
  static MaInstruction le();
  static MaInstruction contains();
  static MaInstruction is();
  static MaInstruction not_();
  static MaInstruction negative();
  static MaInstruction positive();
  static MaInstruction bnot();
  static MaInstruction add(MaLocation location);
  static MaInstruction sub(MaLocation location);
  static MaInstruction mul(MaLocation location);
  static MaInstruction div(MaLocation location);
  static MaInstruction mod(MaLocation location);
  static MaInstruction divdiv(MaLocation location);
  static MaInstruction pow(MaLocation location);
  static MaInstruction xor_();
  static MaInstruction bor();
  static MaInstruction band();
  static MaInstruction shl();
  static MaInstruction shr();
};

#endif // MA_INSTRUCTION_H