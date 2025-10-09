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
/*
int get_btn(void){
  volatile int* btn = (volatile int*)0x040000d0;
  return ((*btn) & 0x1);
}





int get_xrow(void){
    volatile int* x_axis = (volatile int*)0x040000e0;
}

int get_ycol(void){
    volatile int* y_axis = (volatile int*)0x040000e0;
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

    sw = get_sw();
    if (get_btn()){ //If it's on
      //int sw = get_sw();
      int top_switches = (sw >> 8) & 0x3;
      int data_switches = sw & 0x3F;
      
      switch(top_switches){
        case 0x1:
          set_displays(0, data_switches % 10);
          set_displays(1, data_switches / 10);
          break;

        case 0x2:
          set_displays(2, data_switches % 10);
          set_displays(3, data_switches / 10);
          break;

        case 0x3:
          set_displays(4, data_switches % 10);
          set_displays(5, data_switches / 10);
          break;
      }
        }

        */