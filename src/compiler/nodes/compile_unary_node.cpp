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
    if (unary_node->op == ast::UNARY_POSITIVE) {
      code->push(MaInstruction::positive());
    } else if (unary_node->op == ast::UNARY_NEGATIVE) {
      code->push(MaInstruction::negative());
    } else if (unary_node->op == ast::UNARY_NOT) {
      code->push(MaInstruction::not_());
    } else if (unary_node->op == ast::UNARY_BITWISE_NOT) {
      code->push(MaInstruction::bnot());
    }
    return success();
  }
} // namespace mavka::mama
