#include "../../mama.h"

namespace mavka::mama {
  MaCompilationResult compile_binary_node(MaMa* M,
                                          MaCode* code,
                                          mavka::ast::ASTValue* ast_value) {
    const auto binary_node = ast_value->data.BinaryNode;
    const auto left = compile_node(M, code, binary_node->left);
    if (left.error) {
      return left;
    }
    const auto right = compile_node(M, code, binary_node->right);
    if (right.error) {
      return right;
    }
    const auto instruction_location =
        MaLocation(ast_value->start_line, ast_value->start_column);
    if (binary_node->op == ast::ARITHMETIC_ADD) {
      code->push(MaInstruction::add(instruction_location));
    } else if (binary_node->op == ast::ARITHMETIC_SUB) {
      code->push(MaInstruction::sub(instruction_location));
    } else if (binary_node->op == ast::ARITHMETIC_MUL) {
      code->push(MaInstruction::mul(instruction_location));
    } else if (binary_node->op == ast::ARITHMETIC_DIV) {
      code->push(MaInstruction::div(instruction_location));
    } else if (binary_node->op == ast::ARITHMETIC_MOD) {
      code->push(MaInstruction::mod(instruction_location));
    } else if (binary_node->op == ast::ARITHMETIC_DIVDIV) {
      code->push(MaInstruction::divdiv(instruction_location));
    } else if (binary_node->op == ast::ARITHMETIC_POW) {
      code->push(MaInstruction::pow(instruction_location));
    } else if (binary_node->op == ast::BITWISE_XOR) {
      code->push(MaInstruction::xor_());
    } else if (binary_node->op == ast::BITWISE_OR) {
      code->push(MaInstruction::bor());
    } else if (binary_node->op == ast::BITWISE_AND) {
      code->push(MaInstruction::band());
    } else if (binary_node->op == ast::BITWISE_SHIFT_LEFT) {
      code->push(MaInstruction::shl());
    } else if (binary_node->op == ast::BITWISE_SHIFT_RIGHT) {
      code->push(MaInstruction::shr());
    } else if (binary_node->op == ast::COMPARISON_EQ) {
      code->push(MaInstruction::eq());
    } else if (binary_node->op == ast::COMPARISON_NE) {
      code->push(MaInstruction::eq());
      code->push(MaInstruction::not_());
    } else if (binary_node->op == ast::COMPARISON_GT) {
      code->push(MaInstruction::gt());
    } else if (binary_node->op == ast::COMPARISON_GE) {
      code->push(MaInstruction::ge());
    } else if (binary_node->op == ast::COMPARISON_LT) {
      code->push(MaInstruction::lt());
    } else if (binary_node->op == ast::COMPARISON_LE) {
      code->push(MaInstruction::le());
    } else if (binary_node->op == ast::COMPARISON_CONTAINS) {
      code->push(MaInstruction::contains());
    } else if (binary_node->op == ast::COMPARISON_NOT_CONTAINS) {
      code->push(MaInstruction::contains());
      code->push(MaInstruction::not_());
    } else if (binary_node->op == ast::COMPARISON_IS) {
      code->push(MaInstruction::is());
    } else if (binary_node->op == ast::COMPARISON_NOT_IS) {
      code->push(MaInstruction::is());
      code->push(MaInstruction::not_());
    } else if (binary_node->op == ast::UTIL_AS) {
      return error(ast_value, "Вказівка \"як\" тимчасово недоступна.");
    }
    return success();
  }
} // namespace mavka::mama
