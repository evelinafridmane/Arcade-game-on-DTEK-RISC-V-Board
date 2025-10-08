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

const unsigned char font5x7[][5] = {
    // Each byte column (bit = 1 => pixel on)
    // A–Z (ASCII 'A' = 65 → index 0)
    {0x7E,0x11,0x11,0x7E,0x00}, // A
    {0x7F,0x49,0x49,0x36,0x00}, // B
    {0x3E,0x41,0x41,0x22,0x00}, // C
    {0x7F,0x41,0x41,0x3E,0x00}, // D
    {0x7F,0x49,0x49,0x41,0x00}, // E
    {0x7F,0x09,0x09,0x01,0x00}, // F
    {0x3E,0x41,0x51,0x72,0x00}, // G
    {0x7F,0x08,0x08,0x7F,0x00}, // H
    {0x00,0x41,0x7F,0x41,0x00}, // I
    {0x20,0x40,0x41,0x3F,0x00}, // J
    {0x7F,0x08,0x14,0x63,0x00}, // K
    {0x7F,0x40,0x40,0x40,0x00}, // L
    {0x7F,0x06,0x06,0x7F,0x00}, // M
    {0x7F,0x06,0x18,0x7F,0x00}, // N
    {0x3E,0x41,0x41,0x3E,0x00}, // O
    {0x7F,0x09,0x09,0x06,0x00}, // P
    {0x3E,0x41,0x61,0x7E,0x00}, // Q
    {0x7F,0x09,0x19,0x66,0x00}, // R
    {0x26,0x49,0x49,0x32,0x00}, // S
    {0x01,0x7F,0x01,0x01,0x00}, // T
    {0x3F,0x40,0x40,0x3F,0x00}, // U
    {0x1F,0x60,0x60,0x1F,0x00}, // V
    {0x7F,0x30,0x30,0x7F,0x00}, // W
    {0x63,0x1C,0x1C,0x63,0x00}, // X
    {0x07,0x78,0x08,0x07,0x00}, // Y
    {0x61,0x51,0x49,0x47,0x00}, // Z
};

shape locked_pieces[MAX_PIECES];
int num_locked = 0;

void spawn_piece(shape *current){
    current_piece = *current;
    current_piece.x = GRID_WIDTH/2 -2;
    current_piece.y = 0;
}

int is_filled(shape *p) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (p->shape[y][x]) {
                int row = p->y + y;
                int col = p->x + x;
                if (row >= 0 && row < GRID_HEIGHT && col >= 0 && col < GRID_WIDTH){
                    if (grid[row][col]) return 1;
                }
                
            }
        }
    }
    return 0;
}

void draw_char(int x, int y, char c, char color) {
    if (c == ' ') return; // skip space

    if (c >= 'A' && c <= 'Z') {
        const unsigned char *bitmap = font5x7[c - 'A'];
        for (int col = 0; col < 5; col++) {
            unsigned char bits = bitmap[col];
            for (int row = 0; row < 7; row++) {
                if (bits & (1 << row)) {
                    VGA[(y + row) * width_screen + (x + col)] = color;
                }
            }
        }
    }
}
void game_over(){
    for (int i = 0; i < width_screen * height_screen; i++) {
        VGA[i] = 0x00; // black
    }

    const char *msg = "GAME OVER";
    const char *msg2 = "SCORE";
    int msg_len = 9;
    int start_x = width_screen / 2 - (msg_len * 8) / 2;
    int start_y = height_screen / 2 - 9;

    int msg2_len = 5;
    int start2_x = width_screen / 2 - (msg_len * 4) / 2;
    int start2_y = height_screen / 2;


    for (int i = 0; i < msg_len; i++) {
        draw_char(start_x + i * 8, start_y, msg[i], 0xE0); // red letters
    }

    for (int i = 0; i < msg2_len; i++) {
        draw_char(start2_x + i * 5, start2_y, msg2[i], 0xE0); // red letters
    }

    while (1);
}



void spawn_next(){
    current_piece = next_piece;
    next_piece = pieces[rand() % 7];
    spawn_piece(&current_piece);
   
    if (is_filled(&current_piece)) {
        game_over(); 
    }

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


/*Set-up drawing and erasing things on the grids*/
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

void clear_next_shape(volatile char *VGA, shape *next){
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


void update_cell(int row, int col, char color){
    int pixel_index = (y_offset + row * block_size) * width_screen + (x_offset + col * block_size);
    
    for (int y = 0; y < block_size; y++){
        for (int x = 0; x < block_size; x++) {
            // draw borders regardless
            if (y == 0 || x == 0 || y == block_size - 1 || x == block_size - 1)
                VGA[pixel_index + y * width_screen + x] = GRID_COLOR;
            else
                VGA[pixel_index + y * width_screen + x] = color; // fill block
        }
    }
}



/*Piece Property functions*/
#define ERASE   0
#define DRAW    1
#define LOCK    2

void update_shape(shape *p, char mode){
    for (int y = 0; y < 4; y++) {

        for (int x = 0; x < 4; x++) {

            if (! p->shape[y][x]) continue; 

            int row = p->y + y;
            int col = p->x +x;

            if (row < 0 || row >= GRID_HEIGHT || col < 0 || col >= GRID_WIDTH) continue;

            switch (mode){

                case ERASE:
                    if (!grid[row][col])
                        update_cell(row, col, 0); // empty background
                    else if (grid[row][col])
                        update_cell(row, col, grid[row][col]); // locked piece
                        
                    break;
                
                case DRAW:
                    update_cell(row, col, p->color);
                    break;
                
                case LOCK:

                    grid[row][col] = p->color;
                    update_cell(row, col, p->color);
                    break;

                }
            }
        }
    }


int check_fit(shape *p, int new_shape[4][4]){
    for (int row = 0; row < 4; row++){
        for (int col = 0; col < 4; col++){
            if (new_shape[row][col]){
                int grid_row = p->y + row;
                int grid_col = p->x + col;

                if (grid_row < 0 || grid_row >= GRID_HEIGHT || grid_col < 0 || grid_col >= GRID_WIDTH)
                    return 0; // out of bounds
                if (grid[grid_row][grid_col])
                    return 0; // blocked by locked piece
            }
        }
    }
    return 1;
}

int collision(shape *p) {
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (p->shape[y][x]) {
                    int new_y = p->y + y + 1;
                    int new_x = p->x + x;
    
                    // check collision with locked pieces & lower boundary
                    if (new_y >= GRID_HEIGHT) return 0;
                    if (grid[new_y][new_x]) return 0;

                }
            }
        }
        return 1;
    } 

void rotate_piece(shape *p) {
    int temp[4][4];
    for (int y = 0; y < 4; y++){ 
        for (int x = 0; x < 4; x++){
             temp[y][x] = 0; }}
    
    
        
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
        temp[col][3 - row] = p->shape[row][col]; // 90° clockwise
           
        }
    }
    
    if (check_fit(p,temp)){
        for (int row = 0; row < 4; row++){
            for (int col = 0; col < 4; col++){
                p->shape[row][col] = temp[row][col];
            }
        }
    }
}

void move_piece(shape *p){
    

    volatile int sw_pulled = get_sw();

    int max_w = 0;
    for (int y = 0; y < 4; y++){
        for (int x = 0; x < 4; x++){
            if (p->shape[y][x] && x + 1 > max_w){
                max_w = x + 1;  
    }}}
    if(collision(&current_piece)){
    if((sw_pulled&0x1) != 0){
        update_shape(p, ERASE);
        if (p->x > 0)      // prevent going past left wall
            p->x -= 1;
        update_shape(p, DRAW);
    }
    if((sw_pulled&0x2) != 0){
        update_shape(p, ERASE);
        if (p->x + max_w < GRID_WIDTH) // prevent going past right wall
            p->x += 1;
        update_shape(p, DRAW);
    }
    if((sw_pulled&0x4) != 0){
        update_shape(p, ERASE);
        // drop piece until collision
        while (collision(p)) {
            p->y += 1;
        }
        // move back one step to the last valid position
        p->y -= 1;

        update_shape(p, DRAW);
    }
    }
}

void clear_lines(void){
    for (int y = GRID_HEIGHT -1; y >= 0; y--){
        int full = 1;

        for (int x= 0; x < GRID_WIDTH; x++){
            if (grid[y][x] == 0){
                full=0;
                break;
            }
        }

        if (full) {
            for (int x = 0; x < GRID_WIDTH; x++){
                grid[y][x] = 0;
                update_cell(y,x,0);
            }

            for (int yy = y; yy > 0; yy--) {
                for (int x = 0; x < GRID_WIDTH; x++) {
                    grid[yy][x] = grid[yy - 1][x];
                    update_cell(yy, x, grid[yy][x]);
                }
            }

            for (int x = 0; x < GRID_WIDTH; x++) {
                grid[0][x] = 0;
                update_cell(0, x, 0);
            }

            // After shifting, recheck same row (since we moved rows down)
            y++;
        }
    }
}


void tick(volatile char *VGA){
    
    volatile int sw_pulled = get_sw();
    if(sw_pulled){
        move_piece(&current_piece);
    }
   
    update_shape(&current_piece, ERASE);

    if (collision(&current_piece)){
        current_piece.y+=1;  
    }

    else{
        update_shape(&current_piece, LOCK);
                
        clear_lines();
        
        if (num_locked < MAX_PIECES){
            locked_pieces[num_locked++] = current_piece;
        }
        
        clear_next_shape(VGA, &next_piece);
        spawn_next();
        draw_next_piece(VGA, &next_piece);
    }
    
    draw_grid(VGA);

    
    for (int i = 0; i < num_locked; i++)
        draw_shape(VGA, &locked_pieces[i]);

    
    draw_shape(VGA, &current_piece);
    
}

void handle_interrupt(unsigned cause) 
{
    if (cause == 16){ //Timer
        volatile unsigned short* tmr_stat = (unsigned short*)0x04000020;
        *tmr_stat = 0x0;

        timeoutcount++;
        if (timeoutcount == 8){
            timeoutcount = 0;
            tick(VGA);
        }
    }
    
}

void start (){
    for (int i = 0; i < width_screen * height_screen; i++) {
        VGA[i] = 0x00; // black
    }

    const char *msg = "TETRIS";
    
    int msg_len = 6;
    int start_x = width_screen / 2 - (msg_len * 6) / 2;
    int start_y = height_screen/2  - 6;

    for (int i = 0; i < msg_len; i++) {
        draw_char(start_x + i * 8, start_y, msg[i], 0xE0); // red letters
    }

  
}


int main() {
    
    start ();
    delay(2000); 
    for (int r = 0; r < GRID_HEIGHT; r++)
    for (int c = 0; c < GRID_WIDTH; c++)
        grid[r][c] = 0;
    
    draw_grid(VGA);
    
    labinit();

    int sw = get_sw();
    set_rand_seed(sw); //randomise the piece that will follow through.


    next_piece = pieces[rand()%7];
    
    spawn_next();
    
   
    draw_shape(VGA, &current_piece);
    draw_next_piece(VGA, &next_piece);
    
   
    
    
    while(1) {
        
   
        if (get_btn()){
            delay(2000);
            rotate_piece(&current_piece);
        }
        
    

   
}
}



