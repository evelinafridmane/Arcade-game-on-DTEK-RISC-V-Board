/*
This is the main file for the project, where we implement the different parts of the game of tetris, 
specifically the setup such as the logic and creating and initialising the pieces and/or game.
*/

extern void enable_interrupt(void);
#define width_screen 320
#define height_screen 240
#define GRID_WIDTH 10     
#define GRID_HEIGHT 20
#define block_size 8
#define GRID_COLOR 0x12 //to see the actual grid


#define x_offset ((width_screen - GRID_WIDTH * block_size) / 2)
#define y_offset ((height_screen - (GRID_HEIGHT) * block_size) / 2)

char grid[GRID_HEIGHT][GRID_WIDTH];  // [row][col]
int timeoutcount = 0;

// VGA framebuffer base address
volatile char *VGA = (volatile char*) 0x08000000;



typedef struct {
    char name; //id of the piece
    int x, y;      // top-left position on the screen / grid
    char color;    // VGA color of the piece
    int shape[4][4]; // 4x4 grid representing the shape (1 = block, 0 = empty)
} shape;

 shape current_piece;

void spawn_piece(shape *current){
    current_piece = *current;
    current_piece.x = GRID_WIDTH/2 -2;
    current_piece.y = 0;
}



void labinit(void)
{
    volatile unsigned short* tmr_periodlow = (unsigned short*)0x04000028;
    volatile unsigned short* tmr_periodhigh = (unsigned short*)0x0400002c;
    *tmr_periodlow = (29999999/10) & 0xFFFF;
    *tmr_periodhigh = (29999999/10) >> 16;

    volatile unsigned short* tmr_start = (unsigned short*)0x04000024;
    *tmr_start =  0x7;

  
    enable_interrupt();
}

int get_btn(void){
  volatile int* btn = (volatile int*)0x040000d0;
  return ((*btn) & 0x1);
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


void draw_shape(volatile char *VGA, shape *t) {
    // pointer to VGA memory, pointer to tetris shape, how big
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            // loops throught 4x4 piece
            if (t->shape[y][x]) {
                    //checks for 0 and 1

                int pixel_index =
                    (y_offset + (t->y + y) * block_size) * width_screen +
                    (x_offset + (t->x + x) * block_size);
                //gets top-left corner of piece, shifts to the right 


                for (int row = 0; row < block_size; row++) {
                    // counts vertical pixels inside the block
                    for (int col = 0; col < block_size; col++) {
                    // counts horizontal pixels inside the block
                        VGA[pixel_index + row * width_screen + col] = t->color;
                    }
                }
            }
        }
    }
}

shape I = {0, 0, 0, 0x1F, {
        {1,0,0,0},
        {1,0,0,0},
        {1,0,0,0},
        {1,0,0,0}
    }
    };    
    
shape J = {1, 0, 0, 0x03, {
    {0,1,0,0},
    {0,1,0,0},
    {1,1,0,0},
    {0,0,0,0}
}};

shape L = {2, 0, 0, 0xEC, {
    {1,0,0,0},
    {1,0,0,0},
    {1,1,0,0},
    {0,0,0,0}
}};

shape O = {3, 0, 0, 0xFC, {
        {1,1,0,0},
        {1,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
    }
    };


shape S = {4, 0, 0, 0x1C, {
        {1,0,0,0},
        {1,1,0,0},
        {0,1,0,0},
        {0,0,0,0}
    }};

shape T = {5, 0, 0, 0x63, {
        {0,1,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    }};


shape Z = {6, 0, 0, 0xE0 , {
    {1,1,0,0},
    {0,1,1,0},
    {0,0,0,0},
    {0,0,0,0}
}};

void rotate_piece(shape *p) {
    int temp[4][4];

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            temp[x][3 - y] = p->shape[y][x]; // 90° clockwise
        }
    }

    // Copy back to the shape
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            p->shape[y][x] = temp[y][x];
}

int last_btn_state = 0;

void tick(volatile char *VGA){
    draw_grid(VGA); // redraw background


    int btn = get_btn();

    if (btn && !last_btn_state){
        rotate_piece(&current_piece);
    }

    last_btn_state = btn;
    // compute piece height
    int max_row = 0;
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            if (current_piece.shape[y][x] && y + 1 > max_row)
                max_row = y + 1;  // max_row = height of the piece

    // Only fall if bottom of piece is below the last row
    if (current_piece.y + max_row < GRID_HEIGHT)
        current_piece.y += 1;  // piece falls

    draw_shape(VGA, &current_piece); // draw at new position
}

void handle_interrupt(unsigned cause) 
{
    if (cause == 16){
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
    
    spawn_piece(&L); //test
    
    draw_grid(VGA);
    
    draw_shape(VGA, &current_piece);
    //draw_shape(VGA, &O);
    //draw_shape(VGA, &I);
    //draw_shape(VGA, &S);
    //draw_shape(VGA, &Z);
    //draw_shape(VGA, &L);
    //draw_shape(VGA, &J);

    
    while(1) {
       
    }

}



