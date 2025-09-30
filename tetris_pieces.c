typedef struct {
    int x, y;      // top-left position on the screen / grid
    char color;    // color of the piece
    int shape[4][4]; // 4x4 grid representing the shape (1 = block, 0 = empty)
} shape;


void draw_shape(volatile char *VGA, shape *t, int block_size){
    // pointer to VGA memory, pointer to tetris shape, how big
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
    // loops throught 4x4 piece
            if (t->shape[x][y]){ 
    //checks for 0 and 1
            int pixel_index = (t->y + y*block_size) * 320 + (t->x + block_size *x);
    //gets top-left corner of piece, shifts to the right         
    
    for (int row = 0; row < block_size; row++) {
    // counts vertical pixels inside the block
        for (int col = 0; col < block_size; col++) {
    // counts horizontal pixels inside the block
            VGA[pixel_index + row * 320 + col] = t->color;
        }
    }
    
                 }
            }
        }
    }



int main() {
    // VGA framebuffer base address
    volatile char *VGA = (volatile char*) 0x08000000;
    int block_size = 10;

    shape T = {50, 80, 5, {
        {0,1,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    }};

    shape O = { 60, 90, 10, {
        {0,0,0,0},
        {0,1,1,0},
        {0,1,1,0},
        {0,0,0,0}
    }
    };

    shape I = { 70, 100, 2, {
        {0,0,1,0},
        {0,0,1,0},
        {0,0,1,0},
        {0,0,1,0}
    }
    };



    draw_shape(VGA, &T, block_size);
    draw_shape(VGA, &O, block_size);
    draw_shape(VGA, &I, block_size);

    while(1) {}

}

