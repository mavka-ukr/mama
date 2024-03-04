cmake_minimum_required(VERSION 3.26)
project(mama)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external/parser)
add_subdirectory(external/ordered-map)

set(MAMA_SRC
        compiler/nodes/compile_list_node.cpp
        compiler/nodes/compile_property_set_node.cpp
        compiler/nodes/compile_assign_node.cpp
        compiler/nodes/compile_binary_node.cpp
        compiler/nodes/compile_break_node.cpp
        compiler/nodes/compile_call_node.cpp
        compiler/nodes/compile_property_get_node.cpp
        compiler/nodes/compile_continue_node.cpp
        compiler/nodes/compile_dictionary_node.cpp
        compiler/nodes/compile_diia_node.cpp
        compiler/nodes/compile_give_node.cpp
        compiler/nodes/compile_god_node.cpp
        compiler/nodes/compile_identifier_node.cpp
        compiler/nodes/compile_if_node.cpp
        compiler/nodes/compile_mockup_diia_node.cpp
        compiler/nodes/compile_mockup_module_node.cpp
        compiler/nodes/compile_mockup_structure_node.cpp
        compiler/nodes/compile_mockup_subject_node.cpp
        compiler/nodes/compile_module_node.cpp
        compiler/nodes/compile_mrm_diia_node.cpp
        compiler/nodes/compile_number_node.cpp
        compiler/nodes/compile_unary_node.cpp
        compiler/nodes/compile_return_node.cpp
        compiler/nodes/compile_string_node.cpp
        compiler/nodes/compile_structure_node.cpp
        compiler/nodes/compile_take_node.cpp
        compiler/nodes/compile_ternary_node.cpp
        compiler/nodes/compile_test_node.cpp
        compiler/nodes/compile_throw_node.cpp
        compiler/nodes/compile_try_node.cpp
        compiler/nodes/compile_type_node.cpp
        compiler/nodes/compile_wait_node.cpp
        compiler/nodes/compile_while_node.cpp
        compiler/nodes/diia.cpp
        compiler/compiler.cpp
        objects/MaDict.cpp
        objects/MaDiia.cpp
        objects/MaList.cpp
        objects/MaLogical.cpp
        objects/MaModule.cpp
        objects/MaNumber.cpp
        objects/MaObject.cpp
        objects/MaText.cpp
        objects/MaStructure.cpp
        utils/chrono.h
        utils/tools.cpp
        utils/tools.h
        utils/helpers.cpp
        utils/helpers.h
        ma_call.cpp
        ma_take.cpp
        MaFrame.h
        MaInstruction.cpp
        MaInstruction.h
        ma_run.cpp
        mama.cpp
        mama.h
        MaObject.h
        MaScope.cpp
        MaScope.h
)

add_library(mama ${MAMA_SRC})
target_link_libraries(mama mavka_parser tsl::ordered_map)
