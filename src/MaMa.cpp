#include "mama.h"

#define PUSH(cell) stack.push(cell)
#define PUSH_EMPTY() PUSH(MaValue::Empty())
#define PUSH_NUMBER(v) PUSH(MaValue::Number((v)))
#define PUSH_YES() PUSH(MaValue::Yes())
#define PUSH_NO() PUSH(MaValue::No())
#define PUSH_OBJECT(v) PUSH(MaValue::Object((v)))

#define TOP() stack.top()
#define TOP_VALUE(name) const auto name = TOP();
#define POP() stack.pop();
#define POP_VALUE(name)    \
  const auto name = TOP(); \
  POP();

namespace mavka::mama {
  MaMa* MaMa::Create() {
    const auto M = new MaMa();
    M->global_scope = new MaScope(nullptr);
    MaStructure::Init(M);
    MaObject::Init(M);
    MaDiia::Init(M);
    MaModule::Init(M);
    InitNumber(M);
    InitLogical(M);
    MaText::Init(M);
    MaList::Init(M);
    MaDict::Init(M);
    MaBytes::Init(M);
    MaStructure::Init2(M);
    return M;
  }

  MaValue MaMa::Run(MaCode* code, std::stack<MaValue>& stack) {
    const auto M = this;
    READ_TOP_FRAME();
    auto size = code->instructions.size();
    size_t i = 0;
    for (;;) {
    start:
      if (i >= size) {
        return stack.empty() ? MaValue::Empty() : stack.top();
      }
      auto I = code->instructions[i];

      DEBUG_DO(print_instruction_with_index(code, i, I))

      switch (I.v) {
        case VPop: {
          POP();
          break;
        }
        case VConstant: {
          PUSH(this->constants[I.data.constant]);
          break;
        }
        case VNumber: {
          PUSH_NUMBER(I.data.number);
          break;
        }
        case VEmpty: {
          PUSH_EMPTY();
          break;
        }
        case VYes: {
          PUSH_YES();
          break;
        }
        case VNo: {
          PUSH_NO();
          break;
        }
        case VArgs: {
          PUSH(MaValue::Args(new MaArgs(I.data.args_type)));
          break;
        }
        case VPushArg: {
          POP_VALUE(value_cell);
          TOP_VALUE(args_cell);
          args_cell.v.args->Push(value_cell);
          break;
        }
        case VStoreArg: {
          POP_VALUE(value_cell);
          TOP_VALUE(args_cell);
          args_cell.v.args->Set(I.data.store->name, value_cell);
          break;
        }
        case VCall: {
          POP_VALUE(args_value);
          POP_VALUE(value);
          const auto result = value.Call(this, args_value.v.args, I.location);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VReturn: {
          return stack.top();
        }
        case VDiia: {
          const auto diia_object = MaDiia::Create(this, I.data.diia->name,
                                                  I.data.diia->code, nullptr);
          diia_object->d.diia->scope = frame->scope;
          diia_object->d.diia->fm = frame->module;
          PUSH_OBJECT(diia_object);
          break;
        }
        case VDiiaParam: {
          POP_VALUE(default_value_cell);
          TOP_VALUE(diia_cell);
          diia_cell.v.object->d.diia->params.push_back(
              MaDiiaParam{.name = I.data.diiaParam->name,
                          .default_value = default_value_cell});
          break;
        }
        case VStore: {
          POP_VALUE(value);
          frame->scope->SetSubject(I.data.store->name, value);
          break;
        }
        case VLoad: {
          const auto scope = frame->scope;
          if (scope->HasSubject(I.data.load->name)) {
            PUSH(scope->GetSubject(I.data.load->name));
            break;
          }
          return MaValue::Error(MaError::Create(
              M, "Субʼєкт \"" + I.data.load->name + "\" не визначено.",
              I.location));
        }
        case VJump: {
          i = I.data.jump;
          goto start;
        }
        case VJumpIfTrue: {
          POP_VALUE(cell);
          if (cell.isNumber() && cell.asNumber() != 0.0) {
            i = I.data.jumpIfTrue;
            goto start;
          }
          if (!cell.isNo()) {
            i = I.data.jumpIfTrue;
            goto start;
          }
          break;
        }
        case VJumpIfFalse: {
          POP_VALUE(cell);
          if (cell.isEmpty()) {
            i = I.data.jumpIfFalse;
            goto start;
          } else if (cell.isNumber()) {
            if (cell.v.number == 0.0) {
              i = I.data.jumpIfFalse;
              goto start;
            }
          } else if (cell.isNo()) {
            i = I.data.jumpIfFalse;
            goto start;
          }
          break;
        }
        case VEJumpIfTrue: {
          TOP_VALUE(cell);
          if (cell.isNumber()) {
            if (cell.v.number != 0.0) {
              i = I.data.jumpIfTrue;
              goto start;
            }
          } else if (!cell.isNo()) {
            i = I.data.jumpIfTrue;
            goto start;
          }
          break;
        }
        case VEJumpIfFalse: {
          TOP_VALUE(cell);
          if (cell.isEmpty()) {
            i = I.data.jumpIfFalse;
            goto start;
          }
          if (cell.isNumber() && cell.v.number == 0.0) {
            i = I.data.jumpIfFalse;
            goto start;
          }
          if (cell.isNo()) {
            i = I.data.jumpIfFalse;
            goto start;
          }
          break;
        }
        case VGet: {
          POP_VALUE(value_v);
          if (value_v.isObject()) {
            PUSH(value_v.asObject()->getProperty(this, I.data.get->name));
            break;
          }
          PUSH_EMPTY();
          break;
        }
        case VEGet: {
          TOP_VALUE(value_v);
          if (value_v.isObject()) {
            PUSH(value_v.asObject()->getProperty(this, I.data.get->name));
            break;
          }
          PUSH_EMPTY();
          break;
        }
        case VSet: {
          POP_VALUE(cell);
          POP_VALUE(value);
          if (cell.isObject()) {
            if (cell.asObject()->isText(M)) {
              break;
            }
            cell.asObject()->setProperty(this, I.data.set->name, value);
            break;
          }
          break;
        }
        case VESetR: {
          POP_VALUE(value);
          TOP_VALUE(cell);
          if (cell.isObject()) {
            cell.asObject()->setProperty(this, I.data.set->name, value);
          }
          break;
        }
        case VTry: {
          const auto frames_size = this->call_stack.size();
          const auto result = this->Run(I.data.try_->try_code, stack);
          if (result.isError()) {
            PUSH(result.asError()->value);
            while (this->call_stack.size() > frames_size) {
              FRAME_POP();
            }
            const auto result2 = this->Run(I.data.try_->catch_code, stack);
            if (result2.isError()) {
              return result2;
            }
          }
          break;
        }
        case VTryDone: {
          FRAME_POP();
          i = I.data.tryDone->index;
          break;
        }
        case VThrow: {
          POP_VALUE(cell);
          return MaValue::Error(new MaError(cell, I.location));
        }
        case VList: {
          PUSH_OBJECT(MaList::Create(this));
          break;
        }
        case VListAppend: {
          POP_VALUE(value);
          TOP_VALUE(list_cell);
          list_cell.asObject()->asList()->append(M, value);
          break;
        }
        case VDict: {
          PUSH_OBJECT(MaDict::Create(this));
          break;
        }
        case VDictSet: {
          POP_VALUE(value);
          TOP_VALUE(dict_cell);
          dict_cell.asDict()->setAt(
              M, MaValue::Object(MaText::Create(this, I.data.dictSet->key)),
              value);
          break;
        }
        case VStruct: {
          const auto structure_object =
              MaStructure::Create(this, I.data.struct_->name);
          PUSH_OBJECT(structure_object);
          break;
        }
        case VStructParam: {
          POP_VALUE(default_value_cell);
          TOP_VALUE(structure_cell);
          structure_cell.v.object->d.structure->params.push_back(
              MaDiiaParam{.name = I.data.diiaParam->name,
                          .default_value = default_value_cell});
          break;
        }
        case VStructMethod: {
          POP_VALUE(diia_cell);
          TOP_VALUE(structure_cell);
          if (structure_cell.isObject()) {
            if (structure_cell.asObject()->isStructure(M)) {
              structure_cell.v.object->d.structure->methods.push_back(
                  diia_cell.v.object);
              break;
            }
          }
          DO_RETURN_STRING_ERROR(
              "Неможливо створити метод для типу " + structure_cell.GetName(),
              I.location)
        }
        case VModule: {
          const auto module_object =
              MaModule::Create(this, I.data.module->name);
          const auto module_scope = new MaScope(frame->scope);
          const auto module_frame =
              new MaFrame(module_scope, module_object, frame->module);
          frame->scope->SetSubject(I.data.module->name, module_object);
          FRAME_PUSH(module_frame);
          const auto result = this->Run(I.data.module->code, stack);
          FRAME_POP();
          if (result.isError()) {
            return result;
          }
          break;
        }
        case VGive: {
          POP_VALUE(value);
          const auto meValue = frame->scope->GetSubject("я");
          if (meValue.isError()) {
            return meValue;
          }
          if (meValue.isObject()) {
            meValue.asObject()->setProperty(this, I.data.give->name, value);
          }
          break;
        }
        case VEq: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (left.isEqual(M, right)) {
            PUSH_YES();
          } else {
            PUSH_NO();
          }
          break;
        }
        case VGt: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (left.isNumber()) {
            if (right.isNumber()) {
              if (left.asNumber() > right.asNumber()) {
                PUSH_YES();
              } else {
                PUSH_NO();
              }
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_GREATER) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.isObject()) {
            left.asObject()->retain();
            const auto diia_cell =
                left.asObject()->getProperty(this, MAG_GREATER);
            if (diia_cell.isObject()) {
              diia_cell.asObject()->retain();
              if (right.isObject()) {
                right.retain();
              }
              const auto result = diia_cell.Call(this, {right}, {});
              diia_cell.asObject()->release();
              left.asObject()->release();
              if (right.isObject()) {
                right.asObject()->release();
              }
              if (result.isError()) {
                return result;
              }
              PUSH(result);
              break;
            }
          }
          DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_GREATER, left,
                                                    I.location);
          break;
        }
        case VGe: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (left.isNumber()) {
            if (right.isNumber()) {
              if (left.asNumber() >= right.asNumber()) {
                PUSH_YES();
              } else {
                PUSH_NO();
              }
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_GREATER_EQUAL) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.isObject()) {
            left.retain();
            const auto diia_cell =
                left.asObject()->getProperty(this, MAG_GREATER_EQUAL);
            diia_cell.retain();
            right.retain();
            const auto result = diia_cell.Call(this, {right}, {});
            if (result.isError()) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_GREATER_EQUAL, left,
                                                      I.location);
          }
          break;
        }
        case VLt: {
          POP_VALUE(right);
          POP_VALUE(left);
          left.retain();
          if (left.isNumber()) {
            if (right.isNumber()) {
              if (left.asNumber() < right.asNumber()) {
                PUSH_YES();
              } else {
                PUSH_NO();
              }
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_LESSER) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.isObject()) {
            const auto diia_cell =
                left.asObject()->getProperty(this, MAG_LESSER);
            diia_cell.retain();
            const auto result = diia_cell.Call(this, {right}, {});
            if (result.isError()) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_LESSER, left,
                                                      I.location);
          }
          break;
        }
        case VLe: {
          POP_VALUE(right);
          POP_VALUE(left);
          left.retain();
          if (left.isNumber()) {
            if (right.isNumber()) {
              if (left.asNumber() <= right.asNumber()) {
                PUSH_YES();
              } else {
                PUSH_NO();
              }
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_LESSER_EQUAL) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.isObject()) {
            const auto diia_cell =
                left.asObject()->getProperty(this, MAG_LESSER_EQUAL);
            diia_cell.retain();
            const auto result = diia_cell.Call(this, {right}, {});
            if (result.isError()) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_LESSER_EQUAL, left,
                                                      I.location);
          }
          break;
        }
        case VContains: {
          POP_VALUE(right);
          POP_VALUE(left);
          left.retain();
          if (left.isObject()) {
            const auto diia_cell =
                left.asObject()->getProperty(this, MAG_CONTAINS);
            diia_cell.retain();
            const auto result = diia_cell.Call(this, {right}, {});
            if (result.isError()) {
              return result;
            }
            PUSH(result);
            ;
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_CONTAINS, left,
                                                      I.location);
          }
          break;
        }
        case VIs: {
          POP_VALUE(right);
          POP_VALUE(left);
          left.retain();
          if (left.isEmpty()) {
            if (right.isEmpty()) {
              PUSH_YES();
            } else {
              PUSH_NO();
            }
            break;
          }
          if (left.isNumber()) {
            if (right.asObject() == this->number_structure_object) {
              PUSH_YES();
            } else {
              PUSH_NO();
            }
            break;
          }
          if (left.isYes() || left.isNo()) {
            if (right.v.object == this->logical_structure_object) {
              PUSH_YES();
            } else {
              PUSH_NO();
            }
            break;
          }
          if (right.isObject() && left.isObject()) {
            if (right.asObject()->is(this, left.asObject())) {
              PUSH_YES();
            } else {
              PUSH_NO();
            }
            break;
          }
          PUSH_NO();
          break;
        }
        case VNot: {
          POP_VALUE(value);
          if (value.isEmpty()) {
            PUSH_YES();
          } else if (value.isNumber()) {
            if (value.v.number == 0.0) {
              PUSH_YES();
            } else {
              PUSH_NO();
            }
          } else if (value.isYes()) {
            PUSH_NO();
          } else if (value.isNo()) {
            PUSH_YES();
          } else {
            PUSH_NO();
          }
          break;
        }
        case VNegative: {
          POP_VALUE(value);
          if (value.isNumber()) {
            PUSH_NUMBER(-value.asNumber());
          } else if (value.isObject()) {
            const auto diia_cell =
                value.asObject()->getProperty(this, MAG_NEGATIVE);
            diia_cell.retain();
            const auto result = diia_cell.Call(this, {}, {});
            if (result.isError()) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_NEGATIVE, value,
                                                      I.location);
          }
          break;
        }
        case VPositive: {
          POP_VALUE(value);
          if (value.isNumber()) {
            PUSH_NUMBER(value.v.number * -1);
          } else if (value.isObject()) {
            const auto diia_cell =
                value.asObject()->getProperty(this, MAG_POSITIVE);
            diia_cell.retain();
            const auto result = diia_cell.Call(this, {}, {});
            if (result.isError()) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_POSITIVE, value,
                                                      I.location);
          }
          break;
        }
        case VBnot: {
          POP_VALUE(value);
          if (value.isNumber()) {
            PUSH_NUMBER(
                static_cast<double>(~static_cast<long>(value.v.number)));
          } else if (value.isObject()) {
            const auto diia_cell =
                value.asObject()->getProperty(this, MAG_BW_NOT);
            diia_cell.retain();
            const auto result = diia_cell.Call(this, {}, {});
            if (result.isError()) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_BW_NOT, value,
                                                      I.location);
          }
          break;
        }
        case VAdd: {
          POP_VALUE(right);
          POP_VALUE(left);
          left.retain();
          if (left.isNumber()) {
            if (right.isNumber()) {
              PUSH_NUMBER(left.asNumber() + right.asNumber());
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_ADD) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.isObject()) {
            const auto diia_cell = left.asObject()->getProperty(this, MAG_ADD);
            diia_cell.retain();
            const auto result = diia_cell.Call(this, {right}, I.location);
            if (result.isError()) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_ADD, left,
                                                      I.location);
          }
          break;
        }
        case VSub: {
          POP_VALUE(right);
          POP_VALUE(left);
          left.retain();
          if (left.isNumber()) {
            if (right.isNumber()) {
              PUSH_NUMBER(left.asNumber() - right.asNumber());
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_SUB) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.isObject()) {
            const auto diia_cell = left.asObject()->getProperty(this, MAG_SUB);
            diia_cell.retain();
            const auto result = diia_cell.Call(this, {right}, I.location);
            if (result.isError()) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_SUB, left,
                                                      I.location);
          }
          break;
        }
        case VMul: {
          POP_VALUE(right);
          POP_VALUE(left);
          left.retain();
          if (left.isNumber()) {
            if (right.isNumber()) {
              PUSH_NUMBER(left.asNumber() * right.asNumber());
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_MUL) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.isObject()) {
            const auto diia_cell = left.asObject()->getProperty(this, MAG_MUL);
            diia_cell.retain();
            const auto result = diia_cell.Call(this, {right}, I.location);
            if (result.isError()) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_MUL, left,
                                                      I.location);
          }
          break;
        }
        case VDiv: {
          POP_VALUE(right);
          POP_VALUE(left);
          left.retain();
          if (left.isNumber()) {
            if (right.isNumber()) {
              PUSH_NUMBER(left.asNumber() / right.asNumber());
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_DIV) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.isObject()) {
            const auto diia_cell = left.asObject()->getProperty(this, MAG_DIV);
            diia_cell.retain();
            const auto result = diia_cell.Call(this, {right}, I.location);
            if (result.isError()) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_DIV, left,
                                                      I.location);
          }
          break;
        }
        case VMod: {
          POP_VALUE(right);
          POP_VALUE(left);
          left.retain();
          if (left.isNumber()) {
            if (right.isNumber()) {
              PUSH_NUMBER(fmod(left.asNumber(), right.asNumber()));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_MOD) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.isObject()) {
            const auto diia_cell = left.asObject()->getProperty(this, MAG_MOD);
            diia_cell.retain();
            const auto result = diia_cell.Call(this, {right}, I.location);
            if (result.isError()) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_MOD, left,
                                                      I.location);
          }
          break;
        }
        case VDivDiv: {
          POP_VALUE(right);
          POP_VALUE(left);
          left.retain();
          if (left.isNumber()) {
            if (right.isNumber()) {
              PUSH_NUMBER(floor(left.asNumber() / right.asNumber()));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_DIVDIV) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.isObject()) {
            const auto diia_cell =
                left.asObject()->getProperty(this, MAG_DIVDIV);
            diia_cell.retain();
            const auto result = diia_cell.Call(this, {right}, I.location);
            if (result.isError()) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_DIVDIV, left,
                                                      I.location);
          }
          break;
        }
        case VPow: {
          POP_VALUE(right);
          POP_VALUE(left);
          left.retain();
          if (left.isNumber()) {
            if (right.isNumber()) {
              PUSH_NUMBER(pow(left.asNumber(), right.asNumber()));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_POW) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.isObject()) {
            const auto diia_cell = left.asObject()->getProperty(this, MAG_POW);
            diia_cell.retain();
            const auto result = diia_cell.Call(this, {right}, I.location);
            if (result.isError()) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_POW, left,
                                                      I.location);
          }
          break;
        }
        case VXor: {
          POP_VALUE(right);
          POP_VALUE(left);
          left.retain();
          if (left.isNumber()) {
            if (right.isNumber()) {
              PUSH_NUMBER(
                  static_cast<double>(static_cast<long>(left.asNumber()) ^
                                      static_cast<long>(right.asNumber())));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_BW_XOR) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.isObject()) {
            const auto diia_cell =
                left.asObject()->getProperty(this, MAG_BW_XOR);
            diia_cell.retain();
            const auto result = diia_cell.Call(this, {right}, {});
            if (result.isError()) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_BW_XOR, left,
                                                      I.location);
          }
          break;
        }
        case VBor: {
          POP_VALUE(right);
          POP_VALUE(left);
          left.retain();
          if (left.isNumber()) {
            if (right.isNumber()) {
              PUSH_NUMBER(
                  static_cast<double>(static_cast<long>(left.asNumber()) |
                                      static_cast<long>(right.asNumber())));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_BW_OR) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.isObject()) {
            const auto diia_cell =
                left.asObject()->getProperty(this, MAG_BW_OR);
            diia_cell.retain();
            const auto result = diia_cell.Call(this, {right}, {});
            if (result.isError()) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_BW_OR, left,
                                                      I.location);
          }
          break;
        }
        case VBand: {
          POP_VALUE(right);
          POP_VALUE(left);
          left.retain();
          if (left.isNumber()) {
            if (right.isNumber()) {
              PUSH_NUMBER(
                  static_cast<double>(static_cast<long>(left.asNumber()) &
                                      static_cast<long>(right.asNumber())));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_BW_AND) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.isObject()) {
            const auto diia_cell =
                left.asObject()->getProperty(this, MAG_BW_AND);
            diia_cell.retain();
            const auto result = diia_cell.Call(this, {right}, {});
            if (result.isError()) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_BW_AND, left,
                                                      I.location);
          }
          break;
        }
        case VShl: {
          POP_VALUE(right);
          POP_VALUE(left);
          left.retain();
          if (left.isNumber()) {
            if (right.isNumber()) {
              PUSH_NUMBER(
                  static_cast<double>(static_cast<long>(left.asNumber())
                                      << static_cast<long>(right.asNumber())));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_BW_SHIFT_LEFT) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.isObject()) {
            const auto diia_cell =
                left.asObject()->getProperty(this, MAG_BW_SHIFT_LEFT);
            diia_cell.retain();
            const auto result = diia_cell.Call(this, {right}, {});
            if (result.isError()) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_BW_SHIFT_LEFT, left,
                                                      I.location);
          }
          break;
        }
        case VShr: {
          POP_VALUE(right);
          POP_VALUE(left);
          left.retain();
          if (left.isNumber()) {
            if (right.isNumber()) {
              PUSH_NUMBER(
                  static_cast<double>(static_cast<long>(left.asNumber()) >>
                                      static_cast<long>(right.asNumber())));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_BW_SHIFT_RIGHT) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.isObject()) {
            const auto diia_cell =
                left.asObject()->getProperty(this, MAG_BW_SHIFT_RIGHT);
            diia_cell.retain();
            const auto result = diia_cell.Call(this, {right}, {});
            if (result.isError()) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_BW_SHIFT_RIGHT, left,
                                                      I.location);
          }
          break;
        }
        case VTake: {
          const auto result =
              this->TakeFn(this, I.data.take->repository, I.data.take->relative,
                           I.data.take->path_parts, I.location);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        default: {
          std::cout << "unsupported instruction " << I.ToString() << std::endl;
          return stack.empty() ? MaValue::Empty() : stack.top();
        }
      }

      i++;
    }

    return stack.empty() ? MaValue::Empty() : stack.top();
  }

  MaValue MaMa::Eval(const std::string& code, const MaLocation& location) {
    const auto M = this;

    const auto parser_result = parser::parse(code, "");
    if (!parser_result.errors.empty()) {
      const auto error = parser_result.errors[0];
      DO_RETURN_STRING_ERROR(error.path + ":" + std::to_string(error.line) +
                                 ":" + std::to_string(error.column) + ": " +
                                 error.message,
                             location);
    }

    const auto eval_code = new MaCode();
    const auto body_compilation_result =
        compile_body(this, eval_code, parser_result.module_node->body);
    if (body_compilation_result.error) {
      DO_RETURN_STRING_ERROR(body_compilation_result.error->message, location);
    }
    std::stack<MaValue> stack;
    const auto result = this->Run(eval_code, stack);
    if (result.isError()) {
      return result;
    }
    return result;
  }

  MaValue MaMa::DoTake(const std::string& path,
                       const std::string& name,
                       const std::string& code,
                       const MaLocation& location) {
    const auto parent_scope = this->call_stack.empty()
                                  ? this->global_scope
                                  : this->call_stack.top()->scope;
    const auto module_scope = new MaScope(parent_scope);
    return DoTakeWithScope(path, name, code, location, module_scope);
  }

  MaValue MaMa::DoTakeWithScope(const std::string& path,
                                const std::string& name,
                                const std::string& code,
                                const MaLocation& location,
                                MaScope* module_scope) {
    const auto M = this;

    const auto parser_result = parser::parse(code, path);
    if (!parser_result.errors.empty()) {
      const auto error = parser_result.errors[0];
      DO_RETURN_STRING_ERROR(error.path + ":" + std::to_string(error.line) +
                                 ":" + std::to_string(error.column) + ": " +
                                 error.message,
                             location);
    }

    const auto module_code = new MaCode();
    module_code->path = path;

    const auto module_object = MaModule::Create(this, name);
    module_object->d.module->code = module_code;
    module_object->d.module->is_file_module = true;
    if (this->main_module == nullptr) {
      this->main_module = module_object;
    }
    this->loaded_file_modules.insert_or_assign(path, module_object);

    module_scope->SetSubject("я", module_object);

    const auto body_compilation_result =
        compile_body(this, module_code, parser_result.module_node->body);
    if (body_compilation_result.error) {
      DO_RETURN_STRING_ERROR(
          path + ":" + std::to_string(body_compilation_result.error->line) +
              ":" + std::to_string(body_compilation_result.error->column) +
              ": " + body_compilation_result.error->message,
          location);
    }

    const auto module_frame =
        new MaFrame(module_scope, module_object, module_object);
    FRAME_PUSH(module_frame);
    std::stack<MaValue> stack;
    const auto result = this->Run(module_code, stack);
    if (result.isError()) {
      return result;
    }
    FRAME_POP();
    return MaValue::Object(module_object);
  }
} // namespace mavka::mama