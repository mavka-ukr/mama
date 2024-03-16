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
      case VEGet:
        return "VEGet";
      case VSet:
        return "VSet";
      case VESetR:
        return "VESetR";
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
      default:
        return std::to_string(v);
        break;
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
  MaInstruction MaInstruction::args() {
    return MaInstruction{VArgs, {}};
  }
  MaInstruction MaInstruction::storeArg(const std::string& name) {
    return MaInstruction{VStoreArg,
                         {.storeArg = new MaStoreArgInstructionArgs(name)}};
  }
  MaInstruction MaInstruction::call(size_t li) {
    return MaInstruction{VCall, {}, li};
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
  MaInstruction MaInstruction::eGet(const std::string& name) {
    return MaInstruction{VEGet, {.get = new MaGetInstructionArgs(name)}};
  }
  MaInstruction MaInstruction::set(const std::string& name) {
    return MaInstruction{VSet, {.set = new MaSetInstructionArgs(name)}};
  }
  MaInstruction MaInstruction::try_(MaTryInstructionArgs* args) {
    return MaInstruction{VTry, {.try_ = args}};
  }
  MaInstruction MaInstruction::throw_(size_t li) {
    return MaInstruction{VThrow, {.throw_ = new MaThrowInstructionArgs()}, li};
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
  MaInstruction MaInstruction::take(
      const std::string& repository,
      bool relative,
      const std::vector<std::string>& path_parts) {
    return MaInstruction{
        VTake,
        {.take = new MaTakeInstructionArgs(repository, relative, path_parts)}};
  }
  MaInstruction MaInstruction::eq(size_t li) {
    return MaInstruction{VEq, {}, li};
  }
  MaInstruction MaInstruction::gt(size_t li) {
    return MaInstruction{VGt, {}, li};
  }
  MaInstruction MaInstruction::ge(size_t li) {
    return MaInstruction{VGe, {}, li};
  }
  MaInstruction MaInstruction::lt(size_t li) {
    return MaInstruction{VLt, {}, li};
  }
  MaInstruction MaInstruction::le(size_t li) {
    return MaInstruction{VLe, {}, li};
  }
  MaInstruction MaInstruction::contains(size_t li) {
    return MaInstruction{VContains, {}, li};
  }
  MaInstruction MaInstruction::is(size_t li) {
    return MaInstruction{VIs, {}, li};
  }
  MaInstruction MaInstruction::not_(size_t li) {
    return MaInstruction{VNot, {}, li};
  }
  MaInstruction MaInstruction::negative(size_t li) {
    return MaInstruction{VNegative, {}, li};
  }
  MaInstruction MaInstruction::positive(size_t li) {
    return MaInstruction{VPositive, {}, li};
  }
  MaInstruction MaInstruction::bnot(size_t li) {
    return MaInstruction{VBnot, {}, li};
  }
  MaInstruction MaInstruction::add(size_t li) {
    return MaInstruction{VAdd, {}, li};
  }
  MaInstruction MaInstruction::sub(size_t li) {
    return MaInstruction{VSub, {}, li};
  }
  MaInstruction MaInstruction::mul(size_t li) {
    return MaInstruction{VMul, {}, li};
  }
  MaInstruction MaInstruction::div(size_t li) {
    return MaInstruction{VDiv, {}, li};
  }
  MaInstruction MaInstruction::mod(size_t li) {
    return MaInstruction{VMod, {}, li};
  }
  MaInstruction MaInstruction::divdiv(size_t li) {
    return MaInstruction{VDivDiv, {}, li};
  }
  MaInstruction MaInstruction::pow(size_t li) {
    return MaInstruction{VPow, {}, li};
  }
  MaInstruction MaInstruction::xor_(size_t li) {
    return MaInstruction{VXor, {}, li};
  }
  MaInstruction MaInstruction::bor(size_t li) {
    return MaInstruction{VBor, {}, li};
  }
  MaInstruction MaInstruction::band(size_t li) {
    return MaInstruction{VBand, {}, li};
  }
  MaInstruction MaInstruction::shl(size_t li) {
    return MaInstruction{VShl, {}, li};
  }
  MaInstruction MaInstruction::shr(size_t li) {
    return MaInstruction{VShr, {}, li};
  }
} // namespace mavka::mama