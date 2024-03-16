#include "../../mama.h"

namespace mavka::mama {
  MaCompilationResult compile_call_node(MaMa* M,
                                        MaCode* code,
                                        mavka::ast::ASTValue* ast_value) {
    const auto call_node = ast_value->data.CallNode;
    const auto result = compile_node(M, code, call_node->value);
    if (result.error) {
      return result;
    }

    const auto instruction_location =
        MaLocation(ast_value->start_line, ast_value->start_column);

    code->push(MaInstruction::args());

    for (const auto& arg : call_node->args) {
      const auto arg_result = compile_node(M, code, arg->data.ArgNode->value);
      if (arg_result.error) {
        return arg_result;
      }
      if (arg->data.ArgNode->name.empty()) {
        code->push(
            MaInstruction::storeArg(std::to_string(arg->data.ArgNode->index)));
      } else {
        code->push(MaInstruction::storeArg(arg->data.ArgNode->name));
      }
    }

    code->push(MaInstruction::call(instruction_location));

    return success();
  }
} // namespace mavka::mama
