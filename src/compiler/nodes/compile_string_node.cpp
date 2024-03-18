#include "../../mama.h"

namespace mavka::mama {
  MaCompilationResult compile_string_node(MaMa* M,
                                          MaCode* code,
                                          mavka::ast::ASTValue* ast_value) {
    const auto string_node = ast_value->data.StringNode;
    const auto stringObject = M->createText( string_node->value);
    stringObject->retain();
    M->constants.push_back(stringObject);
    code->push(MaInstruction::constant(M->constants.size() - 1));
    return success();
  }
} // namespace mavka::mama