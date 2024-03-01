#include "../../mama.h"

namespace mavka::mama {
  MaCompilationResult compile_identifier_node(MaMa* M,
                                              MaCode* code,
                                              mavka::ast::ASTValue* ast_value) {
    const auto identifier_node = ast_value->data.IdentifierNode;
    if (identifier_node->name == "пусто") {
      code->push(MaInstruction::empty());
    } else if (identifier_node->name == "так") {
      code->push(MaInstruction::yes());
    } else if (identifier_node->name == "ні") {
      code->push(MaInstruction::no());
    } else {
      code->push(MaInstruction::load(identifier_node->name));
    }
    return success();
  }
} // namespace mavka::mama