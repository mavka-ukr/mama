#include "mama.h"

namespace mavka::mama {
  MaValue maTakeFsPath(MaMa* M,
                       const std::string& raw_path,
                       bool root,
                       size_t li) {
    const auto canonical_path = std::filesystem::weakly_canonical(raw_path);
    const auto path = canonical_path.string();
    if (!std::filesystem::exists(canonical_path)) {
      return MaValue::Error(MaError::Create(
          M, "Шлях \"" + canonical_path.string() + "\" не існує.", li));
    }
    if (!std::filesystem::is_regular_file(canonical_path)) {
      return MaValue::Error(
          MaError::Create(M, "Шлях \"" + path + "\" не вказує на файл.", li));
    }

    if (M->loaded_file_modules.contains(path)) {
      return MaValue::Object(M->loaded_file_modules[path]);
    }

    auto file = std::ifstream(path);
    if (!file.is_open()) {
      return MaValue::Error(MaError::Create(
          M, "Не вдалося прочитати файл \"" + path + "\".", li));
    }

    const auto fs_path = std::filesystem::path(path);
    const auto name = fs_path.stem().string();

    const auto source = std::string(std::istreambuf_iterator(file),
                                    std::istreambuf_iterator<char>());

    return M->takeSource(path, name, source, root, li);
  }

  MaValue maTakeFsFn(MaMa* M,
                     const std::string& repository,
                     bool relative,
                     const std::vector<std::string>& parts,
                     size_t li) {
    const auto currentModule = M->call_stack.top()->getModule();
    const auto currentModulePath =
        currentModule->asModule()->getCode()->getPath();
    const auto currentModuleDir =
        std::filesystem::path(currentModulePath).parent_path();
    const auto rootModule = currentModule->asModule()->getRoot();
    const auto rootModulePath = rootModule->asModule()->getCode()->getPath();
    const auto rootModuleDir =
        std::filesystem::path(rootModulePath).parent_path();
    if (!repository.empty()) {
      const auto mainModule = M->main_module;
      const auto mainModulePath = mainModule->asModule()->getCode()->getPath();
      const auto mainModuleDir =
          std::filesystem::path(mainModulePath).parent_path();
      const auto path = mainModuleDir.string() + "/.паки/" + parts[0] + "/" +
                        mavka::internal::tools::implode(parts, "/") + ".м";
      return maTakeFsPath(M, path, true, li);
    }
    std::string takeDir;
    if (relative) {
      takeDir = currentModuleDir.string();
    } else {
      takeDir = rootModuleDir.string();
    }
    const auto path =
        takeDir + "/" + mavka::internal::tools::implode(parts, "/") + ".м";
    return maTakeFsPath(M, path, false, li);
  }
} // namespace mavka::mama