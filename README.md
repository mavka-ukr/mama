<img src="./assets/cover.png" /> 

# МаМа

Машина вказівок Мавки.

## Використання

```shell
git clone https://github.com/mavka-ukr/mama
git submodule update --init --recursive
```

```CMake
cmake_minimum_required(VERSION 3.26)
project(mavka)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(mama/src)

add_executable(mavka mavka.cpp)
target_link_libraries(mavka PRIVATE mama)
target_compile_options(mavka PRIVATE -fexceptions)
set_target_properties(mavka PROPERTIES OUTPUT_NAME "мавка")
```

```c++
#include "mama/src/mama.h"

using namespace mavka::mama;

MaCell TakePath(MaMa* M,
                const std::string& raw_path,
                const MaLocation& location) {
  const auto canonical_path = std::filesystem::weakly_canonical(raw_path);
  const auto path = canonical_path.string();
  if (!std::filesystem::exists(canonical_path)) {
    return MaCell::Error(MaError::Create(
        M, "Шлях \"" + canonical_path.string() + "\" не існує.", location));
  }
  if (!std::filesystem::is_regular_file(canonical_path)) {
    return MaCell::Error(MaError::Create(
        M, "Шлях \"" + path + "\" не вказує на файл.", location));
  }

  if (M->loaded_file_modules.contains(path)) {
    return MaCell::Object(M->loaded_file_modules[path]);
  }

  auto file = std::ifstream(path);
  if (!file.is_open()) {
    return MaCell::Error(MaError::Create(
        M, "Не вдалося прочитати файл \"" + path + "\".", location));
  }

  const auto fs_path = std::filesystem::path(path);
  const auto name = fs_path.stem().string();

  const auto source = std::string(std::istreambuf_iterator(file),
                                  std::istreambuf_iterator<char>());

  return M->DoTake(path, name, source, location);
}

MaCell TakeFn(MaMa* M,
              const std::string& repository,
              bool relative,
              const std::vector<std::string>& parts,
              const MaLocation& location) {
  if (!repository.empty()) {
    return MaCell::Error(
        MaError::Create(M, "Не підтримується взяття з репозиторію.", location));
  }
  if (relative) {
    return MaCell::Error(MaError::Create(
        M, "Не підтримується взяття відносного шляху.", location));
  }
  const auto cwd = std::filesystem::current_path();
  const auto raw_path =
      cwd.string() + "/" + mavka::internal::tools::implode(parts, "/") + ".м";
  return TakePath(M, raw_path, location);
}

int main(int argc, char** argv) {
  const auto M = MaMa::Create();
  M->TakeFn = TakeFn;

  const auto take_result = TakePath(M, "./старт.м", {});
  if (take_result.IsError()) {
    std::cerr << cell_to_string(take_result.v.error->value) << std::endl;
    return 1;
  }

  return 0;
}
```