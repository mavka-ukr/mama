cmake_minimum_required(VERSION 3.26)
project(mama)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src/external/parser)
add_subdirectory(src/external/ordered-map)

set(MAMA_SRC
        src/compiler/nodes/compile_list_node.cpp
        src/compiler/nodes/compile_property_set_node.cpp
        src/compiler/nodes/compile_assign_node.cpp
        src/compiler/nodes/compile_binary_node.cpp
        src/compiler/nodes/compile_break_node.cpp
        src/compiler/nodes/compile_call_node.cpp
        src/compiler/nodes/compile_property_get_node.cpp
        src/compiler/nodes/compile_continue_node.cpp
        src/compiler/nodes/compile_dictionary_node.cpp
        src/compiler/nodes/compile_diia_node.cpp
        src/compiler/nodes/compile_give_node.cpp
        src/compiler/nodes/compile_god_node.cpp
        src/compiler/nodes/compile_identifier_node.cpp
        src/compiler/nodes/compile_if_node.cpp
        src/compiler/nodes/compile_mockup_diia_node.cpp
        src/compiler/nodes/compile_mockup_module_node.cpp
        src/compiler/nodes/compile_mockup_structure_node.cpp
        src/compiler/nodes/compile_mockup_subject_node.cpp
        src/compiler/nodes/compile_module_node.cpp
        src/compiler/nodes/compile_mrm_diia_node.cpp
        src/compiler/nodes/compile_number_node.cpp
        src/compiler/nodes/compile_unary_node.cpp
        src/compiler/nodes/compile_return_node.cpp
        src/compiler/nodes/compile_string_node.cpp
        src/compiler/nodes/compile_structure_node.cpp
        src/compiler/nodes/compile_take_node.cpp
        src/compiler/nodes/compile_ternary_node.cpp
        src/compiler/nodes/compile_test_node.cpp
        src/compiler/nodes/compile_throw_node.cpp
        src/compiler/nodes/compile_try_node.cpp
        src/compiler/nodes/compile_type_node.cpp
        src/compiler/nodes/compile_wait_node.cpp
        src/compiler/nodes/compile_while_node.cpp
        src/compiler/nodes/diia.cpp
        src/compiler/compiler.cpp
        src/objects/MaBytes.cpp
        src/objects/MaDict.cpp
        src/objects/MaDiia.cpp
        src/objects/MaList.cpp
        src/objects/MaLogical.cpp
        src/objects/MaModule.cpp
        src/objects/MaNumber.cpp
        src/objects/MaObject.cpp
        src/objects/MaText.cpp
        src/objects/MaStructure.cpp
        src/utils/chrono.h
        src/utils/tools.cpp
        src/utils/tools.h
        src/utils/helpers.cpp
        src/utils/helpers.h
        src/MaArgs.cpp
        src/MaValue.cpp
        src/MaFrame.h
        src/MaInstruction.cpp
        src/MaInstruction.h
        src/MaMa.cpp
        src/mama.h
        src/MaObject.h
        src/MaScope.cpp
        src/MaScope.h
)

add_library(mama ${MAMA_SRC})
target_link_libraries(mama mavka_parser tsl::ordered_map)

if (MAMA_TESTING)
    add_executable(mavka testing/mavka.cpp)
    target_link_libraries(mavka PRIVATE mama)
    target_compile_options(mavka PRIVATE -fexceptions)
    set_target_properties(mavka PROPERTIES OUTPUT_NAME "мавка")
endif ()