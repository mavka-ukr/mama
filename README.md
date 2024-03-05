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

using namespace mavka;

int main(int argc, char** argv) {
  const auto M = mama::MaMa::Create();

  const auto take_result = mama::ma_take(M, "./старт.м");
  if (IS_ERROR(take_result)) {
    std::cerr << mama::cell_to_string(take_result.v.error->value) << std::endl;
    M->Destroy();
    return 1;
  }

  M->Destroy();
  return 0;
}
```