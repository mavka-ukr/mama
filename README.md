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

int main(int argc, char **argv) {
    const auto M = new mavka::mama::MaMa();

    return 0;
}
```