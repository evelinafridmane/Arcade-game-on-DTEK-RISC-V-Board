#define TETRIS_H

#include <stdint.h>
#include <stddef.h>

//SCREEN AND GRID CONSTANTS

#define width_screen 320
#define height_screen 240
#define GRID_WIDTH 10     
#define GRID_HEIGHT 20
#define block_size 10
#define GRID_COLOR 0x12 

#define x_offset ((width_screen - GRID_WIDTH * block_size) / 2)
#define y_offset ((height_screen - (GRID_HEIGHT) * block_size) / 2)

//Joystick constants

#define JOY_LEFT  0x1  // bit x 0001
#define JOY_RIGHT 0x2  // bit y 0010

//Piece modes
#define ERASE   0
#define DRAW    1
#define LOCK    2

//EXTERNAL DECLARATIONS
extern void enable_interrupt(void);

//GLOBAL VARIABLES
extern volatile char *VGA;
extern char grid[GRID_HEIGHT][GRID_WIDTH];
extern int timeoutcount;
extern int score;
extern uint32_t seed;

// shape structure
typedef struct {
    char name;
    int x, y;
    char color;
    int shape[4][4];
} shape;

extern shape current_piece;
extern shape next_piece;
extern shape pieces[7];

// Font
extern const unsigned char font5x7[][5];



// Hardware & gpio functions
void labinit(void);
int get_sw(void);
void joystick_init(void);
int get_joystick_data(void);
int joystick_left(void);
int joystick_right(void);


// Random functions
void set_rand_seed(uint8_t s);
uint8_t rand(void);

// Utility functions from Lab
void delay(int ms);

// Game functions
void spawn_next(void);
int is_filled(shape *p);
void game_over(void);
void start(void);

// Drawing functions
void draw_char(int x, int y, char c, char color);
void draw_grid(volatile char *VGA);
void draw_shape(volatile char *VGA, shape *current);
void draw_next_piece(volatile char *VGA, shape *next);
void clear_next_shape(volatile char *VGA, shape *next);
void update_cell(int row, int col, char color);

//Display functions
void set_displays(int display_number, int value);
void show_score(int score);

// Piece manipulation
void update_shape(shape *p, char mode);
int can_move(shape *p, int dx, int dy, int new_shape[4][4]);
void rotate_piece(shape *p);
void move_piece(shape *p, int joystick_data);
void clear_lines(void);



// Game loop 
void tick(volatile char *VGA);
void handle_interrupt(unsigned cause);



