#include "../../mama.h"

namespace mavka::mama {
  MaCompilationResult compile_take_node(MaMa* M,
                                        MaCode* code,
                                        mavka::ast::ASTValue* ast_value) {
    const auto take_node = ast_value->data.TakeNode;
    const auto parts = mavka::internal::tools::explode(take_node->name, ".");

    code->push(MaInstruction::take(take_node->repo, parts));

    if (take_node->elements.empty()) {
      if (take_node->all_elements) {
        code->push(MaInstruction::eFetchAll());
      } else {
        if (take_node->as.empty()) {
          code->push(MaInstruction::store(parts.back()));
        } else {
          code->push(MaInstruction::store(take_node->as));
        }
      }
    } else {
      for (const auto& element : take_node->elements) {
        code->push(MaInstruction::eGet(element.first));
        code->push(MaInstruction::store(element.second));
      }
    }
    return success();
  }
} // namespace mavka::mama