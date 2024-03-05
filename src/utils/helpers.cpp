#include "../mama.h"

namespace mavka::mama {
  std::string MaCell::GetName() const {
    if (type == MA_CELL_OBJECT) {
      if (v.object->structure) {
        return v.object->structure->d.structure->name;
      }
    }
    switch (type) {
      case MA_CELL_EMPTY:
        return "пусто";
      case MA_CELL_NUMBER:
        return "число";
      case MA_CELL_YES:
        return "логічне";
      case MA_CELL_NO:
        return "логічне";
      case MA_CELL_OBJECT:
        return "обʼєкт";
      case MA_CELL_ARGS:
        return "аргументи";
      default:
        break;
    }
    return std::to_string(type);
  }

  void print_instruction_with_index(MaCode* code,
                                    int index,
                                    MaInstruction instruction) {
    std::cout << code << "[" << index << "]: " << instruction.ToString()
              << std::endl;
  }

  std::string ma_number_to_string(const double number) {
    std::ostringstream stream;
    stream << number;
    return stream.str();
  }
} // namespace mavka::mama
