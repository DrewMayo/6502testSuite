#include "test.h"
#include "cpu.h"
#include <stdbool.h>
#include <stdint.h>

uint8_t bus_read(const struct Bus *const bus, uint16_t addr,
                 enum Memtype memtype) {
  (void)memtype;
  return bus->memory[addr];
}

void bus_write(struct Bus *const bus, uint16_t addr, uint8_t value,
               enum Memtype memtype) {
  (void)memtype;
  bus->memory[addr] = value;
}

void testCpuPart(const struct cpu_6502 cpu, const struct instruction instr) {
  printf("%04X  ", cpu.PC & 0xFFFF);
  printf("%02X ", bus_read(cpu.bus, cpu.PC, CPUMEM) & 0xFF);
  switch (instr.addr_mode) {
  case IMPLIED:
  case ACCUMULATOR:
    printf("     ");
    break;
  case IMMEDIATE:
  case ZEROPAGE:
  case ZEROPAGEX:
  case INDIRECTX:
  case INDIRECTY:
  case ZEROPAGEY:
  case RELATIVE:
    printf("%02X   ", bus_read(cpu.bus, cpu.PC + 1, CPUMEM) & 0xFF);
    break;
  case ABSOLUTE:
  case ABSOLUTEX:
  case ABSOLUTEY:
  case INDIRECT:
    printf("%02X %02X", bus_read(cpu.bus, cpu.PC + 1, CPUMEM) & 0xFF,
           bus_read(cpu.bus, cpu.PC + 2, CPUMEM) & 0xFF);
    break;
  }
  printf("  ");
  printf("%s ", instr.name);
  printf("A:%02X X:%02X Y:%02X P:%02X SP:%02X PPU:%3d,%3d CYC:%d\n", cpu.AC,
         cpu.X, cpu.Y, cpu_combine_SR(cpu.SR), cpu.SP, 0, 0, cpu.cycles);
  // printf("A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%d\n", cpu.AC, cpu.X,
  // cpu.Y, cpu_combine_SR(cpu.SR), cpu.SP,
  //        cpu.cycles);
}
