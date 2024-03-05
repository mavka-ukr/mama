#include "mama.h"

namespace mavka::mama {
  std::string MaInstruction::ToString() {
    switch (this->v) {
      case VConstant:
        return "VConstant";
      case VAdd:
        return "VAdd";
      case VSub:
        return "VSub";
      case VMul:
        return "VMul";
      case VDiv:
        return "VDiv";
      case VMod:
        return "VMod";
      case VDivDiv:
        return "VDivDiv";
      case VPow:
        return "VPow";
      case VStore:
        return "VStore";
      case VLoad:
        return "VLoad " + this->data.load->name;
      case VGet:
        return "VGet";
      case VSet:
        return "VSet";
      case VXor:
        return "VXor";
      case VBor:
        return "VBor";
      case VBand:
        return "VBand";
      case VShl:
        return "VShl";
      case VShr:
        return "VShr";
      case VBnot:
        return "VBnot";
      case VJumpIfFalse:
        return "VJumpIfFalse";
      case VJumpIfTrue:
        return "VJumpIfTrue";
      case VEJumpIfFalse:
        return "VEJumpIfFalse";
      case VEJumpIfTrue:
        return "VEJumpIfTrue";
      case VJump:
        return "VJump";
      case VThrow:
        return "VThrow";
      case VEq:
        return "VEq";
      case VLt:
        return "VLt";
      case VLe:
        return "VLe";
      case VContains:
        return "VContains";
      case VGt:
        return "VGt";
      case VGe:
        return "VGe";
      case VCall:
        return "VCall";
      case VReturn:
        return "VReturn";
      case VPop:
        return "VPop";
      case VList:
        return "VList";
      case VListAppend:
        return "VListAppend";
      case VNegative:
        return "VNegative";
      case VPositive:
        return "VPositive";
      case VDiia:
        return "VDiia";
      case VDict:
        return "VDict";
      case VDictSet:
        return "VDictSet";
      case VStruct:
        return "VStruct";
      case VGive:
        return "VGive";
      case VModule:
        return "VModule";
      case VTry:
        return "VTry";
      case VTryDone:
        return "VTryDone";
      case VNot:
        return "VNot";
      case VArgs:
        return "VArgs";
      case VDiiaParam:
        return "VDiiaParam";
      case VStructParam:
        return "VStructParam";
      case VStructMethod:
        return "VStructMethod";
      case VStoreArg:
        return "VStoreArg";
      case VNumber:
        return "VNumber";
      case VEmpty:
        return "VEmpty";
      case VYes:
        return "VYes";
      case VNo:
        return "VNo";
      case VIs:
        return "VIs";
      case VTake:
        return "VTake";
      case VModuleLoad:
        return "VModuleLoad";
      case VPushArg:
        return "VPushArg";
      default:
        return std::to_string(v);
    }
  }
  MaInstruction MaInstruction::pop() {
    return MaInstruction{VPop};
  }
  MaInstruction MaInstruction::constant(size_t index) {
    return MaInstruction{VConstant, {.constant = index}};
  }
  MaInstruction MaInstruction::number(double value) {
    return MaInstruction{VNumber, {.number = value}};
  }
  MaInstruction MaInstruction::empty() {
    return MaInstruction{VEmpty};
  }
  MaInstruction MaInstruction::yes() {
    return MaInstruction{VYes};
  }
  MaInstruction MaInstruction::no() {
    return MaInstruction{VNo};
  }
  MaInstruction MaInstruction::args(MaArgsType args_type) {
    return MaInstruction{VArgs, {.args_type = args_type}};
  }
  MaInstruction MaInstruction::pushArg() {
    return MaInstruction{VPushArg};
  }
  MaInstruction MaInstruction::storeArg(const std::string& name) {
    return MaInstruction{VStoreArg,
                         {.storeArg = new MaStoreArgInstructionArgs(name)}};
  }
  MaInstruction MaInstruction::call(MaLocation location) {
    return MaInstruction{VCall, {}, location};
  }
  MaInstruction MaInstruction::return_() {
    return MaInstruction{VReturn};
  }
  MaInstruction MaInstruction::diia(MaCode* code, const std::string& name) {
    return MaInstruction{VDiia,
                         {.diia = new MaDiiaInstructionArgs(code, name)}};
  }
  MaInstruction MaInstruction::diiaParam(const std::string& name) {
    return MaInstruction{VDiiaParam,
                         {.diiaParam = new MaDiiaParamInstructionArgs(name)}};
  }
  MaInstruction MaInstruction::store(const std::string& name) {
    return MaInstruction{VStore, {.store = new MaStoreInstructionArgs(name)}};
  }
  MaInstruction MaInstruction::load(const std::string& name) {
    return MaInstruction{VLoad, {.load = new MaLoadInstructionArgs(name)}};
  }
  MaInstruction MaInstruction::jump(size_t index) {
    return MaInstruction{VJump, {.jump = index}};
  }
  MaInstruction MaInstruction::jumpIfTrue(size_t index) {
    return MaInstruction{VJumpIfTrue, {.jumpIfTrue = index}};
  }
  MaInstruction MaInstruction::jumpIfFalse(size_t index) {
    return MaInstruction{VJumpIfFalse, {.jumpIfFalse = index}};
  }
  MaInstruction MaInstruction::get(const std::string& name) {
    return MaInstruction{VGet, {.get = new MaGetInstructionArgs(name)}};
  }
  MaInstruction MaInstruction::set(const std::string& name) {
    return MaInstruction{VSet, {.set = new MaSetInstructionArgs(name)}};
  }
  MaInstruction MaInstruction::try_(MaTryInstructionArgs* args) {
    return MaInstruction{VTry, {.try_ = args}};
  }
  MaInstruction MaInstruction::throw_(MaLocation location) {
    return MaInstruction{VThrow,
                         {.throw_ = new MaThrowInstructionArgs(location)}};
  }
  MaInstruction MaInstruction::list() {
    return MaInstruction{VList};
  }
  MaInstruction MaInstruction::listAppend() {
    return MaInstruction{VListAppend};
  }
  MaInstruction MaInstruction::dict() {
    return MaInstruction{VDict};
  }
  MaInstruction MaInstruction::dictSet(const std::string& key) {
    return MaInstruction{VDictSet,
                         {.dictSet = new MaDictSetInstructionArgs(key)}};
  }
  MaInstruction MaInstruction::struct_(const std::string& name) {
    return MaInstruction{VStruct,
                         {.struct_ = new MaStructInstructionArgs(name)}};
  }
  MaInstruction MaInstruction::structParam(const std::string& name) {
    return MaInstruction{
        VStructParam, {.structParam = new MaStructParamInstructionArgs(name)}};
  }
  MaInstruction MaInstruction::structMethod() {
    return MaInstruction{VStructMethod};
  }
  MaInstruction MaInstruction::module(MaCode* code, const std::string& name) {
    return MaInstruction{VModule,
                         {.module = new MaModuleInstructionArgs(code, name)}};
  }
  MaInstruction MaInstruction::give(const std::string& name) {
    return MaInstruction{VGive, {.give = new MaGiveInstructionArgs(name)}};
  }
  MaInstruction MaInstruction::moduleLoad(const std::string& name,
                                          const std::string& as) {
    return MaInstruction{
        VModuleLoad, {.moduleLoad = new MaModuleLoadInstructionArgs(name, as)}};
  }
  MaInstruction MaInstruction::take(
      const std::string& repository,
      bool relative,
      const std::vector<std::string>& path_parts) {
    return MaInstruction{
        VTake,
        {.take = new MaTakeInstructionArgs(repository, relative, path_parts)}};
  }
  MaInstruction MaInstruction::eq() {
    return MaInstruction{VEq};
  }
  MaInstruction MaInstruction::gt() {
    return MaInstruction{VGt};
  }
  MaInstruction MaInstruction::ge() {
    return MaInstruction{VGe};
  }
  MaInstruction MaInstruction::lt() {
    return MaInstruction{VLt};
  }
  MaInstruction MaInstruction::le() {
    return MaInstruction{VLe};
  }
  MaInstruction MaInstruction::contains() {
    return MaInstruction{VContains};
  }
  MaInstruction MaInstruction::is() {
    return MaInstruction{VIs};
  }
  MaInstruction MaInstruction::not_() {
    return MaInstruction{VNot};
  }
  MaInstruction MaInstruction::negative() {
    return MaInstruction{VNegative};
  }
  MaInstruction MaInstruction::positive() {
    return MaInstruction{VPositive};
  }
  MaInstruction MaInstruction::bnot() {
    return MaInstruction{VBnot};
  }
  MaInstruction MaInstruction::add(MaLocation location) {
    return MaInstruction{VAdd, {}, location};
  }
  MaInstruction MaInstruction::sub(MaLocation location) {
    return MaInstruction{VSub, {}, location};
  }
  MaInstruction MaInstruction::mul(MaLocation location) {
    return MaInstruction{VMul, {}, location};
  }
  MaInstruction MaInstruction::div(MaLocation location) {
    return MaInstruction{VDiv, {}, location};
  }
  MaInstruction MaInstruction::mod(MaLocation location) {
    return MaInstruction{VMod, {}, location};
  }
  MaInstruction MaInstruction::divdiv(MaLocation location) {
    return MaInstruction{VDivDiv, {}, location};
  }
  MaInstruction MaInstruction::pow(MaLocation location) {
    return MaInstruction{VPow, {}, location};
  }
  MaInstruction MaInstruction::xor_() {
    return MaInstruction{VXor};
  }
  MaInstruction MaInstruction::bor() {
    return MaInstruction{VBor};
  }
  MaInstruction MaInstruction::band() {
    return MaInstruction{VBand};
  }
  MaInstruction MaInstruction::shl() {
    return MaInstruction{VShl};
  }
  MaInstruction MaInstruction::shr() {
    return MaInstruction{VShr};
  }
} // namespace mavka::mama