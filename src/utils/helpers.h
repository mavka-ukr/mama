#ifndef HELPERS_H
#define HELPERS_H

void print_instruction_with_index(MaCode* code,
                                  int index,
                                  MaInstruction instruction);

std::string ma_number_to_string(const double number);

#endif // HELPERS_H