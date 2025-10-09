/*
This is the main file for the project, where we implement the different parts of the game of tetris, 
specifically the setup such as the logic and creating and initialising the pieces and/or game.

#include <stdint.h>
#include <stddef.h>

extern void enable_interrupt(void);

#define width_screen 320
#define height_screen 240
#define GRID_WIDTH 10     
#define GRID_HEIGHT 20
#define block_size 10
#define GRID_COLOR 0x12 //to see the actual grid


#define x_offset ((width_screen - GRID_WIDTH * block_size) / 2)
#define y_offset ((height_screen - (GRID_HEIGHT) * block_size) / 2)

char grid[GRID_HEIGHT][GRID_WIDTH];  // [row][col]
int timeoutcount = 0;
int score = 0;

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

    
    for (int i  = 0; i < ms; i++) {
        for (int j = 0; j < 5000; j++) {
            // do nothing
        }
    }
}

//SETUP FOR JOYSTICK
#define JOY_LEFT  0x1  // bit x 0001
#define JOY_RIGHT 0x2  // bit y 0010


void joystick_init(void){
    volatile int* gpio_direction = (volatile int*)0x040000e4; //direction
    *gpio_direction = 0x0;

}

int get_joystick_data(void){
    volatile int* gpio_data = (volatile int*)0x040000e0; //data
    return *gpio_data;
}

int joystick_left(void){
    return (get_joystick_data() & JOY_LEFT) !=0;
}
int joystick_right(void){
    return (get_joystick_data()& JOY_RIGHT) !=0;
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
    {0x3E,0x51,0x49,0x45,0x3E}, // 0
    {0x00,0x42,0x7F,0x40,0x00}, // 1
    {0x42,0x61,0x51,0x49,0x46}, // 2
    {0x21,0x41,0x45,0x4B,0x31}, // 3
    {0x18,0x14,0x12,0x7F,0x10}, // 4
    {0x27,0x45,0x45,0x45,0x39}, // 5
    {0x3C,0x4A,0x49,0x49,0x30}, // 6
    {0x01,0x71,0x09,0x05,0x03}, // 7
    {0x36,0x49,0x49,0x49,0x36}, // 8
    {0x06,0x49,0x49,0x29,0x1E}, // 9
    
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
    const unsigned char *bitmap = NULL;

    if (c >= '0' && c <= '9') {
        bitmap = font5x7[c - '0'];
    } else if (c >= 'A' && c <= 'Z') {
        bitmap = font5x7[10 + (c - 'A')]; // letters start after 10 digits
    } else {
        return; 
    }

    for (int col = 0; col < 5; col++) {
        unsigned char bits = bitmap[col];
        for (int row = 0; row < 7; row++) {
            if (bits & (1 << row)) {
                VGA[(y + row) * width_screen + (x + col)] = color;
            }
        }
    }
}

void game_over(){
    for (int i = 0; i < width_screen * height_screen; i++) {
        VGA[i] = 0x00; // black
    }

    const char *msg = "GAME OVER";
   
    int msg_len = 9;
    int start_x = width_screen / 2 - (msg_len * 8) / 2;
    int start_y = height_screen / 2 - 9;

    for (int i = 0; i < msg_len; i++) {
        draw_char(start_x + i * 8, start_y, msg[i], 0xE0); // red letters
    }

    const char *msg2 = "SCORE";
    int msg2_len = 5;
    int start2_x = width_screen / 2 - (msg2_len * 8) / 2;
    int start2_y = height_screen / 2;

    for (int i = 0; i < msg2_len; i++) {
        draw_char(start2_x + i * 8, start2_y, msg2[i], 0xE0); // red letters
    }

    int score_digits[6]; // up to 6 digits for your show_score function
    score_digits[0] = (score / 100000) % 10;
    score_digits[1] = (score / 10000) % 10;
    score_digits[2] = (score / 1000) % 10;
    score_digits[3] = (score / 100) % 10;
    score_digits[4] = (score / 10) % 10;
    score_digits[5] = score % 10;

    int score_x = width_screen / 2 - (msg2_len * 8) / 2; // adjust centering for 6 digits
    int score_y = start2_y + 10;

    int leading = 1; // flag to skip leading zeros
    for (int i = 0; i < 6; i++) {
        if (!leading || score_digits[i] != 0 || i == 5) {
            draw_char(score_x + i*8, score_y, '0' + score_digits[i], 0xE0);
            leading = 0;
        }
    }
    while (1);

}



void spawn_next() {
    current_piece = next_piece;
    current_piece.x = GRID_WIDTH / 2 - 2;
    current_piece.y = 0;
    next_piece = pieces[rand() % 7];
    
    // Check if new piece can be placed
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


 


Set-up drawing and erasing things on the grids
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

void set_displays(int display_number, int value){
    volatile int* DIS = (volatile int*)(0x04000050 + (0x10 * display_number));
    switch(value){
      case 0:
        *DIS = 0xC0;
        break;
      case 1:
        *DIS = 0xF9;
        break;
      case 2:
        *DIS = 0xA4;
        break;
      case 3:
        *DIS = 0xB0; 
        break;
      case 4:
        *DIS = 0x99;
        break;
      case 5:
        *DIS = 0x92;
        break;
      case 6:
        *DIS = 0x82;
        break;
      case 7:
        *DIS = 0xF8;
        break;
      case 8:
        *DIS = 0x80;
        break;
      case 9:
        *DIS = 0x90;
        break;
    }
  }

void show_score(int score) {
    

    int sixth = (score / 100000) % 10;
    int fifth = (score / 10000) % 10;
    int forth = (score / 1000) % 10;
    int third  = (score / 100) % 10;
    int second = (score / 10) % 10;
    int first  = score % 10;

    set_displays(0, first);
    set_displays(1, second);
    set_displays(2, third);
    set_displays(3, forth);
    set_displays(4, fifth);
    set_displays(5, sixth);


}

Piece Property functions
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

int can_move(shape *p, int dx, int dy, int new_shape[4][4]){
    for (int row = 0; row < 4; row++){
        for (int col = 0; col < 4; col++){
            if (new_shape[row][col]){
                int grid_row = p->y + row +dy;
                int grid_col = p->x + col + dx;

                if (grid_row< 0 || grid_row >= GRID_HEIGHT || grid_col < 0 || grid_col >= GRID_WIDTH) return 0;
                if (grid[grid_row][grid_col]) return 0;
            }
        }
    }
    return 1;
}


void rotate_piece(shape *p) {
    int temp[4][4];
    
    for (int y = 0; y < 4; y++){ 
        for (int x = 0; x < 4; x++){
             temp[y][x] = 0; 
        }
    }  
            
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            temp[col][3 - row] = p->shape[row][col]; // 90° clockwise
        }
    }

    if (can_move(p, 0, 0, temp)){
        for (int row = 0; row < 4; row++){
            for (int col = 0; col < 4; col++){
                p->shape[row][col] = temp[row][col];
            }
        }
    }
}

void move_piece(shape *p, int joystick_data){
    update_shape(p,ERASE);
    int moved = 1;
    
    volatile int sw_pulled = get_sw();
    
    if (joystick_left() && can_move(p, -1, 0, p-> shape)){
        p->x -= 1;
        moved = 1;
    }
    if (joystick_right() && can_move(p, 1, 0, p-> shape)){
        p->x += 1;
        moved = 1;
    }
    if (sw_pulled & 0x2){
        while (can_move(p, 0, 1, p->shape)){
            p->y += 1;
        }
        moved = 1;     
        score+=50;
        show_score(score);  
    } 

    if (moved){
        update_shape(p, DRAW);
    }
}











void clear_lines() {
    for (int y = GRID_HEIGHT - 1; y >= 0; y--) {
        int full = 1;

        for (int x = 0; x < GRID_WIDTH; x++)
            if (grid[y][x] == 0) {
                 full = 0;
                 break;
     }
        if (full) {
            for (int y_new = y; y_new > 0; y_new--)
                for (int x = 0; x < GRID_WIDTH; x++)
                    grid[y_new][x] = grid[y_new - 1][x];
            for (int x = 0; x < GRID_WIDTH; x++) grid[0][x] = 0;
            y++; // re-check same row after shifting
            score+=100;
            show_score(score); 
        }
    }
       
}

  



void tick(volatile char *VGA){
    int joystick_data = get_joystick_data();
   
    
    int switch_state = get_sw();

    if (switch_state & 0x1) {
        update_shape(&current_piece, ERASE);
        rotate_piece(&current_piece);
        update_shape(&current_piece, DRAW);
       
    }

    
   

    move_piece(&current_piece, joystick_data);
    
   
    

    if (can_move(&current_piece, 0, 1, current_piece.shape)){
        current_piece.y+=1;  
    }

    else{
        update_shape(&current_piece, LOCK);
                
        clear_lines();
        
        clear_next_shape(VGA, &next_piece);
        spawn_next();
        draw_next_piece(VGA, &next_piece);
    }
    
    draw_grid(VGA);
   
    
    
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

void start()
{
    for (int i = 0; i < width_screen * height_screen; i++){ 
        VGA[i] = 0;
        const char *msg = "TETRIS";
        int msg_len = 6;
        int start_x = width_screen / 2 - (msg_len * 8) / 2;
        int start_y = height_screen / 2 - 10;


        for (int i = 0; i < msg_len; i++)
            draw_char(start_x + i * 8, start_y, msg[i], 0x5B);

        delay(3000);
        }
    for (int i = 0; i < width_screen * height_screen; i++)
        VGA[i] = 0;
}


int main() {
    joystick_init();
    for (int r = 0; r < GRID_HEIGHT; r++){
    for (int c = 0; c < GRID_WIDTH; c++){
        grid[r][c] = 0;}}


    start();
    
    draw_grid(VGA);
    
   
    labinit();

    int sw = get_sw();
    set_rand_seed(sw); //randomise the piece that will follow through.


    next_piece = pieces[rand()%7];
    spawn_next();
    
   
    draw_shape(VGA, &current_piece);
    clear_next_shape(VGA, &next_piece);
    draw_next_piece(VGA, &next_piece);

    while(1) {
    }
}

*/

