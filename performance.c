#include "performance.h"



void performance_start(){
    asm volatile ("csrw mcycle, x0");
    asm volatile ("csrw minstret, x0");
    asm volatile ("csrw mhpmcounter3, x0");
    asm volatile ("csrw mhpmcounter4, x0");
    asm volatile ("csrw mhpmcounter5, x0");
    asm volatile ("csrw mhpmcounter6, x0");
    asm volatile ("csrw mhpmcounter7, x0");
    asm volatile ("csrw mhpmcounter8, x0");
    asm volatile ("csrw mhpmcounter9, x0");
}

void performance_stop(performance_counters *counters){
    asm("csrr %0, mcycle" : "=r"(counters->cycles) );
    asm("csrr %0, minstret" : "=r"(counters->instructions) );
    asm("csrr %0, mhpmcounter3" : "=r"(counters->mem_instructs) );
    asm("csrr %0, mhpmcounter4" : "=r"(counters->i_cache) );
    asm("csrr %0, mhpmcounter5" : "=r"(counters->d_cache) );
    asm("csrr %0, mhpmcounter6" : "=r"(counters->i_stall) );
    asm("csrr %0, mhpmcounter7" : "=r"(counters->d_stall) );
    asm("csrr %0, mhpmcounter8" : "=r"(counters->data_hazards) );
    asm("csrr %0, mhpmcounter9" : "=r"(counters->alu_operations) );
    

}