#include "../../mama.h"

namespace mavka::mama {
  MaCompilationResult compile_unary_node(MaMa* M,
                                         MaCode* code,
                                         mavka::ast::ASTValue* ast_value) {
    const auto unary_node = ast_value->data.UnaryNode;
    const auto result = compile_node(M, code, unary_node->value);
    if (result.error) {
      return result;
    }
    const auto location =
        MaLocation(ast_value->start_line, ast_value->start_column, code->path);
    M->locations.push_back(location);
    const auto li = M->locations.size() - 1;
    if (unary_node->op == ast::UNARY_POSITIVE) {
      code->push(MaInstruction::positive(li));
    } else if (unary_node->op == ast::UNARY_NEGATIVE) {
      code->push(MaInstruction::negative(li));
    } else if (unary_node->op == ast::UNARY_NOT) {
      code->push(MaInstruction::not_(li));
    } else if (unary_node->op == ast::UNARY_BITWISE_NOT) {
      code->push(MaInstruction::bnot(li));
    }
    return success();
  }
} // namespace mavka::mama
