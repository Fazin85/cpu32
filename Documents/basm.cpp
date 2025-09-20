#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

void writeLogisimInstructions(const std::string &filename,
                              const std::vector<uint32_t> &instructions) {
  std::ofstream file(filename);
  file << "v2.0 raw" << std::endl;

  for (size_t i = 0; i < instructions.size(); ++i) {
    // Write as 8-digit hex (32 bits)
    file << std::hex << std::setfill('0') << std::setw(8) << instructions[i];

    // 4 instructions per line for readability
    if ((i + 1) % 4 == 0) {
      file << std::endl;
    } else {
      file << " ";
    }
  }
  file << std::endl;
}

uint32_t genInstruction(uint8_t opcode, uint8_t reg1, uint8_t reg2,
                        uint16_t imm) {
  uint32_t instruction = 0;
  instruction |= (uint32_t)opcode;             // Bits 7-0
  instruction |= ((uint32_t)reg1 & 0xF) << 8;  // Bits 11-8 (mask to 4 bits)
  instruction |= ((uint32_t)reg2 & 0xF) << 12; // Bits 15-12 (mask to 4 bits)
  instruction |= (uint32_t)imm << 16;          // Bits 31-16
  return instruction;
}

std::vector<uint32_t> instructions;

void gi(uint8_t o, uint8_t r1, uint8_t r2, uint16_t i) {
  instructions.emplace_back(genInstruction(o, r1, r2, i));
}

#define NOP 0
#define ADD 1
#define SUB 2
#define LW 3
#define SW 4
#define LOADI 5
#define JNZ 6
#define ADC 7
#define SBB 8
#define LUI 9
#define ORI 10
#define LH 11
#define LHU 12
#define LB 13
#define LBU 14
#define SH 15
#define SB 16

int main() {
  gi(LOADI, 1, 0, 6);  // LOADI R1, #6
  gi(LOADI, 2, 0, 67); // LOADI R2, #67

  // Add them together (result goes to R0)
  gi(ADD, 1, 2, 6); // ADD R1, R2, 6    (R0 = R0 + R1 = 6 + 6 + 67 = 79)

  // Store the result to address 2
  gi(SW, 1, 0, 4); // STORE [R0], R1, 4
  gi(LOADI, 2, 0, 15);
  gi(LOADI, 3, 0, 15);
  gi(SUB, 2, 3, 0);

  gi(LW, 13, 0, 4);
  gi(JNZ, 0, 0, 0);

  // gi(LUI, 1, 1, 0x1234); // R0 = 0x12340000
  // gi(ORI, 1, 1, 0x5678); // R0 = 0x12345678
  gi(LUI, 1, 1, 0x7fff);
  gi(ORI, 1, 1, 0xffff);
  gi(SW, 1, 0, 4);
  gi(LW, 5, 0, 4);
  gi(LBU, 2, 0, 4);
  gi(LBU, 3, 0, 5);
  gi(LHU, 4, 0, 6);
  writeLogisimInstructions("program.rom", instructions);
}
