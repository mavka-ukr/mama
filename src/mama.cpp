#include "mama.h"

namespace mavka::mama {
  MaMa* MaMa::Create() {
    const auto M = new MaMa();
    M->global_scope = new MaScope(nullptr);
    MaStructure::Init(M);
    MaObject::Init(M);
    MaDiia::Init(M);
    MaModule::Init(M);
    InitNumber(M);
    InitLogical(M);
    MaText::Init(M);
    MaList::Init(M);
    MaDict::Init(M);
    MaStructure::Init2(M);
    const auto main_module_object = MaModule::Create(M, "мавка");
    const auto main_frame =
        new MaFrame(M->global_scope, main_module_object, main_module_object);
    FRAME_PUSH(main_frame);
    return M;
  }

  MaCell MaMa::Take(const std::string& repository,
                    bool relative,
                    const std::vector<std::string>& path_parts) {
    const auto M = this;
    const auto path =
        M->cwd + "/" + internal::tools::implode(path_parts, "/") + ".м";
    if (!std::filesystem::exists(path)) {
      DO_RETURN_STRING_ERROR("Не вдалося прочитати файл \"" + path + "\".");
    }
    return this->Take(path);
  }

  MaCell MaMa::Take(const std::string& path) {
    const auto canonical_path = std::filesystem::canonical(path).string();
    const auto M = this;

    const auto fs_path = std::filesystem::path(canonical_path);
    if (!fs_path.has_filename()) {
      DO_RETURN_STRING_ERROR("Не вдалося прочитати файл \"" + canonical_path +
                             "\".");
    }

    const auto name = fs_path.stem().string();

    if (M->loaded_file_modules.contains(canonical_path)) {
      return MA_MAKE_OBJECT(M->loaded_file_modules[canonical_path]);
    }

    auto file = std::ifstream(canonical_path);
    if (!file.is_open()) {
      DO_RETURN_STRING_ERROR("Не вдалося прочитати файл \"" + canonical_path +
                             "\".");
    }

    const auto source = std::string(std::istreambuf_iterator(file),
                                    std::istreambuf_iterator<char>());

    const auto parser_result = parser::parse(source, canonical_path);
    if (!parser_result.errors.empty()) {
      const auto error = parser_result.errors[0];
      DO_RETURN_STRING_ERROR(error.path + ":" + std::to_string(error.line) +
                             ":" + std::to_string(error.column) + ": " +
                             error.message);
    }

    const auto module_code = new MaCode();
    module_code->path = canonical_path;

    const auto module_object = MaModule::Create(M, name);
    module_object->d.module->code = module_code;
    module_object->d.module->is_file_module = true;
    if (M->main_module == nullptr) {
      M->main_module = module_object;
    }
    M->loaded_file_modules.insert_or_assign(canonical_path, module_object);

    const auto body_compilation_result =
        compile_body(M, module_code, parser_result.module_node->body);
    if (body_compilation_result.error) {
      DO_RETURN_STRING_ERROR(
          canonical_path + ":" +
          std::to_string(body_compilation_result.error->line) + ":" +
          std::to_string(body_compilation_result.error->column) + ": " +
          body_compilation_result.error->message);
    }

    READ_TOP_FRAME();
    const auto module_scope = new MaScope(frame->scope);
    const auto module_frame =
        new MaFrame(module_scope, module_object, module_object);
    FRAME_PUSH(module_frame);
    const auto result = ma_run(M, module_code);
    if (IS_ERROR(result)) {
      return result;
    }
    FRAME_POP();
    return MA_MAKE_OBJECT(module_object);
  }

} // namespace mavka::mama