#include "../../mama.h"

namespace mavka::mama {
  MaCompilationResult compile_string_node(MaMa* M,
                                          MaCode* code,
                                          mavka::ast::ASTValue* ast_value) {
    const auto string_node = ast_value->data.StringNode;
    M->constants.push_back(
        MA_MAKE_OBJECT(MaText::Create(M, string_node->value)));
    code->push(MaInstruction::constant(M->constants.size() - 1));
    return success();
  }
} // namespace mavka::mama