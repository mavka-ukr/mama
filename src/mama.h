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
#define MAG_LOGICAL "чародія_логічне"
#define MAG_BYTES "чародія_байти"
#define MAG_BYTES "чародія_байти"

#define MA_CELL_EMPTY 0
#define MA_CELL_NUMBER 1
#define MA_CELL_YES 2
#define MA_CELL_NO 3
#define MA_CELL_OBJECT 4
#define MA_CELL_ARGS 5
#define MA_CELL_ERROR 6

#define MA_OBJECT 0
#define MA_OBJECT_DIIA 1
#define MA_OBJECT_DIIA_NATIVE 2
#define MA_OBJECT_STRING 3
#define MA_OBJECT_LIST 4
#define MA_OBJECT_DICT 5
#define MA_OBJECT_STRUCTURE 6
#define MA_OBJECT_MODULE 7

#define MA_MAKE_ARGS(value) (MaCell{MA_CELL_ARGS, {.args = (value)}})
#define MA_MAKE_ERROR(value) (MaCell{MA_CELL_ERROR, {.error = (value)}})

#define IS_EMPTY(cell) ((cell).type == MA_CELL_EMPTY)
#define IS_NUMBER(cell) ((cell).type == MA_CELL_NUMBER)
#define IS_YES(cell) ((cell).type == MA_CELL_YES)
#define IS_NO(cell) ((cell).type == MA_CELL_NO)
#define IS_OBJECT(cell) ((cell).type == MA_CELL_OBJECT)
#define IS_OBJECT_STRING(cell) (cell).v.object->type == MA_OBJECT_STRING
#define IS_OBJECT_STRUCTURE(cell) (cell).v.object->type == MA_OBJECT_STRUCTURE
#define IS_OBJECT_DIIA(cell) (cell).v.object->type == MA_OBJECT_DIIA
#define IS_OBJECT_DIIA_NATIVE(cell) \
  (cell).v.object->type == MA_OBJECT_DIIA_NATIVE
#define IS_STRING(cell) IS_OBJECT(cell) && IS_OBJECT_STRING(cell)
#define IS_ARGS(cell) ((cell).type == MA_CELL_ARGS)
#define IS_ERROR(cell) ((cell).type == MA_CELL_ERROR)

#define PUSH(cell) frame->stack.push(cell)
#define PUSH_EMPTY() PUSH(MaCell::Empty())
#define PUSH_NUMBER(v) PUSH(MaCell::Number((v)))
#define PUSH_YES() PUSH(MaCell::Yes())
#define PUSH_NO() PUSH(MaCell::No())
#define PUSH_OBJECT(v) PUSH(MaCell::Object((v)))
#define PUSH_ARGS(v) PUSH(MA_MAKE_ARGS((v)))

#define RETURN(cell) return cell;
#define RETURN_EMPTY() return MaCell::Empty();
#define RETURN_NUMBER(v) return MaCell::Number((v));
#define RETURN_YES() return MaCell::Yes();
#define RETURN_NO() return MaCell::No();
#define RETURN_OBJECT(v) return MaCell::Object((v));
#define RETURN_ERROR(v) return MA_MAKE_ERROR((v));

#define TOP() frame->stack.top()
#define TOP_VALUE(name) const auto name = TOP();
#define POP() frame->stack.pop();
#define POP_VALUE(name)    \
  const auto name = TOP(); \
  POP();

#define OBJECT_STRING_DATA(cell) (cell).v.object->d.string->data

#define READ_TOP_FRAME() const auto frame = M->frame_stack.top();
#define FRAME_POP() M->frame_stack.pop();
#define FRAME_TOP() M->frame_stack.top();
#define FRAME_PUSH(frame) M->frame_stack.push(frame);
#define POP_FRAME(name)          \
  const auto name = FRAME_TOP(); \
  FRAME_POP();

#define OBJECT_GET(cell, varname, propname)                       \
  MaCell varname{};                                               \
  if ((cell).v.object->get) {                                     \
    varname = (cell).v.object->get(M, (cell).v.object, propname); \
  } else {                                                        \
    if ((cell).v.object->properties.contains(propname)) {         \
      varname = (cell).v.object->properties[propname];            \
    } else {                                                      \
      varname = MaCell::Empty();                                  \
    }                                                             \
  }
#define OBJECT_SET(cell, propname, value)                      \
  if ((cell).v.object->set) {                                  \
    (cell).v.object->set(M, (cell).v.object, propname, value); \
  } else {                                                     \
    (cell).v.object->properties[propname] = value;             \
  }

#define DO_RETURN_STRING_ERROR(v, location) \
  RETURN_ERROR(new MaError(MaCell::Object(MaText::Create(M, (v))), (location)));
#define DO_RETURN_DIIA_NOT_DEFINED_FOR_TYPE_ERROR(varname, cell, location) \
  DO_RETURN_STRING_ERROR("Дію \"" + std::string(varname) +                 \
                             "\" не визначено для типу \"" +               \
                             (cell).GetName() + "\".",                     \
                         (location))
#define DO_RETURN_CANNOT_CALL_CELL_ERROR(cell, location)                    \
  DO_RETURN_STRING_ERROR("Неможливо викликати \"" + cell.GetName() + "\".", \
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
  class MaDiiaNative;
  class MaModule;
  struct MaCell;
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

  struct MaError {
    MaCell value;
    MaLocation location;

    static MaError* Create(const MaCell& value, const MaLocation& location) {
      const auto error = new MaError();
      error->value = value;
      error->location = location;
      return error;
    }

    static MaError* Create(MaMa* M,
                           const std::string& value,
                           const MaLocation& location) {
      const auto error = new MaError();
      error->value = MaCell::Object(MaText::Create(M, value));
      error->location = location;
      return error;
    }
  };

  struct MaCode {
    std::vector<MaInstruction> instructions;
    std::string path;

    inline void push(const MaInstruction& instruction) {
      instructions.push_back(instruction);
    }
  };

  struct MaMa {
    std::vector<MaCell> constants;
    MaScope* global_scope;
    std::unordered_map<std::string, MaObject*> loaded_file_modules;
    MaObject* main_module;

    std::stack<MaFrame*> frame_stack;

    MaObject* object_structure_object;
    MaObject* structure_structure_object;
    MaObject* number_structure_object;
    MaObject* logical_structure_object;
    MaObject* text_structure_object;
    MaObject* diia_structure_object;
    MaObject* list_structure_object;
    MaObject* dict_structure_object;
    MaObject* module_structure_object;

    std::function<MaCell(MaMa*,
                         const std::string& repository,
                         bool relative,
                         const std::vector<std::string>& parts,
                         const MaLocation& location)>
        TakeFn;

    static MaMa* Create();

    MaCell Run(MaCode* code);

    MaCell DoTake(const std::string& id,
                  const std::string& name,
                  const std::string& code,
                  const MaLocation& location);
  };
} // namespace mavka::mama

#endif // MAMA_H
