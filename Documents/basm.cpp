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
#define LOAD 3
#define STORE 4
#define LOADI 5
#define JNZ 6
#define ADC 7
#define SBB 8
#define LUI 9
#define ORI 10

int main() {
  gi(LOADI, 0, 0, 6);  // LOADI R0, #6
  gi(LOADI, 1, 0, 67); // LOADI R1, #67

  // Add them together (result goes to R0)
  gi(ADD, 0, 1, 6); // ADD R0, R1    (R0 = R0 + R1 = 6 + 67 = 73)

  // Load address 2 into a register for storing
  gi(LOADI, 2, 0, 2); // LOADI R2, #2

  // Store the result to address 2
  gi(STORE, 2, 0, 0); // STORE [R2], R0
  gi(LOADI, 1, 0, 15);
  gi(LOADI, 2, 0, 15);
  gi(SUB, 1, 2, 0);

  gi(JNZ, 0, 0, 0);

  gi(LUI, 0, 0, 0x1234); // R0 = 0x12340000
  gi(ORI, 0, 0, 0x5678); // R0 = 0x12345678

  writeLogisimInstructions("program.rom", instructions);
}
