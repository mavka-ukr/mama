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

    const auto args_type = call_node->args.empty()
                               ? MA_ARGS_TYPE_POSITIONED
                               : (call_node->args[0]->data.ArgNode->name.empty()
                                      ? MA_ARGS_TYPE_POSITIONED
                                      : MA_ARGS_TYPE_NAMED);

    const auto instruction_location =
        MaLocation(ast_value->start_line, ast_value->start_column);

    code->push(MaInstruction::args(args_type));

    for (const auto& arg : call_node->args) {
      const auto arg_result = compile_node(M, code, arg->data.ArgNode->value);
      if (arg_result.error) {
        return arg_result;
      }
      if (arg->data.ArgNode->name.empty()) {
        code->push(MaInstruction::pushArg());
      } else {
        code->push(MaInstruction::storeArg(arg->data.ArgNode->name));
      }
    }

    code->push(MaInstruction::call(instruction_location));

    return success();
  }
} // namespace mavka::mama
