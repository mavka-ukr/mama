#include "mama.h"

namespace mavka::mama {
  std::string MaCell::get_name() const {
    if (type == MA_CELL_OBJECT) {
      if (v.object->structure) {
        return v.object->structure->d.structure->name;
      }
    }
    switch (type) {
      case MA_CELL_EMPTY:
        return "пусто";
      case MA_CELL_NUMBER:
        return "число";
      case MA_CELL_YES:
        return "логічне";
      case MA_CELL_NO:
        return "логічне";
      case MA_CELL_OBJECT:
        return "обʼєкт";
      case MA_CELL_ARGS:
        return "аргументи";
      default:
        break;
    }
    return std::to_string(type);
  }

  std::string cell_to_string(MaCell cell, int depth) {
    if (cell.type == MA_CELL_EMPTY) {
      return "пусто";
    }
    if (cell.type == MA_CELL_NUMBER) {
      return ma_number_to_string(cell.v.number);
    }
    if (cell.type == MA_CELL_YES) {
      return "так";
    }
    if (cell.type == MA_CELL_NO) {
      return "ні";
    }
    if (cell.type == MA_CELL_OBJECT) {
      if (cell.v.object->type == MA_OBJECT) {
        std::vector<std::string> items;
        for (const auto& param :
             cell.v.object->structure->d.structure->params) {
          const auto value = ma_object_get(cell.v.object, param.name);
          items.push_back(param.name + "=" + cell_to_string(value, depth + 1));
        }
        return cell.v.object->structure->d.structure->name + "(" +
               internal::tools::implode(items, ", ") + ")";
      }
      if (cell.v.object->type == MA_OBJECT_DIIA) {
        const auto name = cell.v.object->d.diia->name;
        if (name == "") {
          return "<дія>";
        }
        return "<дія " + name + ">";
      }
      if (cell.v.object->type == MA_OBJECT_DIIA_NATIVE) {
        const auto name = cell.v.object->d.diia_native->name;
        if (name == "") {
          return "<дія>";
        }
        return "<дія " + name + ">";
      }
      if (cell.v.object->type == MA_OBJECT_STRING) {
        if (depth > 0) {
          return "\"" + cell.v.object->d.string->data + "\"";
        }
        return cell.v.object->d.string->data;
      }
      if (cell.v.object->type == MA_OBJECT_LIST) {
        std::vector<std::string> items;
        for (const auto& item : cell.v.object->d.list->data) {
          items.push_back(cell_to_string(item, depth + 1));
        }
        return "[" + internal::tools::implode(items, ", ") + "]";
      }
      if (cell.v.object->type == MA_OBJECT_DICT) {
        std::vector<std::string> items;
        for (const auto& item : cell.v.object->d.dict->data) {
          items.push_back(cell_to_string(item.first, depth + 1) + "=" +
                          cell_to_string(item.second, depth + 1));
        }
        return "(" + internal::tools::implode(items, ", ") + ")";
      }
      if (cell.v.object->type == MA_OBJECT_STRUCTURE) {
        return "<структура " + cell.v.object->d.structure->name + ">";
      }
      if (cell.v.object->type == MA_OBJECT_MODULE) {
        const auto name = cell.v.object->d.module->name;
        std::vector<std::string> items;
        for (const auto& [k, v] : cell.v.object->properties) {
          if (k != "назва") {
            items.push_back(k);
          }
        }
        return "<модуль " + name + "[" + internal::tools::implode(items, ", ") +
               "]>";
      }
    }
    return "<невідомо>";
  }

  void print_instruction_with_index(MaCode* code,
                                    int index,
                                    MaInstruction instruction) {
    std::cout << code << "[" << index << "]: " << instruction.to_string()
              << std::endl;
  }
} // namespace mavka::mama
