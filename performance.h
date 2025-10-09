#define PERFORMANCE_H

#include <stdint.h>

//struct for performance counters

typedef struct{
    uint32_t cycles;
    uint32_t instructions;
    uint32_t mem_instructs;
    uint32_t i_cache;
    uint32_t d_cache;
    uint32_t i_stall;
    uint32_t d_stall;
    uint32_t data_hazards;
    uint32_t alu_operations;

} performance_counters;

void performance_start();
void performance_stop(performance_counters *counters);
