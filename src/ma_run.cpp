#include "mama.h"

namespace mavka::mama {
  MaCell ma_run(MaMa* M, MaCode* code) {
    READ_TOP_FRAME();
    auto size = code->instructions.size();
    size_t i = 0;
    for (;;) {
    start:
      if (i >= size) {
        return frame->stack.top();
      }
      auto I = code->instructions[i];

      DEBUG_DO(print_instruction_with_index(code, i, I))

      switch (I.v) {
        case VPop: {
          POP();
          break;
        }
        case VConstant: {
          PUSH(M->constants[I.data.constant]);
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
          PUSH_ARGS(new MaArgs(I.data.args_type));
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
          POP_VALUE(args_cell);
          POP_VALUE(cell);
          const auto result = cell.Call(M, args_cell.v.args, I.location);
          if (result.IsError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VReturn: {
          return frame->stack.top();
        }
        case VDiia: {
          const auto diia_object =
              MaDiia::Create(M, I.data.diia->name, I.data.diia->code, nullptr);
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
          DO_RETURN_STRING_ERROR("Субʼєкт \"" + I.data.load->name +
                                 "\" не визначено.")
        }
        case VJump: {
          i = I.data.jump;
          goto start;
        }
        case VJumpIfTrue: {
          POP_VALUE(cell);
          if (IS_NUMBER(cell) && cell.v.number != 0.0) {
            i = I.data.jumpIfTrue;
            goto start;
          }
          if (!IS_NO(cell)) {
            i = I.data.jumpIfTrue;
            goto start;
          }
          break;
        }
        case VJumpIfFalse: {
          POP_VALUE(cell);
          if (IS_EMPTY(cell)) {
            i = I.data.jumpIfFalse;
            goto start;
          } else if (IS_NUMBER(cell)) {
            if (cell.v.number == 0.0) {
              i = I.data.jumpIfFalse;
              goto start;
            }
          } else if (IS_NO(cell)) {
            i = I.data.jumpIfFalse;
            goto start;
          }
          break;
        }
        case VEJumpIfTrue: {
          TOP_VALUE(cell);
          if (IS_NUMBER(cell)) {
            if (cell.v.number != 0.0) {
              i = I.data.jumpIfTrue;
              goto start;
            }
          } else if (!IS_NO(cell)) {
            i = I.data.jumpIfTrue;
            goto start;
          }
          break;
        }
        case VEJumpIfFalse: {
          TOP_VALUE(cell);
          if (IS_EMPTY(cell)) {
            i = I.data.jumpIfFalse;
            goto start;
          }
          if (IS_NUMBER(cell) && cell.v.number == 0.0) {
            i = I.data.jumpIfFalse;
            goto start;
          }
          if (IS_NO(cell)) {
            i = I.data.jumpIfFalse;
            goto start;
          }
          break;
        }
        case VGet: {
          POP_VALUE(cell);
          if (IS_OBJECT(cell)) {
            OBJECT_GET(cell, value, I.data.get->name);
            PUSH(value);
            break;
          }
          PUSH_EMPTY();
          break;
        }
        case VSet: {
          POP_VALUE(cell);
          POP_VALUE(value);
          if (IS_OBJECT(cell)) {
            if (IS_OBJECT_STRING(cell)) {
              break;
            }
            OBJECT_SET(cell, I.data.set->name, value);
            break;
          }
          break;
        }
        case VESetR: {
          POP_VALUE(value);
          TOP_VALUE(cell);
          if (IS_OBJECT(cell)) {
            cell.v.object->SetProperty(I.data.set->name, value);
          }
          break;
        }
        case VTry: {
          const auto frames_size = M->frame_stack.size();
          const auto result = ma_run(M, I.data.try_->try_code);
          if (IS_ERROR(result)) {
            const auto value = M->throw_cell;
            PUSH(value);
            while (M->frame_stack.size() > frames_size) {
              FRAME_POP();
            }
            const auto result2 = ma_run(M, I.data.try_->catch_code);
            if (IS_ERROR(result2)) {
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
          RETURN_ERROR(new MaError(cell));
        }
        case VList: {
          PUSH_OBJECT(MaList::Create(M));
          break;
        }
        case VListAppend: {
          POP_VALUE(value);
          TOP_VALUE(list_cell);
          list_cell.v.object->d.list->append(value);
          break;
        }
        case VDict: {
          PUSH_OBJECT(MaDict::Create(M));
          break;
        }
        case VDictSet: {
          POP_VALUE(value);
          TOP_VALUE(dict_cell);
          dict_cell.v.object->d.dict->set(
              MaCell::Object(MaText::Create(M, I.data.dictSet->key)), value);
          break;
        }
        case VStruct: {
          const auto structure_object =
              MaStructure::Create(M, I.data.struct_->name);
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
          if (IS_OBJECT(structure_cell)) {
            if (IS_OBJECT_STRUCTURE(structure_cell)) {
              structure_cell.v.object->d.structure->methods.push_back(
                  diia_cell.v.object);
              break;
            }
          }
          DO_RETURN_STRING_ERROR("Неможливо створити метод для типу " +
                                 structure_cell.GetName())
        }
        case VModule: {
          const auto module_object = MaModule::Create(M, I.data.module->name);
          const auto module_scope = new MaScope(frame->scope);
          const auto module_frame =
              new MaFrame(module_scope, module_object, frame->module);
          FRAME_PUSH(module_frame);
          frame->scope->SetSubject(I.data.module->name, module_object);
          break;
        }
        case VGive: {
          POP_VALUE(value);
          frame->object->properties.insert_or_assign(I.data.give->name, value);
        }
        case VEq: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (IS_EMPTY(left)) {
            if (IS_EMPTY(right)) {
              PUSH_YES();
            } else {
              PUSH_NO();
            }
          } else if (IS_YES(left)) {
            if (IS_YES(right)) {
              PUSH_YES();
            } else {
              PUSH_NO();
            }
          } else if (IS_NO(left)) {
            if (IS_NO(right)) {
              PUSH_YES();
            } else {
              PUSH_NO();
            }
          } else if (IS_NUMBER(left)) {
            if (IS_NUMBER(right)) {
              if (left.v.number == right.v.number) {
                PUSH_YES();
              } else {
                PUSH_NO();
              }
            } else {
              PUSH_NO();
            }
          } else if (IS_OBJECT(left)) {
            if (IS_OBJECT_STRING(left)) {
              if (IS_OBJECT(right) && IS_OBJECT_STRING(right)) {
                if (OBJECT_STRING_DATA(left) == OBJECT_STRING_DATA(right)) {
                  PUSH_YES();
                } else {
                  PUSH_NO();
                }
              } else {
                PUSH_NO();
              }
            } else {
              if (left.v.object == right.v.object) {
                PUSH_YES();
              } else {
                PUSH_NO();
              }
            }
          } else {
            PUSH_NO();
          }
          break;
        }
        case VGt: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (IS_NUMBER(left)) {
            if (IS_NUMBER(right)) {
              if (left.v.number > right.v.number) {
                PUSH_YES();
              } else {
                PUSH_NO();
              }
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_GREATER) +
                  "\" для типу \"число\" "
                  "очікує параметром значення типу \"число\".")
            }
          } else if (IS_OBJECT(left)) {
            OBJECT_GET(left, diia_cell, MAG_GREATER);
            const auto result = diia_cell.Call(M, {right}, {});
            if (IS_ERROR(result)) {
              return result;
            }
            PUSH(result);
            ;
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_GREATER, left);
          }
          break;
        }
        case VGe: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (IS_NUMBER(left)) {
            if (IS_NUMBER(right)) {
              if (left.v.number >= right.v.number) {
                PUSH_YES();
              } else {
                PUSH_NO();
              }
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_GREATER_EQUAL) +
                  "\" для типу \"число\" "
                  "очікує параметром значення типу \"число\".")
            }
          } else if (IS_OBJECT(left)) {
            OBJECT_GET(left, diia_cell, MAG_GREATER_EQUAL);
            const auto result = diia_cell.Call(M, {right}, {});
            if (IS_ERROR(result)) {
              return result;
            }
            PUSH(result);
            ;
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_GREATER_EQUAL, left);
          }
          break;
        }
        case VLt: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (IS_NUMBER(left)) {
            if (IS_NUMBER(right)) {
              if (left.v.number < right.v.number) {
                PUSH_YES();
              } else {
                PUSH_NO();
              }
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_LESSER) +
                  "\" для типу \"число\" "
                  "очікує параметром значення типу \"число\".")
            }
          } else if (IS_OBJECT(left)) {
            OBJECT_GET(left, diia_cell, MAG_LESSER);
            const auto result = diia_cell.Call(M, {right}, {});
            if (IS_ERROR(result)) {
              return result;
            }
            PUSH(result);
            ;
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_LESSER, left);
          }
          break;
        }
        case VLe: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (IS_NUMBER(left)) {
            if (IS_NUMBER(right)) {
              if (left.v.number <= right.v.number) {
                PUSH_YES();
              } else {
                PUSH_NO();
              }
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_LESSER_EQUAL) +
                  "\" для типу \"число\" "
                  "очікує параметром значення типу \"число\".")
            }
          } else if (IS_OBJECT(left)) {
            OBJECT_GET(left, diia_cell, MAG_LESSER_EQUAL);
            const auto result = diia_cell.Call(M, {right}, {});
            if (IS_ERROR(result)) {
              return result;
            }
            PUSH(result);
            ;
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_LESSER_EQUAL, left);
          }
          break;
        }
        case VContains: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (IS_OBJECT(left)) {
            OBJECT_GET(left, diia_cell, MAG_CONTAINS);
            const auto result = diia_cell.Call(M, {right}, {});
            if (IS_ERROR(result)) {
              return result;
            }
            PUSH(result);
            ;
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_CONTAINS, left);
          }
          break;
        }
        case VIs: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (IS_EMPTY(left)) {
            if (IS_EMPTY(right)) {
              PUSH_YES();
            } else {
              PUSH_NO();
            }
            break;
          }
          if (IS_NUMBER(left)) {
            if (right.v.object == M->number_structure_object) {
              PUSH_YES();
            } else {
              PUSH_NO();
            }
            break;
          }
          if (IS_YES(left) || IS_NO(left)) {
            if (right.v.object == M->logical_structure_object) {
              PUSH_YES();
            } else {
              PUSH_NO();
            }
            break;
          }
          if (IS_OBJECT(right) && IS_OBJECT(left)) {
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
          if (IS_EMPTY(value)) {
            PUSH_YES();
          } else if (IS_NUMBER(value)) {
            if (value.v.number == 0.0) {
              PUSH_YES();
            } else {
              PUSH_NO();
            }
          } else if (IS_YES(value)) {
            PUSH_NO();
          } else if (IS_NO(value)) {
            PUSH_YES();
          } else {
            PUSH_NO();
          }
          break;
        }
        case VNegative: {
          POP_VALUE(value);
          if (IS_NUMBER(value)) {
            PUSH_NUMBER(-value.v.number);
          } else if (IS_OBJECT(value)) {
            OBJECT_GET(value, diia_cell, MAG_NEGATIVE);
            const auto result = diia_cell.Call(M, {}, {});
            if (IS_ERROR(result)) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_NEGATIVE, value);
          }
          break;
        }
        case VPositive: {
          POP_VALUE(value);
          if (IS_NUMBER(value)) {
            PUSH_NUMBER(value.v.number * -1);
          } else if (IS_OBJECT(value)) {
            OBJECT_GET(value, diia_cell, MAG_POSITIVE);
            const auto result = diia_cell.Call(M, {}, {});
            if (IS_ERROR(result)) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_POSITIVE, value);
          }
          break;
        }
        case VBnot: {
          POP_VALUE(value);
          if (IS_NUMBER(value)) {
            PUSH_NUMBER(
                static_cast<double>(~static_cast<long>(value.v.number)));
          } else if (IS_OBJECT(value)) {
            OBJECT_GET(value, diia_cell, MAG_BW_NOT);
            const auto result = diia_cell.Call(M, {}, {});
            if (IS_ERROR(result)) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_BW_NOT, value);
          }
          break;
        }
        case VAdd: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (IS_NUMBER(left)) {
            if (IS_NUMBER(right)) {
              PUSH_NUMBER(left.v.number + right.v.number);
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_ADD) +
                  "\" для типу \"число\" "
                  "очікує параметром значення типу \"число\".")
            }
          } else if (IS_OBJECT(left)) {
            OBJECT_GET(left, diia_cell, MAG_ADD);
            const auto result = diia_cell.Call(M, {right}, I.location);
            if (IS_ERROR(result)) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_ADD, left);
          }
          break;
        }
        case VSub: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (IS_NUMBER(left)) {
            if (IS_NUMBER(right)) {
              PUSH_NUMBER(left.v.number - right.v.number);
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_SUB) +
                  "\" для типу \"число\" "
                  "очікує параметром значення типу \"число\".")
            }
          } else if (IS_OBJECT(left)) {
            OBJECT_GET(left, diia_cell, MAG_SUB);
            const auto result = diia_cell.Call(M, {right}, I.location);
            if (IS_ERROR(result)) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_SUB, left);
          }
          break;
        }
        case VMul: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (IS_NUMBER(left)) {
            if (IS_NUMBER(right)) {
              PUSH_NUMBER(left.v.number * right.v.number);
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_MUL) +
                  "\" для типу \"число\" "
                  "очікує параметром значення типу \"число\".")
            }
          } else if (IS_OBJECT(left)) {
            OBJECT_GET(left, diia_cell, MAG_MUL);
            const auto result = diia_cell.Call(M, {right}, I.location);
            if (IS_ERROR(result)) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_MUL, left);
          }
          break;
        }
        case VDiv: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (IS_NUMBER(left)) {
            if (IS_NUMBER(right)) {
              PUSH_NUMBER(left.v.number / right.v.number);
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_DIV) +
                  "\" для типу \"число\" "
                  "очікує параметром значення типу \"число\".")
            }
          } else if (IS_OBJECT(left)) {
            OBJECT_GET(left, diia_cell, MAG_DIV);
            const auto result = diia_cell.Call(M, {right}, I.location);
            if (IS_ERROR(result)) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_DIV, left);
          }
          break;
        }
        case VMod: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (IS_NUMBER(left)) {
            if (IS_NUMBER(right)) {
              PUSH_NUMBER(fmod(left.v.number, right.v.number));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_MOD) +
                  "\" для типу \"число\" "
                  "очікує параметром значення типу \"число\".")
            }
          } else if (IS_OBJECT(left)) {
            OBJECT_GET(left, diia_cell, MAG_MOD);
            const auto result = diia_cell.Call(M, {right}, I.location);
            if (IS_ERROR(result)) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_MOD, left);
          }
          break;
        }
        case VDivDiv: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (IS_NUMBER(left)) {
            if (IS_NUMBER(right)) {
              PUSH_NUMBER(floor(left.v.number / right.v.number));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_DIVDIV) +
                  "\" для типу \"число\" "
                  "очікує параметром значення типу \"число\".")
            }
          } else if (IS_OBJECT(left)) {
            OBJECT_GET(left, diia_cell, MAG_DIVDIV);
            const auto result = diia_cell.Call(M, {right}, I.location);
            if (IS_ERROR(result)) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_DIVDIV, left);
          }
          break;
        }
        case VPow: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (IS_NUMBER(left)) {
            if (IS_NUMBER(right)) {
              PUSH_NUMBER(pow(left.v.number, right.v.number));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_POW) +
                  "\" для типу \"число\" "
                  "очікує параметром значення типу \"число\".")
            }
          } else if (IS_OBJECT(left)) {
            OBJECT_GET(left, diia_cell, MAG_POW);
            const auto result = diia_cell.Call(M, {right}, I.location);
            if (IS_ERROR(result)) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_POW, left);
          }
          break;
        }
        case VXor: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (IS_NUMBER(left)) {
            if (IS_NUMBER(right)) {
              PUSH_NUMBER(
                  static_cast<double>(static_cast<long>(left.v.number) ^
                                      static_cast<long>(right.v.number)));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_BW_XOR) +
                  "\" для типу \"число\" "
                  "очікує параметром значення типу \"число\".")
            }
          } else if (IS_OBJECT(left)) {
            OBJECT_GET(left, diia_cell, MAG_BW_XOR);
            const auto result = diia_cell.Call(M, {right}, {});
            if (IS_ERROR(result)) {
              return result;
            }
            PUSH(result);
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_BW_XOR, left);
          }
          break;
        }
        case VBor: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (IS_NUMBER(left)) {
            if (IS_NUMBER(right)) {
              PUSH_NUMBER(
                  static_cast<double>(static_cast<long>(left.v.number) |
                                      static_cast<long>(right.v.number)));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_BW_OR) +
                  "\" для типу \"число\" "
                  "очікує параметром значення типу \"число\".")
            }
          } else if (IS_OBJECT(left)) {
            OBJECT_GET(left, diia_cell, MAG_BW_OR);
            const auto result = diia_cell.Call(M, {right}, {});
            if (IS_ERROR(result)) {
              return result;
            }
            PUSH(result);
            ;
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_BW_OR, left);
          }
          break;
        }
        case VBand: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (IS_NUMBER(left)) {
            if (IS_NUMBER(right)) {
              PUSH_NUMBER(
                  static_cast<double>(static_cast<long>(left.v.number) &
                                      static_cast<long>(right.v.number)));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_BW_AND) +
                  "\" для типу \"число\" "
                  "очікує параметром значення типу \"число\".")
            }
          } else if (IS_OBJECT(left)) {
            OBJECT_GET(left, diia_cell, MAG_BW_AND);
            const auto result = diia_cell.Call(M, {right}, {});
            if (IS_ERROR(result)) {
              return result;
            }
            PUSH(result);
            ;
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_BW_AND, left);
          }
          break;
        }
        case VShl: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (IS_NUMBER(left)) {
            if (IS_NUMBER(right)) {
              PUSH_NUMBER(
                  static_cast<double>(static_cast<long>(left.v.number)
                                      << static_cast<long>(right.v.number)));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_BW_SHIFT_LEFT) +
                  "\" для типу \"число\" "
                  "очікує параметром значення типу \"число\".")
            }
          } else if (IS_OBJECT(left)) {
            OBJECT_GET(left, diia_cell, MAG_BW_SHIFT_LEFT);
            const auto result = diia_cell.Call(M, {right}, {});
            if (IS_ERROR(result)) {
              return result;
            }
            PUSH(result);
            ;
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_BW_SHIFT_LEFT, left);
          }
          break;
        }
        case VShr: {
          POP_VALUE(right);
          POP_VALUE(left);
          if (IS_NUMBER(left)) {
            if (IS_NUMBER(right)) {
              PUSH_NUMBER(
                  static_cast<double>(static_cast<long>(left.v.number) >>
                                      static_cast<long>(right.v.number)));
            } else {
              DO_RETURN_STRING_ERROR(
                  "Дія \"" + std::string(MAG_BW_SHIFT_RIGHT) +
                  "\" для типу \"число\" "
                  "очікує параметром значення типу \"число\".")
            }
          } else if (IS_OBJECT(left)) {
            OBJECT_GET(left, diia_cell, MAG_BW_SHIFT_RIGHT);
            const auto result = diia_cell.Call(M, {right}, {});
            if (IS_ERROR(result)) {
              return result;
            }
            PUSH(result);
            ;
            break;
          } else {
            DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(MAG_BW_SHIFT_RIGHT, left);
          }
          break;
        }
        case VTake: {
          const auto result =
              M->Take(I.data.take->repository, I.data.take->relative,
                      I.data.take->path_parts);
          if (IS_ERROR(result)) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VModuleLoad: {
          TOP_VALUE(module_cell);
          OBJECT_GET(module_cell, value, I.data.moduleLoad->name);
          frame->scope->SetSubject(I.data.moduleLoad->as, value);
          break;
        }
        default: {
          std::cout << "unsupported instruction " << I.to_string() << std::endl;
          return frame->stack.top();
        }
      }

      i++;
    }

    return frame->stack.top();
  }
} // namespace mavka::mama
