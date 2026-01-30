#include <stdio.h>
#include "colors.h"

void applyAttrib(uint16_t attr) {
    uint8_t fg = attr & FG_MASK;
    uint8_t bg = (attr & BG_MASK) >> 4;

    // Start with a reset to clear previous state
    // Then apply Bold (1), FG (30+), and BG (40+)
    printf("\033[0;%d;%d;%dm", 
           (attr & ATTR_BOLD) ? 1 : 22, 
           30 + fg, 
           40 + bg);

    if (attr & ATTR_BLINK) printf("\033[5m");
}
int wSetTermClear(){
    printf(A_ESC "2J"); // clear screen
    printf(A_ESC "H");  // reset cursor
    fflush(stdout);
    return 0;
}
