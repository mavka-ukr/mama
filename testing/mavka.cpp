#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include "../src/mama.h"
#include "../src/takeFs.h"

using namespace mavka::mama;

std::string cell_to_string(MaMa* M, MaValue cell, int depth = 0);

std::string cell_to_string(MaMa* M, MaValue cell, int depth) {
  if (cell.isEmpty()) {
    return "пусто";
  }
  if (cell.isNumber()) {
    if (std::isinf(cell.asNumber())) {
      return "нескінченність";
    }
    if (std::isnan(cell.asNumber())) {
      return "невизначеність";
    }
    return ma_number_to_string(cell.v.number);
  }
  if (cell.isYes()) {
    return "так";
  }
  if (cell.isNo()) {
    return "ні";
  }
  if (cell.isObject()) {
    if (cell.v.object->isDiia(M)) {
      const auto name = cell.v.object->diiaGetName();
      if (name == "") {
        return "<дія>";
      }
      return "<дія " + name + ">";
    }
    if (cell.v.object->isText(M)) {
      if (depth > 0) {
        return "\"" + cell.v.object->textData + "\"";
      }
      return cell.v.object->textData;
    }
    if (cell.v.object->isList(M)) {
      std::vector<std::string> items;
      for (const auto& item : cell.v.object->listData) {
        items.push_back(cell_to_string(M, item, depth + 1));
      }
      return "[" + mavka::internal::tools::implode(items, ", ") + "]";
    }
    if (cell.v.object->isDict(M)) {
      std::vector<std::string> items;
      for (const auto& item : cell.v.object->dictData) {
        items.push_back(cell_to_string(M, item.first, depth + 1) + "=" +
                        cell_to_string(M, item.second, depth + 1));
      }
      return "(" + mavka::internal::tools::implode(items, ", ") + ")";
    }
    if (cell.v.object->isStructure(M)) {
      return "<структура " + cell.v.object->structureGetName() + ">";
    }
    if (cell.v.object->isModule(M)) {
      const auto name = cell.v.object->moduleGetName();
      std::vector<std::string> items;
      for (const auto& [k, v] : cell.v.object->properties) {
        if (k != "назва") {
          items.push_back(k);
        }
      }
      return "<модуль " + name + "[" +
             mavka::internal::tools::implode(items, ", ") + "]>";
    }
    std::vector<std::string> items;
    for (const auto& param :
         cell.v.object->getStructure()->structureGetParams()) {
      const auto value = cell.v.object->getProperty(M, param.name);
      items.push_back(param.name + "=" + cell_to_string(M, value, depth + 1));
    }
    return cell.v.object->getStructure()->structureGetName() + "(" +
           mavka::internal::tools::implode(items, ", ") + ")";
  }
  if (cell.isError()) {
    return "<помилка>";
  }
  return "<невідомо>";
}

void init_print(MaMa* M) {
  const auto native_fn = [](MaMa* M, MaObject* me, MaObject* args, size_t li) {
    for (const auto& [key, value] : args->properties) {
      std::cout << cell_to_string(M, value) << std::endl;
    }
    return MaValue::Empty();
  };
  M->global_scope->setProperty(
      M, "друк", MaObject::CreateDiiaNativeFn(M, "друк", native_fn, nullptr));
}

void init_read(MaMa* M) {
  const auto native_fn = [](MaMa* M, MaObject* me, MaObject* args, size_t li) {
    const auto prefix = args->getArg(M, "0", "префікс");
    if (prefix.isObject() && prefix.asObject()->isText(M)) {
      std::cout << prefix.asObject()->textData;
    }
    std::string value;
    getline(std::cin, value);
    if (std::cin.eof()) {
      return MaValue::Empty();
    }
    return MaValue::Object(MaObject::CreateText(M, value));
  };
  M->global_scope->setProperty(
      M, "читати",
      MaObject::CreateDiiaNativeFn(M, "читати", native_fn, nullptr));
}

int main(int argc, char** argv) {
  const auto args = std::vector<std::string>(argv, argv + argc);

  const auto M = MaMa::Create();
  M->take_fn = maTakeFsFn;

  init_print(M);
  init_read(M);

  const auto take_result = maTakeFsPath(M, args[1], true, {});
  if (take_result.isError()) {
    const auto stackTrace = M->getStackTrace();
    if (!stackTrace.empty()) {
      std::cout << stackTrace << std::endl;
    }
    const auto location = M->locations[take_result.asError()->li];
    std::cerr << location.path << ":" << location.line << ":" << location.column
              << ": " << cell_to_string(M, take_result.v.error->value)
              << std::endl;
    return 1;
  }

  return 0;
}