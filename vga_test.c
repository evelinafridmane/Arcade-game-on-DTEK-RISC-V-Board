/*void handle_interrupt(unsigned cause) 
{}


void labinit(void)
{}
int main() {
    // VGA framebuffer base address
    volatile char *VGA = (volatile char*) 0x08000000;

    // Define block size and position
    int block_x = 50;   // X position (column)
    int block_y = 80;   // Y position (row)
    int block_size = 10;
    char color = 5;     // some arbitrary color value

    // Draw a 10x10 block at (50,80)
    for (int y = 0; y < block_size; y++) {
        for (int x = 0; x < block_size; x++) {
            int pixel_index = (block_y + y) * 320 + (block_x + x);
            VGA[pixel_index] = color;
        }
    }

    while (1) {
        // Do nothing, just keep block on screen
    }
}

*/