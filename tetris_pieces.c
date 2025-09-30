void handle_interrupt(unsigned cause) 
{}

/* Add your code here for initializing interrupts. */
void labinit(void)
{}

#define width_screen 320
#define height_screen 240
#define GRID_WIDTH 10     
#define GRID_HEIGHT 22
#define block_size 5
#define GRID_COLOR 0x08 //to see the actual grid


#define x_offset ((width_screen - GRID_WIDTH * block_size) / 2)
#define y_offset ((height_screen - (GRID_HEIGHT - 2) * block_size) / 2)


char grid[GRID_HEIGHT][GRID_WIDTH];  // [row][col]

typedef struct {
    char name; //id of the piece
    int x, y;      // top-left position on the screen / grid
    char color;    // VGA color of the piece
    int shape[4][4]; // 4x4 grid representing the shape (1 = block, 0 = empty)
} shape;

void draw_grid(volatile char *VGA){
    // skipping the first 2 rows as they are hidden
    for (int row = 2; row < GRID_HEIGHT; row++){
        for (int col = 0; col < GRID_WIDTH; col++){
            char color = grid[row][col];
            if (!color) color = GRID_COLOR; // use gray for empty cells

            int pixel_index = (y_offset + (row - 2) * block_size) * width_screen +
                              (x_offset + col * block_size);

            for (int y = 0; y < block_size; y++) {
                for (int x = 0; x < block_size; x++){
                    // Draw border: only draw a single pixel at the top/left edges
                    if (y == 0 || x == 0)
                        VGA[pixel_index + y * width_screen + x] = 0x08; // outline
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
    
shape J = {1, 3, 0, 0x03, {
    {0,1,0,0},
    {0,1,0,0},
    {1,1,0,0},
    {0,0,0,0}
}};

shape L = {2, 6, 0, 0xEC, {
    {1,0,0,0},
    {1,0,0,0},
    {1,1,0,0},
    {0,0,0,0}
}};

shape O = {3, 1, 6, 0xFC, {
        {1,1,0,0},
        {1,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
    }
    };


shape S = {4, 5, 10, 0x1C, {
        {1,0,0,0},
        {1,1,0,0},
        {0,1,0,0},
        {0,0,0,0}
    }};

shape T = {5, 2, 15, 0x63, {
        {0,1,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    }};


shape Z = {6, 7, 18, 0xE0 , {
    {1,1,0,0},
    {0,1,1,0},
    {0,0,0,0},
    {0,0,0,0}
}};








int main() {
    // VGA framebuffer base address
    volatile char *VGA = (volatile char*) 0x08000000;
    
    draw_shape(VGA, &T);
    draw_shape(VGA, &O);
    draw_shape(VGA, &I);
    draw_shape(VGA, &S);
    draw_shape(VGA, &Z);
    draw_shape(VGA, &L);
    draw_shape(VGA, &J);

    draw_grid(VGA);
    while(1) {}

}



