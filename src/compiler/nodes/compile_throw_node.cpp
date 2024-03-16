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

    const auto location =
        MaLocation(ast_value->start_line, ast_value->start_column, code->path);
    M->locations.push_back(location);
    const auto li = M->locations.size() - 1;

    code->push(MaInstruction::throw_(li));
    return success();
  }
} // namespace mavka::mama