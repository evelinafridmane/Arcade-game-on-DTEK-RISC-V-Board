#include "tetris.h"
#include "performance.h"
#include <stdint.h>
#include <stddef.h>
extern void print(const char*);
extern void print_dec(unsigned int);


void measure_performance(void){
    performance_counters counters;
    int target_pieces = 30;
    int pieces_placed = 0;
    print("Starting performance measurements... \n");
    print("Running until 30 pieces are placed game ticks...\n");

    performance_start();

    while (pieces_placed < target_pieces){
        tick(VGA);
        pieces_placed++;
    }
    

    performance_stop(&counters);

    //PRINTING ALL THE RAW DATA
    print("\n ===Performance Data === \n");

    print("Cycles: ");
    print_dec(counters.cycles);
    print("\n");

    print("Instructions: ");
    print_dec(counters.instructions);
    print("\n");

    print("Memory Instructions: ");
    print_dec(counters.mem_instructs);
    print("\n");

    print("I-cache Misses: ");
    print_dec(counters.i_cache);
    print("\n");

    print("D-cache Misses: ");
    print_dec(counters.d_cache);
    print("\n");

    print("I-cache Stalls: ");
    print_dec(counters.i_stall);
    print("\n");

    print("D-cache Stalls: ");
    print_dec(counters.d_stall);
    print("\n");

    print("Data Hazard Stalls: ");
    print_dec(counters.data_hazards);
    print("\n");

    print("ALU stalls: ");
    print_dec(counters.alu_operations);
    print("\n");
}




int main()
{
    joystick_init();

    // Initialize grid
    for (int r = 0; r < GRID_HEIGHT; r++)
    {
        for (int c = 0; c < GRID_WIDTH; c++)
        {
            grid[r][c] = 0;
        }
    }

    start();
    draw_grid(VGA);
    labinit();

    int sw = get_sw();
    set_rand_seed(sw);

    int piece_index = rand() % 7;
    next_piece.name = pieces[piece_index].name;
    next_piece.color = pieces[piece_index].color;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            next_piece.shape[i][j] = pieces[piece_index].shape[i][j];
        }
    }
    spawn_next();

    draw_shape(VGA, &current_piece);
    clear_next_shape(VGA, &next_piece);
    draw_next_piece(VGA, &next_piece);


    measure_performance();
    while (1)
    {
    }
}