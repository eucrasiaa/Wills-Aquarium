#pragma once
#include <stdint.h>


#define A_ESC "\033["
// Attributes
#define ATTR_BOLD  (1 << 8)  // 00000001 00000000
#define ATTR_BLINK (1 << 9)  // 00000010 00000000

// masks. 0-3 fg, 4-7 bg, 8 is bold, 9 is blink
#define FG_MASK    0x0F      // 00000000 00001111
#define BG_MASK    0xF0      // 00000000 11110000
// so cell color is a uint16_t
typedef enum {
    COLOR_BLACK = 0,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_CYAN,
    COLOR_WHITE,
    COLOR_BRIGHT_BLACK,
    COLOR_BRIGHT_RED,
    COLOR_BRIGHT_GREEN,
    COLOR_BRIGHT_YELLOW,
    COLOR_BRIGHT_BLUE,
    COLOR_BRIGHT_MAGENTA,
    COLOR_BRIGHT_CYAN,
    COLOR_BRIGHT_WHITE,
    COLOR_RESET,
    COLOR_COUNT,  // i quite like this trick as you'll see :3
    COLOR_DEFAULT = 9    // Standard terminal default
} ColorID;

//2 default color ints
// white text, no bkgd, no Attributes
#define DEFAULT_COLOR_STATE  (COLOR_WHITE & FG_MASK)
// green text, white bkgd, no Attributes
#define ALT_DEFAULT_COLOR_STATE  ((COLOR_WHITE << 4) & BG_MASK | (COLOR_GREEN & FG_MASK))


//sets fg
static inline uint16_t set_fg(uint16_t attr, ColorID color) {
    return (attr & ~FG_MASK) | (color & FG_MASK);
}

// sets bg
static inline uint16_t set_bg(uint16_t attr, ColorID color) {
    return (attr & ~BG_MASK) | ((color << 4) & BG_MASK);
}

// sets bold
static inline uint16_t set_bold(uint16_t attr, int enable) {
    return enable ? (attr | ATTR_BOLD) : (attr & ~ATTR_BOLD);
}
// sets blink
static inline uint16_t set_blink(uint16_t attr, int enable) {
    return enable ? (attr | ATTR_BLINK) : (attr & ~ATTR_BLINK);
}

void applyAttrib(uint16_t attr);

int wSetTermClear();
