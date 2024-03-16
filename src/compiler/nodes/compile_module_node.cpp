#include "../../mama.h"

namespace mavka::mama {
  MaCompilationResult compile_module_node(MaMa* M,
                                          MaCode* code,
                                          mavka::ast::ASTValue* ast_value) {
    const auto module_node = ast_value->data.ModuleNode;
    const auto module_code = new MaCode();
    module_code->path = code->path;
    const auto body_result = compile_body(M, module_code, module_node->body);
    if (body_result.error) {
      return body_result;
    }
    code->push(MaInstruction::module(module_code, module_node->name));
    return success();

//    const auto call_ast_value =
//        mavka::ast::module_ast_value_to_diia_call_ast_value(ast_value, "");
//    const auto assign_node = new mavka::ast::AssignNode();
//    assign_node->name = ast_value->data.ModuleNode->name;
//    assign_node->value = call_ast_value;
//    const auto assign_ast_value =
//        new mavka::ast::ASTValue(mavka::ast::KindAssignNode);
//    assign_ast_value->data.AssignNode = assign_node;
//    assign_ast_value->start_line = ast_value->start_line;
//    assign_ast_value->start_column = ast_value->start_column;
//    assign_ast_value->end_line = ast_value->end_line;
//    assign_ast_value->end_column = ast_value->end_column;
//    return compile_assign_node(M, code, assign_ast_value);
  }
} // namespace mavka::mama