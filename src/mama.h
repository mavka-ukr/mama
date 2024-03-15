#ifndef MAMA_H
#define MAMA_H

#include <algorithm>
#include <cmath>
#include <codecvt>
#include <filesystem>
#include <functional>
#include <iostream>
#include <locale>
#include <map>
#include <ostream>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

#include <tsl/ordered_map.h>
#include "external/parser/ast.h"
#include "external/parser/parser.h"
#include "utils/tools.h"

#define MAMA_DEBUG 0
#define MAMA_GC_DEBUG 0

#if MAMA_DEBUG == 0
#define DEBUG_LOG(value)
#else
#define DEBUG_LOG(value) std::cout << (value) << std::endl;
#endif

#if MAMA_DEBUG == 0
#define DEBUG_DO(value)
#else
#define DEBUG_DO(value) (value);
#endif

#define MAG_ADD "чародія_додати"
#define MAG_SUB "чародія_відняти"
#define MAG_MUL "чародія_помножити"
#define MAG_DIV "чародія_поділити"
#define MAG_MOD "чародія_остача"
#define MAG_DIVDIV "чародія_частка"
#define MAG_POW "чародія_степінь"
#define MAG_BW_NOT "чародія_дні"
#define MAG_BW_XOR "чародія_вабо"
#define MAG_BW_OR "чародія_дабо"
#define MAG_BW_AND "чародія_ді"
#define MAG_BW_SHIFT_LEFT "чародія_вліво"
#define MAG_BW_SHIFT_RIGHT "чародія_вправо"
#define MAG_POSITIVE "чародія_додатнє"
#define MAG_NEGATIVE "чародія_відʼємне"
#define MAG_GREATER "чародія_більше"
#define MAG_LESSER "чародія_менше"
#define MAG_GREATER_EQUAL "чародія_не_менше"
#define MAG_LESSER_EQUAL "чародія_не_більше"
#define MAG_CONTAINS "чародія_містить"
#define MAG_GET_ELEMENT "чародія_отримати"
#define MAG_SET_ELEMENT "чародія_покласти"
#define MAG_CALL "чародія_викликати"
#define MAG_ITERATOR "чародія_перебір"
#define MAG_NUMBER "чародія_число"
#define MAG_TEXT "чародія_текст"
#define MAG_BYTES "чародія_байти"
#define MAG_LIST "чародія_список"

#define READ_TOP_FRAME() const auto frame = M->call_stack.top();
#define FRAME_POP() M->call_stack.pop();
#define FRAME_TOP() M->call_stack.top();
#define FRAME_PUSH(frame) M->call_stack.push(frame);
#define POP_FRAME(name)          \
  const auto name = FRAME_TOP(); \
  FRAME_POP();

#define DO_RETURN_STRING_ERROR(v, location)                    \
  return MaValue::Error(                                       \
      MaError::Create(MaValue::Object(MaText::Create(M, (v))), \
                      M->call_stack.top()->module, (location)));
#define DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(varname, cell, location) \
  DO_RETURN_STRING_ERROR("Дію \"" + std::string(varname) +                 \
                             "\" не визначено для типу \"" +               \
                             (cell).getName() + "\".",                     \
                         (location))

namespace mavka::mama {
  struct MaMa;
  struct MaFrame;
  struct MaLocation;
  struct MaInstruction;
  class MaScope;
  struct MaCompilationError;
  struct MaCompilationResult;
  class MaText;
  class MaList;
  class MaDict;
  class MaDiia;
  class MaStructure;
  class MaModule;
  struct MaValue;
  struct MaObject;
  struct MaCode;
  struct MaError;

  struct MaLocation {
    size_t line;
    size_t column;
  };

#include "MaFrame.h"
#include "MaInstruction.h"
#include "MaObject.h"
#include "MaScope.h"
#include "compiler/compiler.h"
#include "utils/helpers.h"

  struct MaCode {
    std::vector<MaInstruction> instructions;
    std::string path;

    inline void push(const MaInstruction& instruction) {
      instructions.push_back(instruction);
    }
  };

  struct MaMa {
    std::vector<MaValue> constants;
    MaScope* global_scope;
    std::unordered_map<std::string, MaObject*> loaded_file_modules;
    MaObject* main_module;

    std::stack<MaFrame*> call_stack;

    MaObject* object_structure_object;
    MaObject* structure_structure_object;
    MaObject* number_structure_object;
    MaObject* logical_structure_object;
    MaObject* text_structure_object;
    MaObject* diia_structure_object;
    MaObject* list_structure_object;
    MaObject* dict_structure_object;
    MaObject* module_structure_object;
    MaObject* bytes_structure_object;

    std::function<MaValue(MaMa*,
                          const std::string& repository,
                          bool relative,
                          const std::vector<std::string>& parts,
                          const MaLocation& location)>
        take_fn;

    MaValue run(MaCode* code, std::stack<MaValue>& stack);
    MaValue eval(const std::string& code, const MaLocation& location = {});

    MaValue doTake(const std::string& id,
                   const std::string& name,
                   const std::string& code,
                   const MaLocation& location);
    MaValue doTakeWithScope(const std::string& id,
                            const std::string& name,
                            const std::string& code,
                            const MaLocation& location,
                            MaScope* module_scope);

    std::string getStackTrace();

    static MaMa* Create();
  };

  struct MaError {
    MaObject* module;
    MaValue value;
    MaLocation location;

    static MaError* Create(const MaValue& value,
                           MaObject* module,
                           const MaLocation& location) {
      const auto error = new MaError();
      error->module = module;
      error->value = value;
      error->location = location;
      return error;
    }

    static MaError* Create(MaMa* M,
                           const std::string& value,
                           const MaLocation& location) {
      const auto error = new MaError();
      error->module = M->call_stack.top()->module;
      error->value = MaValue::Object(MaText::Create(M, value));
      error->location = location;
      return error;
    }
  };
} // namespace mavka::mama

#endif // MAMA_H
