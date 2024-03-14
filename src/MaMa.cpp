#include "mama.h"

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
          if (result.IsError()) {
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
          if (cell.IsNumber() && cell.AsNumber() != 0.0) {
            i = I.data.jumpIfTrue;
            goto start;
          }
          if (!cell.IsNo()) {
            i = I.data.jumpIfTrue;
            goto start;
          }
          break;
        }
        case VJumpIfFalse: {
          POP_VALUE(cell);
          if (cell.IsEmpty()) {
            i = I.data.jumpIfFalse;
            goto start;
          } else if (cell.IsNumber()) {
            if (cell.v.number == 0.0) {
              i = I.data.jumpIfFalse;
              goto start;
            }
          } else if (cell.IsNo()) {
            i = I.data.jumpIfFalse;
            goto start;
          }
          break;
        }
        case VEJumpIfTrue: {
          TOP_VALUE(cell);
          if (cell.IsNumber()) {
            if (cell.v.number != 0.0) {
              i = I.data.jumpIfTrue;
              goto start;
            }
          } else if (!cell.IsNo()) {
            i = I.data.jumpIfTrue;
            goto start;
          }
          break;
        }
        case VEJumpIfFalse: {
          TOP_VALUE(cell);
          if (cell.IsEmpty()) {
            i = I.data.jumpIfFalse;
            goto start;
          }
          if (cell.IsNumber() && cell.v.number == 0.0) {
            i = I.data.jumpIfFalse;
            goto start;
          }
          if (cell.IsNo()) {
            i = I.data.jumpIfFalse;
            goto start;
          }
          break;
        }
        case VGet: {
          POP_VALUE(value_v);
          if (value_v.IsObject()) {
            PUSH(value_v.AsObject()->GetProperty(this, I.data.get->name));
            break;
          }
          PUSH_EMPTY();
          break;
        }
        case VEGet: {
          TOP_VALUE(value_v);
          if (value_v.IsObject()) {
            PUSH(value_v.AsObject()->GetProperty(this, I.data.get->name));
            break;
          }
          PUSH_EMPTY();
          break;
        }
        case VSet: {
          POP_VALUE(cell);
          POP_VALUE(value);
          if (cell.IsObject()) {
            if (cell.IsObjectText()) {
              break;
            }
            cell.AsObject()->SetProperty(this, I.data.set->name, value);
            break;
          }
          break;
        }
        case VESetR: {
          POP_VALUE(value);
          TOP_VALUE(cell);
          if (cell.IsObject()) {
            cell.AsObject()->SetProperty(this, I.data.set->name, value);
          }
          break;
        }
        case VTry: {
          const auto frames_size = this->frame_stack.size();
          const auto result = this->Run(I.data.try_->try_code, stack);
          if (result.IsError()) {
            PUSH(result.AsError()->value);
            while (this->frame_stack.size() > frames_size) {
              FRAME_POP();
            }
            const auto result2 = this->Run(I.data.try_->catch_code, stack);
            if (result2.IsError()) {
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
          list_cell.AsObject()->AsList()->Append(value);
          break;
        }
        case VDict: {
          PUSH_OBJECT(MaDict::Create(this));
          break;
        }
        case VDictSet: {
          POP_VALUE(value);
          TOP_VALUE(dict_cell);
          dict_cell.AsDict()->Set(
              MaValue::Object(MaText::Create(this, I.data.dictSet->key)),
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
          if (structure_cell.IsObject()) {
            if (structure_cell.IsObjectStructure()) {
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
          if (result.IsError()) {
            return result;
          }
          break;
        }
        case VGive: {
          POP_VALUE(value);
          const auto meValue = frame->scope->GetSubject("я");
          if (meValue.IsError()) {
            return meValue;
          }
          if (meValue.IsObject()) {
            meValue.AsObject()->SetProperty(this, I.data.give->name, value);
          }
          break;
        }
        case VEq: {
          POP_VALUE(right);
          POP_VALUE(left);
          left.Retain();
          if (left.IsSame(right)) {
            PUSH_YES();
          } else {
            PUSH_NO();
          }
          left.Release();
          break;
        }
        case VGt: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (left.IsNumber()) {
            if (right.IsNumber()) {
              if (left.AsNumber() > right.AsNumber()) {
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
          } else if (left.IsObject()) {
            left.Retain();
            const auto diia_cell =
                left.AsObject()->GetProperty(this, MAG_GREATER);
            diia_cell.Retain();
            right.Retain();
            const auto result = diia_cell.Call(this, {right}, {});
            diia_cell.Release();
            left.Release();
            right.Release();
            if (result.IsError()) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_GREATER, left,
                                                      I.location);
          }
          break;
        }
        case VGe: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (left.IsNumber()) {
            if (right.IsNumber()) {
              if (left.AsNumber() >= right.AsNumber()) {
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
          } else if (left.IsObject()) {
            left.Retain();
            const auto diia_cell =
                left.AsObject()->GetProperty(this, MAG_GREATER_EQUAL);
            diia_cell.Retain();
            right.Retain();
            const auto result = diia_cell.Call(this, {right}, {});
            if (result.IsError()) {
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
          left.Retain();
          if (left.IsNumber()) {
            if (right.IsNumber()) {
              if (left.AsNumber() < right.AsNumber()) {
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
          } else if (left.IsObject()) {
            const auto diia_cell =
                left.AsObject()->GetProperty(this, MAG_LESSER);
            diia_cell.Retain();
            const auto result = diia_cell.Call(this, {right}, {});
            if (result.IsError()) {
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
          left.Retain();
          if (left.IsNumber()) {
            if (right.IsNumber()) {
              if (left.AsNumber() <= right.AsNumber()) {
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
          } else if (left.IsObject()) {
            const auto diia_cell =
                left.AsObject()->GetProperty(this, MAG_LESSER_EQUAL);
            diia_cell.Retain();
            const auto result = diia_cell.Call(this, {right}, {});
            if (result.IsError()) {
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
          left.Retain();
          if (left.IsObject()) {
            const auto diia_cell =
                left.AsObject()->GetProperty(this, MAG_CONTAINS);
            diia_cell.Retain();
            const auto result = diia_cell.Call(this, {right}, {});
            if (result.IsError()) {
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
          left.Retain();
          if (left.IsEmpty()) {
            if (right.IsEmpty()) {
              PUSH_YES();
            } else {
              PUSH_NO();
            }
            break;
          }
          if (left.IsNumber()) {
            if (right.AsObject() == this->number_structure_object) {
              PUSH_YES();
            } else {
              PUSH_NO();
            }
            break;
          }
          if (left.IsYes() || left.IsNo()) {
            if (right.v.object == this->logical_structure_object) {
              PUSH_YES();
            } else {
              PUSH_NO();
            }
            break;
          }
          if (right.IsObject() && left.IsObject()) {
            if (right.v.object == left.v.object->structure) {
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
          if (value.IsEmpty()) {
            PUSH_YES();
          } else if (value.IsNumber()) {
            if (value.v.number == 0.0) {
              PUSH_YES();
            } else {
              PUSH_NO();
            }
          } else if (value.IsYes()) {
            PUSH_NO();
          } else if (value.IsNo()) {
            PUSH_YES();
          } else {
            PUSH_NO();
          }
          break;
        }
        case VNegative: {
          POP_VALUE(value);
          if (value.IsNumber()) {
            PUSH_NUMBER(-value.AsNumber());
          } else if (value.IsObject()) {
            const auto diia_cell =
                value.AsObject()->GetProperty(this, MAG_NEGATIVE);
            diia_cell.Retain();
            const auto result = diia_cell.Call(this, {}, {});
            if (result.IsError()) {
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
          if (value.IsNumber()) {
            PUSH_NUMBER(value.v.number * -1);
          } else if (value.IsObject()) {
            const auto diia_cell =
                value.AsObject()->GetProperty(this, MAG_POSITIVE);
            diia_cell.Retain();
            const auto result = diia_cell.Call(this, {}, {});
            if (result.IsError()) {
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
          if (value.IsNumber()) {
            PUSH_NUMBER(
                static_cast<double>(~static_cast<long>(value.v.number)));
          } else if (value.IsObject()) {
            const auto diia_cell =
                value.AsObject()->GetProperty(this, MAG_BW_NOT);
            diia_cell.Retain();
            const auto result = diia_cell.Call(this, {}, {});
            if (result.IsError()) {
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
          left.Retain();
          if (left.IsNumber()) {
            if (right.IsNumber()) {
              PUSH_NUMBER(left.AsNumber() + right.AsNumber());
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_ADD) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.IsObject()) {
            const auto diia_cell = left.AsObject()->GetProperty(this, MAG_ADD);
            diia_cell.Retain();
            const auto result = diia_cell.Call(this, {right}, I.location);
            if (result.IsError()) {
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
          left.Retain();
          if (left.IsNumber()) {
            if (right.IsNumber()) {
              PUSH_NUMBER(left.AsNumber() - right.AsNumber());
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_SUB) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.IsObject()) {
            const auto diia_cell = left.AsObject()->GetProperty(this, MAG_SUB);
            diia_cell.Retain();
            const auto result = diia_cell.Call(this, {right}, I.location);
            if (result.IsError()) {
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
          left.Retain();
          if (left.IsNumber()) {
            if (right.IsNumber()) {
              PUSH_NUMBER(left.AsNumber() * right.AsNumber());
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_MUL) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.IsObject()) {
            const auto diia_cell = left.AsObject()->GetProperty(this, MAG_MUL);
            diia_cell.Retain();
            const auto result = diia_cell.Call(this, {right}, I.location);
            if (result.IsError()) {
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
          left.Retain();
          if (left.IsNumber()) {
            if (right.IsNumber()) {
              PUSH_NUMBER(left.AsNumber() / right.AsNumber());
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_DIV) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.IsObject()) {
            const auto diia_cell = left.AsObject()->GetProperty(this, MAG_DIV);
            diia_cell.Retain();
            const auto result = diia_cell.Call(this, {right}, I.location);
            if (result.IsError()) {
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
          left.Retain();
          if (left.IsNumber()) {
            if (right.IsNumber()) {
              PUSH_NUMBER(fmod(left.AsNumber(), right.AsNumber()));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_MOD) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.IsObject()) {
            const auto diia_cell = left.AsObject()->GetProperty(this, MAG_MOD);
            diia_cell.Retain();
            const auto result = diia_cell.Call(this, {right}, I.location);
            if (result.IsError()) {
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
          left.Retain();
          if (left.IsNumber()) {
            if (right.IsNumber()) {
              PUSH_NUMBER(floor(left.AsNumber() / right.AsNumber()));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_DIVDIV) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.IsObject()) {
            const auto diia_cell =
                left.AsObject()->GetProperty(this, MAG_DIVDIV);
            diia_cell.Retain();
            const auto result = diia_cell.Call(this, {right}, I.location);
            if (result.IsError()) {
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
          left.Retain();
          if (left.IsNumber()) {
            if (right.IsNumber()) {
              PUSH_NUMBER(pow(left.AsNumber(), right.AsNumber()));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_POW) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.IsObject()) {
            const auto diia_cell = left.AsObject()->GetProperty(this, MAG_POW);
            diia_cell.Retain();
            const auto result = diia_cell.Call(this, {right}, I.location);
            if (result.IsError()) {
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
          left.Retain();
          if (left.IsNumber()) {
            if (right.IsNumber()) {
              PUSH_NUMBER(
                  static_cast<double>(static_cast<long>(left.AsNumber()) ^
                                      static_cast<long>(right.AsNumber())));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_BW_XOR) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.IsObject()) {
            const auto diia_cell =
                left.AsObject()->GetProperty(this, MAG_BW_XOR);
            diia_cell.Retain();
            const auto result = diia_cell.Call(this, {right}, {});
            if (result.IsError()) {
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
          left.Retain();
          if (left.IsNumber()) {
            if (right.IsNumber()) {
              PUSH_NUMBER(
                  static_cast<double>(static_cast<long>(left.AsNumber()) |
                                      static_cast<long>(right.AsNumber())));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_BW_OR) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.IsObject()) {
            const auto diia_cell =
                left.AsObject()->GetProperty(this, MAG_BW_OR);
            diia_cell.Retain();
            const auto result = diia_cell.Call(this, {right}, {});
            if (result.IsError()) {
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
          left.Retain();
          if (left.IsNumber()) {
            if (right.IsNumber()) {
              PUSH_NUMBER(
                  static_cast<double>(static_cast<long>(left.AsNumber()) &
                                      static_cast<long>(right.AsNumber())));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_BW_AND) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.IsObject()) {
            const auto diia_cell =
                left.AsObject()->GetProperty(this, MAG_BW_AND);
            diia_cell.Retain();
            const auto result = diia_cell.Call(this, {right}, {});
            if (result.IsError()) {
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
          left.Retain();
          if (left.IsNumber()) {
            if (right.IsNumber()) {
              PUSH_NUMBER(
                  static_cast<double>(static_cast<long>(left.AsNumber())
                                      << static_cast<long>(right.AsNumber())));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_BW_SHIFT_LEFT) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.IsObject()) {
            const auto diia_cell =
                left.AsObject()->GetProperty(this, MAG_BW_SHIFT_LEFT);
            diia_cell.Retain();
            const auto result = diia_cell.Call(this, {right}, {});
            if (result.IsError()) {
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
          left.Retain();
          if (left.IsNumber()) {
            if (right.IsNumber()) {
              PUSH_NUMBER(
                  static_cast<double>(static_cast<long>(left.AsNumber()) >>
                                      static_cast<long>(right.AsNumber())));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_BW_SHIFT_RIGHT) +
                      "\" для типу \"число\" "
                      "очікує параметром значення типу \"число\".",
                  I.location)
            }
          } else if (left.IsObject()) {
            const auto diia_cell =
                left.AsObject()->GetProperty(this, MAG_BW_SHIFT_RIGHT);
            diia_cell.Retain();
            const auto result = diia_cell.Call(this, {right}, {});
            if (result.IsError()) {
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
          if (result.IsError()) {
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
    if (result.IsError()) {
      return result;
    }
    return result;
  }

  MaValue MaMa::DoTake(const std::string& path,
                       const std::string& name,
                       const std::string& code,
                       const MaLocation& location) {
    const auto parent_scope = this->frame_stack.empty()
                                  ? this->global_scope
                                  : this->frame_stack.top()->scope;
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
    if (result.IsError()) {
      return result;
    }
    FRAME_POP();
    return MaValue::Object(module_object);
  }
} // namespace mavka::mama