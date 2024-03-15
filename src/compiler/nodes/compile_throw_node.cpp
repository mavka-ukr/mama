#include "../../mama.h"

namespace mavka::mama {
  MaCompilationResult compile_throw_node(MaMa* M,
                                         MaCode* code,
                                         mavka::ast::ASTValue* ast_value) {
    const auto throw_node = ast_value->data.ThrowNode;
    const auto result = compile_node(M, code, throw_node->value);
    if (result.error) {
      return result;
    }
    const auto location = MaLocation{.line = ast_value->start_line,
                                     .column = ast_value->start_column};
    code->push(MaInstruction::throw_(location));
    return success();
  }
} // namespace mavka::mama