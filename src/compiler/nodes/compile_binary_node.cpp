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
    const auto location =
        MaLocation(ast_value->start_line, ast_value->start_column, code->path);
    M->locations.push_back(location);
    const auto li = M->locations.size() - 1;

    if (binary_node->op == ast::ARITHMETIC_ADD) {
      code->push(MaInstruction::add(li));
    } else if (binary_node->op == ast::ARITHMETIC_SUB) {
      code->push(MaInstruction::sub(li));
    } else if (binary_node->op == ast::ARITHMETIC_MUL) {
      code->push(MaInstruction::mul(li));
    } else if (binary_node->op == ast::ARITHMETIC_DIV) {
      code->push(MaInstruction::div(li));
    } else if (binary_node->op == ast::ARITHMETIC_MOD) {
      code->push(MaInstruction::mod(li));
    } else if (binary_node->op == ast::ARITHMETIC_DIVDIV) {
      code->push(MaInstruction::divdiv(li));
    } else if (binary_node->op == ast::ARITHMETIC_POW) {
      code->push(MaInstruction::pow(li));
    } else if (binary_node->op == ast::BITWISE_XOR) {
      code->push(MaInstruction::xor_(li));
    } else if (binary_node->op == ast::BITWISE_OR) {
      code->push(MaInstruction::bor(li));
    } else if (binary_node->op == ast::BITWISE_AND) {
      code->push(MaInstruction::band(li));
    } else if (binary_node->op == ast::BITWISE_SHIFT_LEFT) {
      code->push(MaInstruction::shl(li));
    } else if (binary_node->op == ast::BITWISE_SHIFT_RIGHT) {
      code->push(MaInstruction::shr(li));
    } else if (binary_node->op == ast::COMPARISON_EQ) {
      code->push(MaInstruction::eq(li));
    } else if (binary_node->op == ast::COMPARISON_NE) {
      code->push(MaInstruction::eq(li));
      code->push(MaInstruction::not_(li));
    } else if (binary_node->op == ast::COMPARISON_GT) {
      code->push(MaInstruction::gt(li));
    } else if (binary_node->op == ast::COMPARISON_GE) {
      code->push(MaInstruction::ge(li));
    } else if (binary_node->op == ast::COMPARISON_LT) {
      code->push(MaInstruction::lt(li));
    } else if (binary_node->op == ast::COMPARISON_LE) {
      code->push(MaInstruction::le(li));
    } else if (binary_node->op == ast::COMPARISON_CONTAINS) {
      code->push(MaInstruction::contains(li));
    } else if (binary_node->op == ast::COMPARISON_NOT_CONTAINS) {
      code->push(MaInstruction::contains(li));
      code->push(MaInstruction::not_(li));
    } else if (binary_node->op == ast::COMPARISON_IS) {
      code->push(MaInstruction::is(li));
    } else if (binary_node->op == ast::COMPARISON_NOT_IS) {
      code->push(MaInstruction::is(li));
      code->push(MaInstruction::not_(li));
    } else if (binary_node->op == ast::UTIL_AS) {
      return error(ast_value, "Вказівка \"як\" тимчасово недоступна.");
    }
    return success();
  }
} // namespace mavka::mama
