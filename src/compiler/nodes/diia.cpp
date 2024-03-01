#include "../../mama.h"

namespace mavka::mama {
  MaCompilationResult compile_diia(MaMa* M,
                                   MaCode* code,
                                   bool async,
                                   const std::vector<ast::ASTValue*>& generics,
                                   const std::string& name,
                                   const std::vector<ast::ASTValue*>& params,
                                   const ast::ASTValue* return_types,
                                   const std::vector<ast::ASTValue*>& body) {
    const auto diia_code = new MaCode();
    diia_code->path = code->path;
    const auto body_result = compile_body(M, diia_code, body);
    if (body_result.error) {
      return body_result;
    }
    diia_code->push(MaInstruction::empty());
    diia_code->push(MaInstruction::return_());

    code->push(MaInstruction::diia(diia_code, name));
    for (const auto& param : params) {
      if (param->data.ParamNode->variadic) {
        return error(param, "Варіативні параметри наразі не підтримуються.");
      }
      if (param->data.ParamNode->value) {
        const auto value_result =
            compile_node(M, code, param->data.ParamNode->value);
        if (value_result.error) {
          return value_result;
        }
      } else {
        code->push(MaInstruction::empty());
      }
      code->push(MaInstruction::diiaParam(param->data.ParamNode->name));
    }
    return success();
  }

  MaCompilationResult compile_method(
      MaMa* M,
      MaCode* code,
      const std::string& structure,
      bool ee,
      bool async,
      const std::vector<ast::ASTValue*>& generics,
      const std::string& name,
      const std::vector<ast::ASTValue*>& params,
      const ast::ASTValue* return_types,
      const std::vector<ast::ASTValue*>& body) {
    code->push(MaInstruction::load(structure));
    const auto result = compile_diia(M, code, async, generics, name, params,
                                     return_types, body);
    if (result.error) {
      return result;
    }
    code->push(MaInstruction::structMethod());
    return success();
  }
} // namespace mavka::mama