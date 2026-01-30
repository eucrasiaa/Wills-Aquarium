#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>

#include "art.h"
#include "colors.h"

#define MAX_FISH 10



static inline void startRawMode() {
  struct termios raw;
  tcgetattr(STDIN_FILENO, &raw);
  raw.c_lflag &= ~(ECHO | ICANON); 
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
  // clear screen 
}

//cell structure to hold character and color info
typedef struct {
  char ch;
  uint16_t colState; // fg & bg color and attribute state
  bool dirty;
} cell;

// fish template
typedef struct {
  const char **frames;
  int numFrames;
  int width;
  int height;
  char *species_name;
  bool animated;
} fishTemplate;


typedef enum {
  SPECIES_BASIC_FISH,
  SPECIES_ANIM_FISH,
  SPECIES_WIDE_FISH,
  SPECIES_MAX 
} speciesID;

extern fishTemplate species_library[SPECIES_MAX];



// proper direction enum in a c-way:
typedef enum {
  DIR_RIGHT,
  DIR_LEFT
} Direction;

typedef struct {
  fishTemplate* template;
  int x, y; // allow negatives to support wrapping behaviour?
  unsigned int curFrame;
  Direction direction; // OF DRAWING, NOT MOVEMENT
  int dx, dy;
  uint16_t colorAttr; // color attribute for this fish
  unsigned int frameCounter; // to control animation speed?
  unsigned int frameDelay; // frames to wait before advancing animation
  int bounceCount;
  int wrap; // 0 no, 1 = yes
  
} Fish;

typedef struct {
    Fish inhabitants[MAX_FISH];
    unsigned int fishCount;
    //other inhabs + count 
} Aquarium;
typedef struct {
  unsigned int width;
  unsigned int height;
  cell *nextBuffer;
  cell *liveBuffer;
  bool running;
  Aquarium *aquarium; 
  bool dbug_ForceFullRedraw;
} termContext;

void setBuffToEmpty(cell* buffer, unsigned int width, unsigned int height);
void buffEmptyHelper(termContext *ctx, bool liveBuffer);
void setCursorPosition(int row, int col);
termContext* initTermCntxt();
void cleanupTermCntxt(termContext* ctx);
void cleanupAquarium(Aquarium* aq);
void dbug_printTermCntxt(termContext* ctx);
void dbug_aquariumState(Aquarium* aq);
void dbug_fishState(Fish* fish);




void writeFishToBuffer(termContext* ctx, Fish* fish);
void writeFishHelper(termContext* ctx);

void updateFish(Fish* fish, unsigned int width, unsigned int height);
void updateAquarium(Aquarium* aq, unsigned int width, unsigned int height);
void updateStep(termContext* ctx);

void drawBuffer(termContext* ctx);
// terminal context
