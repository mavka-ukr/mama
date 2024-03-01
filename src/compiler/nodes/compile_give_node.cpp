#include "../../mama.h"

namespace mavka::mama {
  MaCompilationResult compile_give_node(MaMa* M,
                                        MaCode* code,
                                        mavka::ast::ASTValue* ast_value) {
    const auto give_node = ast_value->data.GiveNode;
    for (const auto& element_node : give_node->elements) {
      code->push(MaInstruction::load(element_node->data.GiveElementNode->name));
      if (element_node->data.GiveElementNode->as.empty()) {
        code->push(
            MaInstruction::give(element_node->data.GiveElementNode->name));
      } else {
        code->push(MaInstruction::give(element_node->data.GiveElementNode->as));
      }
    }
    return success();
  }
} // namespace mavka::mama