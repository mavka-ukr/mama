#ifndef HELPERS_H
#define HELPERS_H

std::string cell_to_string(MaCell cell, int depth = 0);

void print_instruction_with_index(MaCode* code,
                                  int index,
                                  MaInstruction instruction);

#endif // HELPERS_H