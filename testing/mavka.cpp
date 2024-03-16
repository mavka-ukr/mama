#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include "../src/mama.h"

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
      const auto name = cell.v.object->d.diia->name;
      if (name == "") {
        return "<дія>";
      }
      return "<дія " + name + ">";
    }
    if (cell.v.object->isText(M)) {
      if (depth > 0) {
        return "\"" + cell.v.object->d.text->data + "\"";
      }
      return cell.v.object->d.text->data;
    }
    if (cell.v.object->isList(M)) {
      std::vector<std::string> items;
      for (const auto& item : cell.v.object->d.list->data) {
        items.push_back(cell_to_string(M, item, depth + 1));
      }
      return "[" + mavka::internal::tools::implode(items, ", ") + "]";
    }
    if (cell.v.object->isDict(M)) {
      std::vector<std::string> items;
      for (const auto& item : cell.v.object->d.dict->data) {
        items.push_back(cell_to_string(M, item.first, depth + 1) + "=" +
                        cell_to_string(M, item.second, depth + 1));
      }
      return "(" + mavka::internal::tools::implode(items, ", ") + ")";
    }
    if (cell.v.object->isStructure(M)) {
      return "<структура " + cell.v.object->d.structure->name + ">";
    }
    if (cell.v.object->isModule(M)) {
      const auto name = cell.v.object->d.module->name;
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
         cell.v.object->getStructure()->d.structure->params) {
      const auto value = cell.v.object->getProperty(M, param.name);
      items.push_back(param.name + "=" + cell_to_string(M, value, depth + 1));
    }
    return cell.v.object->getStructure()->d.structure->name + "(" +
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
  M->global_scope->setProperty(M, "друк",
                               MaDiia::Create(M, "друк", native_fn, nullptr));
}

void init_create_module(MaMa* M) {
  const auto native_fn = [](MaMa* M, MaObject* diiaObject, MaObject* args,
                            size_t li) {
    const auto name = args->getArg(M, "0", "назва");
    if (name.isObject() && name.asObject()->isText(M)) {
      const auto module =
          MaValue::Object(MaModule::Create(M, name.asText()->data));

      const auto path = args->getArg(M, "1", "шлях");
      if (path.isObject() && path.asObject()->isText(M)) {
        module.asObject()->d.module->is_file_module = true;
        if (M->main_module == nullptr) {
          M->main_module = module.asObject();
        }
        M->loaded_file_modules.insert_or_assign(path.asObject()->asText()->data,
                                                module.asObject());
      }

      return module;
    }
    return MaValue::Error(
        MaError::Create(M, "Не вдалося створити модуль.", li));
  };
  M->global_scope->setProperty(
      M, "створити_модуль",
      MaDiia::Create(M, "створити_модуль", native_fn, nullptr));
}

void init_read(MaMa* M) {
  const auto native_fn = [](MaMa* M, MaObject* me, MaObject* args, size_t li) {
    const auto prefix = args->getArg(M, "0", "префікс");
    if (prefix.isObject() && prefix.asObject()->isText(M)) {
      std::cout << prefix.asText()->data;
    }
    std::string value;
    getline(std::cin, value);
    if (std::cin.eof()) {
      return MaValue::Empty();
    }
    return MaValue::Object(MaText::Create(M, value));
  };
  M->global_scope->setProperty(M, "читати",
                               MaDiia::Create(M, "читати", native_fn, nullptr));
}

MaValue TakePath(MaMa* M, const std::string& raw_path, size_t li) {
  const auto canonical_path = std::filesystem::weakly_canonical(raw_path);
  const auto path = canonical_path.string();
  if (!std::filesystem::exists(canonical_path)) {
    return MaValue::Error(MaError::Create(
        M, "Шлях \"" + canonical_path.string() + "\" не існує.", li));
  }
  if (!std::filesystem::is_regular_file(canonical_path)) {
    return MaValue::Error(
        MaError::Create(M, "Шлях \"" + path + "\" не вказує на файл.", li));
  }

  if (M->loaded_file_modules.contains(path)) {
    return MaValue::Object(M->loaded_file_modules[path]);
  }

  auto file = std::ifstream(path);
  if (!file.is_open()) {
    return MaValue::Error(
        MaError::Create(M, "Не вдалося прочитати файл \"" + path + "\".", li));
  }

  const auto fs_path = std::filesystem::path(path);
  const auto name = fs_path.stem().string();

  const auto source = std::string(std::istreambuf_iterator(file),
                                  std::istreambuf_iterator<char>());

  return M->doTake(path, name, source, li);
}

MaValue take_fn(MaMa* M,
                const std::string& repository,
                bool relative,
                const std::vector<std::string>& parts,
                size_t li) {
  if (!repository.empty()) {
    return MaValue::Error(
        MaError::Create(M, "Не підтримується взяття з репозиторію.", li));
  }
  if (relative) {
    return MaValue::Error(
        MaError::Create(M, "Не підтримується взяття відносного шляху.", li));
  }
  const auto cwd = std::filesystem::current_path();
  const auto raw_path =
      cwd.string() + "/" + mavka::internal::tools::implode(parts, "/") + ".м";
  return TakePath(M, raw_path, li);
}

int main(int argc, char** argv) {
  const auto args = std::vector<std::string>(argv, argv + argc);

  const auto M = MaMa::Create();
  M->take_fn = take_fn;

  init_print(M);
  init_read(M);
  init_create_module(M);

  const auto take_result = TakePath(M, args[1], {});
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