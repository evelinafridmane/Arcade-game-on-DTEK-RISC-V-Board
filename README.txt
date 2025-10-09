TETRIS GAME - DTEK-V IMPLEMENTATION FOR IS1200

DESCRIPTION:
This project contains the full implementation of the classic Tetris game for it to be run on the DTEK-board.
The game features falling pieces, rotation, movement, line clearing, score tracking and next-piece preview.

AUTHORS:
Paola Claudio Fonseca (20061213-5426)
Evelina Fridmane (20050527-7004)

FEATURES:
-VGA graphical display with 10x20 game grid
-Joystick control for left/right movement (x-axis = left, y-axis = right)
-Switch 0: rotate piece
-Switch 1: instant drop
-Score displayed on 7-segment displays
-Next piece preview
-Line clearing and scoring
-Game over detection

COMPILATION INSTRUCTIONS:

1. Build with no optimisations:
Run in the wsl terminal "make"

2.Build with heavy optimisations:
Modify "Makefile" line 8 from -O0 to -O3
Save the file
Run in the wsl terminal "make"

3.Perfomance testing:
Comment out in "main.c" the line "measure_performance()" in the main function.


RUNNING THE GAME:
1.Compile the game using any of the commands above
2.Make sure the DTEK-V board is connected and run the game using
"dtekv-run main.bin"

CONTROLS:
-Joystick left: (x-axis = left) moves the piece to the left
-Joystick right: (y-axis = right) moves the piece to the right
-Switch 0: rotate piece clocwise
-Switch 1: instant drop piece

CONNECTING THE JOYSTICK TO GPIO:
GND pin to GND in corresponding GPIO port
5V pin to 5V in corresponding GPIO port 
VRX pin to GPIO 0 
VRY pin to GPIO 1
SW pin to GPIO 2

SCORING:
- Hard drop: +50 points
- Line clear: +100 points per line


FILES:
- main.c - main function with performance measure
- tetris.h / .c Organised files with all game mechanics, initialisations, features, etc.
- performance.h / .c Perfomance counter functions
- boot.S - file from Lab for enabling timer interrupts
- Makefile, COPYING, dtekv-lib.h /.c, dtekv-script.lds, softfloat.a from Labs
- draft.c file which contains the game, without it being organised thus being commented out


NOTES: 
Since the DTEK-V board only reads digital signals on the GPIO, the analog joystick had to handle x-axis a
nd y-axis to decide whether to move left or right.