<img src="./assets/cover.png" /> 

# МаМа

Машина вказівок Мавки.

## Вимоги

- Clang
- CMake
- Make

## Використання

Клон репо:

```shell
git clone https://github.com/mavka-ukr/mama
cd mama
git submodule update --init --recursive
cd ..
```

CMakeLists.txt:

```CMake
cmake_minimum_required(VERSION 3.26)
project(mavka)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(mama)

add_executable(mavka mavka.cpp)
target_link_libraries(mavka PRIVATE mama)
target_compile_options(mavka PRIVATE -fexceptions)
set_target_properties(mavka PROPERTIES OUTPUT_NAME "мавка")
```

mavka.cpp:

```c++
#include "mama/src/mama.h"

using namespace mavka::mama;

MaValue TakePath(MaMa* M,
                 const std::string& raw_path,
                 size_t li) {
  const auto canonical_path = std::filesystem::weakly_canonical(raw_path);
  const auto path = canonical_path.string();
  if (!std::filesystem::exists(canonical_path)) {
    return MaValue::Error(
        MaError::Create(M, "Шлях \"" + path + "\" не існує.", location));
  }
  if (!std::filesystem::is_regular_file(canonical_path)) {
    return MaValue::Error(MaError::Create(
        M, "Шлях \"" + path + "\" не вказує на файл.", location));
  }

  if (M->loaded_file_modules.contains(path)) {
    return MaValue::Object(M->loaded_file_modules[path]);
  }

  auto file = std::ifstream(path);
  if (!file.is_open()) {
    return MaValue::Error(MaError::Create(
        M, "Не вдалося прочитати файл \"" + path + "\".", location));
  }

  const auto fs_path = std::filesystem::path(path);
  const auto name = fs_path.stem().string();

  const auto source = std::string(std::istreambuf_iterator(file),
                                  std::istreambuf_iterator<char>());

  return M->DoTake(path, name, source, location);
}

MaValue take_fn(MaMa* M,
                const std::string& repository,
                bool relative,
                const std::vector<std::string>& parts,
                size_t li) {
  if (!repository.empty()) {
    return MaValue::Error(
        MaError::Create(M, "Не підтримується взяття з репозиторію.", location));
  }
  if (relative) {
    return MaValue::Error(MaError::Create(
        M, "Не підтримується взяття відносного шляху.", location));
  }
  const auto cwd = std::filesystem::current_path();
  const auto raw_path =
      cwd.string() + "/" + mavka::internal::tools::implode(parts, "/") + ".м";
  return TakePath(M, raw_path, location);
}

int main(int argc, char** argv) {
  const auto M = MaMa::Create();
  M->take_fn = take_fn;

  const auto take_result = TakePath(M, "./старт.м", {});
  if (take_result.isError()) {
    std::cerr << cell_to_string(take_result.v.error->value) << std::endl;
    return 1;
  }

  return 0;
}
```

## Розробка

Клон репо:

```shell
git clone https://github.com/mavka-ukr/mama
cd mama
git submodule update --init --recursive
```

Будування Мавки для тестування:

```shell
bash testing/build.sh
```

Використання Мавки для тестування:

```
./build/мавка testing/старт.м
```

Запуск тестів:

```
bash testing/test.sh
```