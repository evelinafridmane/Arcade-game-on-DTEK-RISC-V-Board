/*
This is the main file for the project, where we implement the different parts of the game of tetris, 
specifically the setup such as the logic and creating and initialising the pieces and/or game.
*/
#include <stdint.h>

extern void enable_interrupt(void);

#define width_screen 320
#define height_screen 240
#define GRID_WIDTH 10     
#define GRID_HEIGHT 20
#define block_size 8
#define GRID_COLOR 0x12 //to see the actual grid
#define MAX_PIECES 300

#define x_offset ((width_screen - GRID_WIDTH * block_size) / 2)
#define y_offset ((height_screen - (GRID_HEIGHT) * block_size) / 2)

char grid[GRID_HEIGHT][GRID_WIDTH];  // [row][col]
int timeoutcount = 0;

//Random number generator
uint32_t seed;

void set_rand_seed(uint8_t s) {
  seed = s;
}

uint8_t rand() {
  static uint8_t a = 75;
  static uint8_t c = 179; 
  static uint8_t modulus = 1 << 6; 
  seed = (a * seed + c) % modulus;
  return seed;
}

//Copied from Lab exercise 1
void delay( int ms ) {
    int i;
    while( ms > 0 ){
        ms = ms - 1;
    /* Executing the following for loop should take 1 ms */
    for( i = 0; i < 4711*ms; i = i + 1 ) /* The constant 4711 must be easy to change! */
    {
    /* Do nothing. */
    }
    }
}





// VGA framebuffer base address
volatile char *VGA = (volatile char*) 0x08000000;

typedef struct {
    char name; //id of the piece
    int x, y;      // top-left position on the screen / grid
    char color;    // VGA color of the piece
    int shape[4][4]; // 4x4 grid representing the shape (1 = block, 0 = empty)
} shape;

shape current_piece;
shape next_piece;

shape pieces[] = {
{0, 0, 0, 0x1F, { //I
        {1,0,0,0},
        {1,0,0,0},
        {1,0,0,0},
        {1,0,0,0}
    }
},
{1, 0, 0, 0x03, { //J
    {0,1,0,0},
    {0,1,0,0},
    {1,1,0,0},
    {0,0,0,0}
}},
{2, 0, 0, 0xEC, { // L
    {1,0,0,0},
    {1,0,0,0},
    {1,1,0,0},
    {0,0,0,0}
}},
{3, 0, 0, 0xFC, { // O
        {1,1,0,0},
        {1,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
}},
{4, 0, 0, 0x1C, { // S
        {1,0,0,0},
        {1,1,0,0},
        {0,1,0,0},
        {0,0,0,0}
}},
{5, 0, 0, 0x63, { //T
        {0,1,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
}},
{6, 0, 0, 0xE0 , { //Z
    {1,1,0,0},
    {0,1,1,0},
    {0,0,0,0},
    {0,0,0,0}
}}
};

shape locked_pieces[MAX_PIECES];
int num_locked = 0;

void spawn_piece(shape *current){
    current_piece = *current;
    current_piece.x = GRID_WIDTH/2 -2;
    current_piece.y = 0;
}

void spawn_next(){
    current_piece = next_piece;
    next_piece = pieces[rand() % 7];
    spawn_piece(&current_piece);
}


void labinit(void)
{
    volatile unsigned short* tmr_periodlow = (unsigned short*)0x04000028;
    volatile unsigned short* tmr_periodhigh = (unsigned short*)0x0400002c;
    *tmr_periodlow = (29999999/10) & 0xFFFF;
    *tmr_periodhigh = (29999999/10) >> 16;

    volatile unsigned short* tmr_start = (unsigned short*)0x04000024;
    *tmr_start =  0x7;

    volatile int* button_interrupt = (int*)0x040000d8;
    *button_interrupt = 0x1;

    volatile int* button_edge = (int*)0x040000dc;
    *button_edge = 0x1;



    enable_interrupt();
}

int get_btn(void){
  volatile int* btn = (volatile int*)0x040000d0;
  return ((*btn) & 0x1);
}

int get_sw( void ){
  volatile int* switches = (volatile int*)0x04000010;
  return (*switches) & 0x3FF;
  //printf("%d",i);
}

void draw_grid(volatile char *VGA){
    
    for (int row = 0; row < GRID_HEIGHT; row++){
        for (int col = 0; col < GRID_WIDTH; col++){
            char color = grid[row][col];
            if (!color) color = GRID_COLOR; // use a different colour for empty cells

            int pixel_index = (y_offset + row * block_size) * width_screen +
                              (x_offset + col * block_size);

            for (int y = 0; y < block_size; y++) {
                for (int x = 0; x < block_size; x++){
                    // Draw border: only draw a single pixel at all edges
                    if (y == 0 || x == 0 || y == block_size - 1 || x == block_size - 1)
                    VGA[pixel_index + y * width_screen + x] = GRID_COLOR;
                    else if (grid[row][col])
                        VGA[pixel_index + y * width_screen + x] = color; // filled block
                    else
                        VGA[pixel_index + y * width_screen + x] = 0; // empty inside
                }
            }
        }
    }
}


void draw_shape(volatile char *VGA, shape *current) {
    // pointer to VGA memory, pointer to tetris shape, how big
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            // loops throught 4x4 piece
            if (current ->shape[y][x]) {
                    //checks for 0 and 1

                int pixel_index =
                    (y_offset + (current->y + y) * block_size) * width_screen +
                    (x_offset + (current->x + x) * block_size);
                //gets top-left corner of piece, shifts to the right 


                for (int row = 0; row < block_size; row++) {
                    // counts vertical pixels inside the block
                    for (int col = 0; col < block_size; col++) {
                    // counts horizontal pixels inside the block
                        VGA[pixel_index + row * width_screen + col] = current->color;
                    }
                }
            }
        }
    }
}

void draw_next_piece(volatile char *VGA, shape *next){ // draws the future piece inside a new grid that shows this.
    int preview_x = width_screen - 60;
    int preview_y = 40;

    for (int y= 0; y < 4; y++){
        for (int x =0; x<4; x++){
            if (next -> shape[y][x]){
                int pixel_index =
                    (preview_y + y*block_size) * width_screen + (preview_x + x*block_size);
                
                    for (int row = 0; row < block_size; row++){
                        for (int col = 0; col < block_size; col++){
                            VGA[pixel_index + row*width_screen + col] = next->color;
                        }
                    }
            }
        }
    }
}

void clear_shape(volatile char *VGA, shape *next){
    int preview_x = width_screen - 60;
    int preview_y = 40;

    for (int y = 0; y < 4; y++){
        for (int x = 0; x < 4; x++){
            if (next->shape[y][x]){
                int pixel_index =
                    (preview_y + y*block_size) * width_screen + (preview_x + x*block_size);

                for (int row = 0; row < block_size; row++){
                    for (int col = 0; col < block_size; col++){
                        VGA[pixel_index + row*width_screen + col] = 0; // clear to black
                    }
                }
            }
        }
    }
}

void rotate_piece(shape *p) {
    int temp[4][4];
    volatile int button_pressed = get_btn();
   
    if (button_pressed){
        
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
        temp[x][3 - y] = p->shape[y][x]; // 90° clockwise
           
        }
    }
    }
    int valid = 1;
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (temp[y][x]) {
                    if (p->x + x < 0) valid = 0;  // outside left
                    if (p->x + x >= GRID_WIDTH) valid = 0;  // outside right
                    if (p->y + y >= GRID_HEIGHT) valid = 0; // outside bottom
                }
            }
        }

        // only apply rotation if it's valid
        if (valid) {
            for (int y = 0; y < 4; y++)
                for (int x = 0; x < 4; x++)
                    p->shape[y][x] = temp[y][x];
        }
}

void move_piece(shape *p){
    

    volatile int sw_pulled = get_sw();

    int max_w = 0;
    for (int y = 0; y < 4; y++){
        for (int x = 0; x < 4; x++){
            if (current_piece.shape[y][x] && x + 1 > max_w){
                max_w = x + 1;  
    }}}

    if(sw_pulled&0x1){
        if (p->x > 0)      // prevent going past left wall
            p->x -= 1;
    
    }

    if(sw_pulled&0x2){
        if (p->x + max_w < GRID_WIDTH) // prevent going past right wall
            p->x += 1;
    }

}

int collision(shape *p) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (p->shape[y][x]) {
                int new_y = p->y + y + 1;
                int new_x = p->x + x;


                // check collision with locked pieces
                if (grid[new_y][new_x]) 
                    return 0;
            }
        }
    }
    return 1;
}

void tick(volatile char *VGA){
    
    volatile int sw_pulled = get_sw();
    if(sw_pulled){
    move_piece(&current_piece);
    }
 
    // compute piece height
    int max_row = 0;
    for (int y = 0; y < 4; y++){
        for (int x = 0; x < 4; x++){
            if (current_piece.shape[y][x] && y + 1 > max_row){
                max_row = y + 1;  // max_row = height of the piece
    }}}
    // falls unless bottom of piece is below the last row or collision
    if ((current_piece.y + max_row < GRID_HEIGHT) && collision(&current_piece) ){
        current_piece.y += 1;  // piece falls
    }
    else{
        for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            if (current_piece.shape[y][x])
                grid[current_piece.y + y][current_piece.x + x] = 1;
                
        if (num_locked < MAX_PIECES){
            locked_pieces[num_locked++] = current_piece;
        }
        clear_shape(VGA, &next_piece);
        spawn_next();
        draw_next_piece(VGA, &next_piece);
    }

    draw_grid(VGA); // redraw background
    for (int i = 0; i < num_locked; i++){
        draw_shape(VGA, &locked_pieces[i]);
    }
    draw_shape(VGA, &current_piece);
}

void handle_interrupt(unsigned cause) 
{
    if (cause == 16){ //Timer
        volatile unsigned short* tmr_stat = (unsigned short*)0x04000020;
        *tmr_stat = 0x0;

        timeoutcount++;
        if (timeoutcount == 10){
            timeoutcount = 0;
            tick(VGA);
        }
    }
    
}



int main() {
    labinit();

    int sw = get_sw();
    set_rand_seed(sw); //randomise the piece that will follow through.


    next_piece = pieces[rand()%7];
    
    spawn_next();
    
   
    
    draw_grid(VGA);
    draw_shape(VGA, &current_piece);
    draw_next_piece(VGA, &next_piece);
    
   
    int prev_btn = 0;
    
    while(1) {
        int btn_state = get_btn();
        if (btn_state && !prev_btn) {
            rotate_piece(&current_piece);
        }
        prev_btn = btn_state;
    

   
}
}



