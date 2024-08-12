#include "cpu.h"
#include "test.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <cjson/cJSON.h>
#include <stdint.h>
#include <stdio.h>
void run_test_file(struct cpu_6502 *cpu, char *filename);

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  struct cpu_6502 *cpu = cpu_build();
  struct Bus bus;
  for (int i = 0; i < 0x10000; i++) {
    bus.memory[i] = 0;
  }
  cpu->bus = &bus;
  char filename[13];
  for (int i = 170; i < 0x100; i++) {
    if (i < 0x10) {
      sprintf(filename, "roms/0%x.json", i);
    } else {
      sprintf(filename, "roms/%x.json", i);
    }
    run_test_file(cpu, filename);
  }
}

void run_test_file(struct cpu_6502 *cpu, char *filename) {
  // open file
  int file_size = 0;
  char *buffer;
  cJSON *json;
  cJSON *item;
  FILE *fp = fopen(filename, "r");
  // make sure the file exists
  if (fp == NULL) {
    printf("ERROR OPENING FILE NAME: %s", filename);
    return;
  }

  // get file size
  fseek(fp, 0, SEEK_END);
  file_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  // read into buffer
  buffer = (char *)malloc(file_size + 1);
  fread(buffer, 1, file_size, fp);
  buffer[file_size] = '\0';

  // Close file
  fclose(fp);

  // Cjson parsing
  json = cJSON_Parse(buffer);
  cJSON *name;
  cJSON *initial;
  cJSON *final;
  cJSON *iram;
  cJSON *iram_index;
  cJSON *fram;
  cJSON *fram_index;
  char *name_s;

  cJSON_ArrayForEach(item, json) {
    cpu_reset(cpu);
    name = cJSON_GetObjectItemCaseSensitive(item, "name");
    name_s = cJSON_Print(name);
    initial = cJSON_GetObjectItemCaseSensitive(item, "initial");
    final = cJSON_GetObjectItemCaseSensitive(item, "final");
    // set initial state
    cpu->PC = cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(initial, "pc"));
    cpu->X = cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(initial, "x"));
    cpu->Y = cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(initial, "y"));
    cpu->AC = cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(initial, "a"));
    cpu->SP = cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(initial, "s"));
    cpu_expand_SR(cpu, cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(initial, "p")));
    // print initial cpu state
    printf("\n");
    printf("Name: %s\n", name_s);
    printf("Initial cpu -- PC: %X, X: %X, Y: %X, AC: %X, SP: %X, SR: %X\n", cpu->PC, cpu->X, cpu->Y, cpu->AC, cpu->SP,
           cpu_combine_SR(cpu->SR));

    // set ram initial state
    iram = cJSON_GetObjectItemCaseSensitive(initial, "ram");
    printf("Initial ram -- ");
    cJSON_ArrayForEach(iram_index, iram) {
      printf("[%X, %X]", (uint16_t)cJSON_GetNumberValue(cJSON_GetArrayItem(iram_index, 0)),
             (uint16_t)cJSON_GetNumberValue(cJSON_GetArrayItem(iram_index, 1)));
      bus_write(cpu->bus, (uint16_t)cJSON_GetNumberValue(cJSON_GetArrayItem(iram_index, 0)),
                (uint8_t)cJSON_GetNumberValue(cJSON_GetArrayItem(iram_index, 1)), CPUMEM);
    }
    printf("\n\n");
    printf("Final cpu expected -- PC: %X, X: %X, Y: %X, AC: %X, SP: %X, SR: %X\n",
           (int)cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(final, "pc")),
           (int)cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(final, "x")),
           (int)cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(final, "y")),
           (int)cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(final, "a")),
           (int)cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(final, "s")),
           (int)cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(final, "p")));
    fram = cJSON_GetObjectItemCaseSensitive(final, "ram");
    printf("Final ram expected -- ");
    cJSON_ArrayForEach(fram_index, fram) {
      printf("[%X, %X]", (int)cJSON_GetNumberValue(cJSON_GetArrayItem(fram_index, 0)),
             (int)cJSON_GetNumberValue(cJSON_GetArrayItem(fram_index, 1)));
    }
    printf("\n\n");
    cpu_run(cpu);
    printf("Final cpu reality -- PC: %X, X: %X, Y: %X, AC: %X, SP: %X, SR: %X\n", cpu->PC, cpu->X, cpu->Y, cpu->AC,
           cpu->SP, cpu_combine_SR(cpu->SR));
    printf("Final ram reality -- ");
    cJSON_ArrayForEach(fram_index, fram) {
      printf("[%X, %X]", (int)cJSON_GetNumberValue(cJSON_GetArrayItem(fram_index, 0)),
             bus_read(cpu->bus, cJSON_GetNumberValue(cJSON_GetArrayItem(fram_index, 0)), CPUMEM));
    }
    printf("\n\n");
    assert((uint16_t)cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(final, "pc")) == cpu->PC);
    assert((uint8_t)cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(final, "x")) == cpu->X);
    assert((uint8_t)cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(final, "y")) == cpu->Y);
    assert((uint8_t)cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(final, "a")) == cpu->AC);
    assert((uint8_t)cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(final, "s")) == cpu->SP);
    assert((uint8_t)cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(final, "p")) == cpu_combine_SR(cpu->SR));
    cJSON_ArrayForEach(fram_index, fram) {
      assert((uint8_t)cJSON_GetNumberValue(cJSON_GetArrayItem(fram_index, 1)) ==
             bus_read(cpu->bus, (uint16_t)cJSON_GetNumberValue(cJSON_GetArrayItem(fram_index, 0)), CPUMEM));
    }
    cJSON_free(name);
    cJSON_free(initial);
    cJSON_free(final);
    cJSON_free(iram);
    cJSON_free(iram_index);
    cJSON_free(fram);
    cJSON_free(fram_index);
    cJSON_free(name_s);
  }
}
