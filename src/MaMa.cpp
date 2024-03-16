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
    M->locations.push_back(MaLocation(0, 0, ""));
    const auto scope_structure_object = new MaObject();
    M->scope_structure_object = scope_structure_object;
    M->global_scope = new MaObject();
    M->global_scope->type = scope_structure_object;
    M->global_scope->d.parent = nullptr;
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
    scope_structure_object->d.structure = new MaStructure();
    scope_structure_object->d.structure->name = "Сковп";
    scope_structure_object->type = M->structure_structure_object;
    return M;
  }

  MaValue MaMa::run(MaCode* code, std::stack<MaValue>& stack) {
    const auto M = this;
    const auto frame = this->call_stack.top();
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
          PUSH(MaValue::Object(MaObject::Empty(this)));
          break;
        }
        case VStoreArg: {
          POP_VALUE(value);
          TOP_VALUE(argsValue);
          argsValue.asObject()->setProperty(this, I.data.store->name, value);
          break;
        }
        case VCall: {
          POP_VALUE(argsValue);
          POP_VALUE(value);
          const auto result = value.call(this, argsValue.asObject(), I.li);
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
          const auto diiaObject = MaDiia::Create(this, I.data.diia->name,
                                                 I.data.diia->code, nullptr);
          diiaObject->d.diia->scope = frame->scope;
          PUSH_OBJECT(diiaObject);
          break;
        }
        case VDiiaParam: {
          POP_VALUE(defaultValue);
          TOP_VALUE(diiaValue);
          diiaValue.asObject()->asDiia()->pushParam(MaDiiaParam{
              .name = I.data.diiaParam->name, .default_value = defaultValue});
          break;
        }
        case VStore: {
          POP_VALUE(value);
          frame->scope->setProperty(M, I.data.store->name, value);
          break;
        }
        case VLoad: {
          const auto scope = frame->scope;
          if (scope->hasProperty(M, I.data.load->name)) {
            PUSH(scope->getProperty(M, I.data.load->name));
            break;
          }
          return MaValue::Error(MaError::Create(
              M, "Субʼєкт \"" + I.data.load->name + "\" не визначено.", I.li));
        }
        case VJump: {
          i = I.data.jump;
          goto start;
        }
        case VJumpIfTrue: {
          POP_VALUE(value);
          if (value.isNumber() && value.asNumber() != 0.0) {
            i = I.data.jumpIfTrue;
            goto start;
          }
          if (!value.isNo()) {
            i = I.data.jumpIfTrue;
            goto start;
          }
          break;
        }
        case VJumpIfFalse: {
          POP_VALUE(value);
          if (value.isEmpty()) {
            i = I.data.jumpIfFalse;
            goto start;
          } else if (value.isNumber()) {
            if (value.asNumber() == 0.0) {
              i = I.data.jumpIfFalse;
              goto start;
            }
          } else if (value.isNo()) {
            i = I.data.jumpIfFalse;
            goto start;
          }
          break;
        }
        case VEJumpIfTrue: {
          TOP_VALUE(value);
          if (value.isNumber()) {
            if (value.asNumber() != 0.0) {
              i = I.data.jumpIfTrue;
              goto start;
            }
          } else if (!value.isNo()) {
            i = I.data.jumpIfTrue;
            goto start;
          }
          break;
        }
        case VEJumpIfFalse: {
          TOP_VALUE(value);
          if (value.isEmpty()) {
            i = I.data.jumpIfFalse;
            goto start;
          }
          if (value.isNumber() && value.asNumber() == 0.0) {
            i = I.data.jumpIfFalse;
            goto start;
          }
          if (value.isNo()) {
            i = I.data.jumpIfFalse;
            goto start;
          }
          break;
        }
        case VGet: {
          POP_VALUE(left);
          if (left.isObject()) {
            PUSH(left.asObject()->getProperty(this, I.data.get->name));
            break;
          }
          PUSH_EMPTY();
          break;
        }
        case VEGet: {
          TOP_VALUE(left);
          if (left.isObject()) {
            PUSH(left.asObject()->getProperty(this, I.data.get->name));
            break;
          }
          PUSH_EMPTY();
          break;
        }
        case VSet: {
          POP_VALUE(left);
          POP_VALUE(value);
          if (left.isObject()) {
            left.asObject()->setProperty(this, I.data.set->name, value);
          }
          break;
        }
        case VESetR: {
          POP_VALUE(value);
          TOP_VALUE(left);
          if (left.isObject()) {
            left.asObject()->setProperty(this, I.data.set->name, value);
          }
          break;
        }
        case VTry: {
          const auto frames_size = this->call_stack.size();
          const auto result = this->run(I.data.try_->try_code, stack);
          if (result.isError()) {
            PUSH(result.asError()->value);
            while (this->call_stack.size() > frames_size) {
              FRAME_POP();
            }
            const auto result2 = this->run(I.data.try_->catch_code, stack);
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
          POP_VALUE(value);
          return MaValue::Error(MaError::Create(value, I.li));
        }
        case VList: {
          PUSH_OBJECT(MaList::Create(this));
          break;
        }
        case VListAppend: {
          POP_VALUE(value);
          TOP_VALUE(listValue);
          listValue.asObject()->asList()->append(M, value);
          break;
        }
        case VDict: {
          PUSH_OBJECT(MaDict::Create(this));
          break;
        }
        case VDictSet: {
          POP_VALUE(value);
          TOP_VALUE(dictValue);
          dictValue.asDict()->setAt(
              M, MaValue::Object(MaText::Create(this, I.data.dictSet->key)),
              value);
          break;
        }
        case VStruct: {
          const auto structureObject =
              MaStructure::Create(this, I.data.struct_->name);
          PUSH_OBJECT(structureObject);
          break;
        }
        case VStructParam: {
          POP_VALUE(defaultValue);
          TOP_VALUE(structureValue);
          structureValue.asObject()->asStructure()->pushParam(MaDiiaParam{
              .name = I.data.diiaParam->name, .default_value = defaultValue});
          break;
        }
        case VStructMethod: {
          POP_VALUE(diiaValue);
          TOP_VALUE(structureValue);
          if (structureValue.isObject()) {
            if (structureValue.asObject()->isStructure(M)) {
              structureValue.asObject()->asStructure()->pushMethod(
                  diiaValue.asObject());
              break;
            }
          }
          DO_RETURN_STRING_ERROR(
              "Неможливо створити метод для типу " + structureValue.getName(),
              I.li)
        }
        case VModule: {
          const auto moduleObject = MaModule::Create(this, I.data.module->name);
          frame->scope->setProperty(M, I.data.module->name, moduleObject);
          const auto make_module_diia_object = MaDiia::Create(
              M, "",
              [&I](MaMa* M, MaObject* diiaObject, MaObject* args, size_t li) {
                std::stack<MaValue> stack;
                const auto result = M->run(I.data.module->code, stack);
                if (result.isError()) {
                  return result;
                }
                return MaValue::Object(diiaObject->d.diia->me);
              },
              moduleObject);
          const auto result =
              make_module_diia_object->call(this, MaObject::Empty(this), I.li);
          if (result.isError()) {
            return result;
          }
          break;
        }
        case VGive: {
          POP_VALUE(value);
          const auto meValue = frame->scope->getProperty(M, "я");
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
          const auto result = left.isGreater(this, right, I.li);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VGe: {
          POP_VALUE(right);
          POP_VALUE(left);
          const auto result = left.isGreaterOrEqual(this, right, I.li);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VLt: {
          POP_VALUE(right);
          POP_VALUE(left);
          const auto result = left.isLesser(this, right, I.li);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VLe: {
          POP_VALUE(right);
          POP_VALUE(left);
          const auto result = left.isLesserOrEqual(this, right, I.li);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VContains: {
          POP_VALUE(right);
          POP_VALUE(left);
          const auto result = left.contains(this, right, I.li);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VIs: {
          POP_VALUE(right);
          POP_VALUE(left);
          const auto result = left.is(this, right, I.li);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VNot: {
          POP_VALUE(value);
          const auto result = value.doNot(this, I.li);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VNegative: {
          POP_VALUE(value);
          const auto result = value.doNegative(this, I.li);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VPositive: {
          POP_VALUE(value);
          const auto result = value.doPositive(this, I.li);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VBnot: {
          POP_VALUE(value);
          const auto result = value.doBNot(this, I.li);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VAdd: {
          POP_VALUE(right);
          POP_VALUE(left);
          const auto result = left.doAdd(this, right, I.li);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VSub: {
          POP_VALUE(right);
          POP_VALUE(left);
          const auto result = left.doSub(this, right, I.li);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VMul: {
          POP_VALUE(right);
          POP_VALUE(left);
          const auto result = left.doMul(this, right, I.li);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VDiv: {
          POP_VALUE(right);
          POP_VALUE(left);
          const auto result = left.doDiv(this, right, I.li);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VMod: {
          POP_VALUE(right);
          POP_VALUE(left);
          const auto result = left.doMod(this, right, I.li);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VDivDiv: {
          POP_VALUE(right);
          POP_VALUE(left);
          const auto result = left.doDivDiv(this, right, I.li);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VPow: {
          POP_VALUE(right);
          POP_VALUE(left);
          const auto result = left.doPow(this, right, I.li);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VXor: {
          POP_VALUE(right);
          POP_VALUE(left);
          const auto result = left.doXor(this, right, I.li);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VBor: {
          POP_VALUE(right);
          POP_VALUE(left);
          const auto result = left.doBor(this, right, I.li);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VBand: {
          POP_VALUE(right);
          POP_VALUE(left);
          const auto result = left.doBand(this, right, I.li);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VShl: {
          POP_VALUE(right);
          POP_VALUE(left);
          const auto result = left.doShl(this, right, I.li);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VShr: {
          POP_VALUE(right);
          POP_VALUE(left);
          const auto result = left.doShr(this, right, I.li);
          if (result.isError()) {
            return result;
          }
          PUSH(result);
          break;
        }
        case VTake: {
          const auto result = this->take_fn(this, I.data.take->repository,
                                            I.data.take->relative,
                                            I.data.take->path_parts, I.li);
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

  MaValue MaMa::eval(const std::string& code, size_t li) {
    const auto M = this;

    const auto parser_result = parser::parse(code);
    if (!parser_result.errors.empty()) {
      const auto error = parser_result.errors[0];
      DO_RETURN_STRING_ERROR(std::to_string(error.line) + ":" +
                                 std::to_string(error.column) + ": " +
                                 error.message,
                             li);
    }

    const auto eval_code = new MaCode();
    const auto body_compilation_result =
        compile_body(this, eval_code, parser_result.module_node->body);
    if (body_compilation_result.error) {
      DO_RETURN_STRING_ERROR(
          std::to_string(body_compilation_result.error->line) + ":" +
              std::to_string(body_compilation_result.error->column) + ": " +
              body_compilation_result.error->message,
          li);
    }
    std::stack<MaValue> stack;
    const auto result = this->run(eval_code, stack);
    if (result.isError()) {
      return result;
    }
    return result;
  }

  MaValue MaMa::doTake(const std::string& path,
                       const std::string& name,
                       const std::string& code,
                       size_t li) {
    const auto M = this;

    const auto parser_result = parser::parse(code);
    if (!parser_result.errors.empty()) {
      const auto error = parser_result.errors[0];
      DO_RETURN_STRING_ERROR(path + ":" + std::to_string(error.line) + ":" +
                                 std::to_string(error.column) + ": " +
                                 error.message,
                             li);
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

    const auto body_compilation_result =
        compile_body(this, module_code, parser_result.module_node->body);
    if (body_compilation_result.error) {
      DO_RETURN_STRING_ERROR(
          path + ":" + std::to_string(body_compilation_result.error->line) +
              ":" + std::to_string(body_compilation_result.error->column) +
              ": " + body_compilation_result.error->message,
          li);
    }

    const auto make_module_diia_object = MaDiia::Create(
        M, "",
        [&module_code](MaMa* M, MaObject* diiaObject, MaObject* args,
                       size_t li) {
          std::stack<MaValue> stack;
          const auto result = M->run(module_code, stack);
          if (result.isError()) {
            return result;
          }
          return MaValue::Object(diiaObject->d.diia->me);
        },
        module_object);

    return make_module_diia_object->call(this, MaObject::Empty(this), li);
  }

  std::string MaMa::getStackTrace() {
    std::vector<std::string> stack_trace{};
    std::stack<MaFrame*> call_stack_copy = this->call_stack;
    while (!call_stack_copy.empty()) {
      const auto frame = call_stack_copy.top();
      call_stack_copy.pop();
      const auto location = this->locations[frame->li];
      const auto path = location.path;
      const auto line = std::to_string(location.line);
      const auto column = std::to_string(location.column);
      stack_trace.push_back("  " + frame->diia->asDiia()->name + " " + path +
                            ":" + line + ":" + column);
    }
    if (!stack_trace.empty()) {
      stack_trace.insert(stack_trace.begin(), "Слід :");
    }
    return internal::tools::implode(stack_trace, "\n");
  }
} // namespace mavka::mama