#pragma once
#ifndef TEST_H
#define TEST_H
#include "cpu.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
int run_unit_tests();
void testCpuPart(const struct cpu_6502 cpu, const struct instruction instr);

struct Bus {
  uint8_t memory[65536];
};

enum Memtype { CPUMEM };


uint8_t bus_read(const struct Bus *const bus, uint16_t addr, enum Memtype memtype);
void bus_write(struct Bus *const bus, uint16_t addr, uint8_t value, enum Memtype memtype);
#endif
