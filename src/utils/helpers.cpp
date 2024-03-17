#include "../mama.h"

namespace mavka::mama {
  std::string MaValue::getName() const {
    if (this->isObject()) {
      if (this->v.object->getStructure()) {
        return this->v.object->getStructure()->structureGetName();
      }
    }
    switch (this->type) {
      case MaValueTypeEmpty:
        return "пусто";
      case MaValueTypeNumber:
        return "число";
      case MaValueTypeYes:
        return "логічне";
      case MaValueTypeNo:
        return "логічне";
      case MaValueTypeObject:
        return "обʼєкт";
      case MaValueTypeError:
        return "помилка";
      default:
        break;
    }
    return std::to_string(this->type);
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
