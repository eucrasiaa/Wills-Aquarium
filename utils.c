#include "utils.h"
#include <string.h>

void setCursorPosition(int row, int col){
  printf(A_ESC "%d;%dH", row, col);
  fflush(stdout);
}
void cleanupAquarium(Aquarium* aq) {
  free(aq);
}
void cleanupTermCntxt(termContext* ctx) {
  free(ctx->nextBuffer);
  free(ctx->liveBuffer);
  cleanupAquarium(ctx->aquarium);
  free(ctx);
}

fishTemplate species_library[SPECIES_MAX] = {
  [SPECIES_BASIC_FISH] = { .species_name = "Basic Fish", .width = 6, .height = 1, .numFrames = 1, .animated = false, .frames = (const char **)basic_fish_anim },
  [SPECIES_ANIM_FISH]  = { .species_name = "Animated Fish", .width = 6, .height = 1, .numFrames = 3, .animated = true, .frames = (const char **)anim_fish_anim },
 [SPECIES_WIDE_FISH]  = { .species_name = "Wide Fish", .width = 4, .height = 4, .numFrames = 1, .animated = false, .frames = (const char **)wide_fish_anim }
};

termContext* initTermCntxt() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  unsigned int width = w.ws_col;
  unsigned int height = w.ws_row;
  termContext* ctx = (termContext*)malloc(sizeof(termContext));
  ctx->width = width;
  ctx->height = height;
  ctx->nextBuffer = (cell *)calloc(ctx->width * ctx->height, sizeof(cell));
  ctx->liveBuffer = (cell *)calloc(ctx->width * ctx->height, sizeof(cell));

  ctx->running = true;  
  ctx->dbug_ForceFullRedraw = false;
  ctx->aquarium = (Aquarium*)malloc(sizeof(Aquarium));
  ctx->aquarium->fishCount = 0; // initialize fish count to 0
  // init all fish to NULL
  for (unsigned int i = 0; i < MAX_FISH; i++) {
    ctx->aquarium->inhabitants[i].template = NULL;
    ctx->aquarium->inhabitants[i].x = 0;
    ctx->aquarium->inhabitants[i].y = 0;
    ctx->aquarium->inhabitants[i].curFrame = 0;
    ctx->aquarium->inhabitants[i].direction = DIR_RIGHT;
    ctx->aquarium->inhabitants[i].colorAttr = DEFAULT_COLOR_STATE;
    ctx->aquarium->inhabitants[i].dx = 0;
    ctx->aquarium->inhabitants[i].dy = 0;
    ctx->aquarium->inhabitants[i].frameCounter = 0;
    ctx->aquarium->inhabitants[i].frameDelay = 0; 
    ctx->aquarium->inhabitants[i].bounceCount = 0;
    ctx->aquarium->inhabitants[i].wrap = 0;
  }
  buffEmptyHelper(ctx, false);
  buffEmptyHelper(ctx, true);
  return ctx;
}


void setBuffToEmpty(cell* buffer, unsigned int width, unsigned int height){
  for(unsigned int row = 0; row < height; row++){
    for(unsigned int col = 0; col < width; col++){
      unsigned int index = row * width + col;
      buffer[index].ch = ' ';
      buffer[index].colState = DEFAULT_COLOR_STATE;
      buffer[index].dirty = false;
    }
  }
}
void buffEmptyHelper(termContext *ctx, bool liveBuffer){
  if(liveBuffer){
    setBuffToEmpty(ctx->liveBuffer, ctx->width, ctx->height);
  }
  else{
    setBuffToEmpty(ctx->nextBuffer, ctx->width, ctx->height);
  }
} 
void writeFishToBuffer(termContext* ctx, Fish* fish){
  // printf("Writing fish %s to buffer at (%d, %d)\n", fish->template->species_name, fish->x, fish->y);
  //nextBuffer is a 1D array representing 2D grid, with the height and width.t
  // we'll use a double for loop to write fish frame, but set initial posion based on fish x,y
  // first off, determine "bounding box" of the fish's frame. top left and bottom Right
  int startX = fish->x;
  int startY = fish->y;
  char* frame = fish->template->frames[fish->curFrame];
  int bottomY = startY + fish->template->height;
  int rightX = startX + fish->template->width;
  uint16_t colorAttr = fish->colorAttr;
  // check if we need to reverse fish direction, and inverse the frame by chunks of Width
  // printf(" Fish frame bounding box: TopLeft(%d, %d) 
        // BottomRight(%d, %d)\n", startX, startY, rightX, bottomY);
  // printf(" Fish frame data: \n");
  // printf("%s\n", frame);
  // usleep(1000000);
  //handle wrap by determining how much fish is out of bounds on a side (in the direction of motion)
  // and drawing that part on the opposite side
  // wrap logic

  for(int row = 0; row < fish->template->height; row++){
    for(int col = 0; col < fish->template->width; col++){
      int drawX = startX + col;
      int drawY = startY + row;
      // boundary check
      if(drawX < 0 || drawX >= ctx->width || drawY < 0 || drawY >= ctx->height){
        if(fish->wrap == 1){
          // handle wrap
          if(drawX < 0){
            drawX = ctx->width + drawX; // wrap to right side
          }
          else if(drawX >= ctx->width){
            drawX = drawX - ctx->width; // wrap to left side
          }
          if(drawY < 0){
            drawY = ctx->height + drawY; // wrap to bottom
          }
          else if(drawY >= ctx->height){
            drawY = drawY - ctx->height; // wrap to top
          }
        }
        else{
          continue; // skip drawing out of bounds
        }
      }
      unsigned int index = drawY * ctx->width + drawX;
      ctx->nextBuffer[index].ch = frame[row * fish->template->width + col];
      ctx->nextBuffer[index].colState = colorAttr ; // green fish
      ctx->nextBuffer[index].dirty = true; // mark as dirty for redraw
    }
  }
  //why is this printing jibberish?
  // its because frame is a pointer to a string, need to dereference it
  //

}


void updateFish(Fish* fish, unsigned int width, unsigned int height){
  // printf("Updating fish %s at (%d, %d)\n", fish->template->species_name, fish->x, fish->y);
  // Update Position
  // basic for now 
  fish->x += fish->dx;
  fish->y += fish->dy;
  if(fish->template->animated){
    fish->frameCounter++;
    if(fish->frameCounter >= fish->frameDelay){
      fish->curFrame = (fish->curFrame + 1) % fish->template->numFrames;
      fish->frameCounter = 0;
    }
  }
  // Boundary checks
}

void updateAquarium(Aquarium* aq, unsigned int screen_width, unsigned int screen_height) {
  for (unsigned int i = 0; i < MAX_FISH; i++) {
    if (aq->inhabitants[i].template == NULL) {
      continue; // skip empty fish slots
    }
    updateFish(&aq->inhabitants[i], screen_width, screen_height);
  }
  // run writeFish for each fish to draw to nextBuffer

}

void writeFishHelper(termContext* ctx) {
  for (unsigned int i = 0; i < MAX_FISH; i++) {
    if (ctx->aquarium->inhabitants[i].template == NULL) {
      continue; // skip empty fish slots
    }
    writeFishToBuffer(ctx, &ctx->aquarium->inhabitants[i]);
  }
}
void updateStep(termContext* ctx) {
  buffEmptyHelper(ctx, false); // clear nextBuffer
  updateAquarium(ctx->aquarium, ctx->width, ctx->height);
  writeFishHelper(ctx);

  drawBuffer(ctx);
}

//TODO ALL PLACEHOLDER. DO DIRTY CHECK OPTIMIZATION!!
void drawBuffer(termContext* ctx) { 
  uint16_t last_attr = 0xFFFF; // invalid initial value to force first color set
  //iterate and only draw dirty cells, otherwise skip
  for (unsigned int row = 0; row < ctx->height; row++) {
    for (unsigned int col = 0; col < ctx->width; col++) {
      unsigned int idx = row * ctx->width + col;

      cell *next = &ctx->nextBuffer[idx];
      cell *live = &ctx->liveBuffer[idx];

      // Only draw if the character OR the color changed
      if (next->ch != live->ch || next->colState != live->colState || ctx->dbug_ForceFullRedraw) {

        setCursorPosition(row + 1, col + 1);

        // Optimization: only update color if it's different from the PREVIOUS cell
        if (next->colState != last_attr) {
          applyAttrib(next->colState);
          last_attr = next->colState;
        }

        putchar(next->ch);

        // Sync: Now the live buffer matches what we just printed
        *live = *next;
      }
    }
  }
  ctx->dbug_ForceFullRedraw = false;
  setCursorPosition(0, 0);
  fflush(stdout);

// // for now, just print the nextBuffer to the terminal
// setCursorPosition(0, 0);
// for (unsigned int row = 0; row < ctx->height; row++) {
//   for (unsigned int col = 0; col < ctx->width; col++) {
//     unsigned int index = row * ctx->width + col;
//     // use the color State
//     applyAttrib(ctx->nextBuffer[index].colState);
//     putchar(ctx->nextBuffer[index].ch);
//   }
//   putchar('\n');
// }
// // memcpy nextBuffer to liveBuffer, 
// memcpy(ctx->liveBuffer, ctx->nextBuffer, ctx->width * ctx->height * sizeof(cell));
// usleep(500000); // sleep for 5ms to reduce flicker
// fflush(stdout);
}

void dbug_printTermCntxt(termContext* ctx){
  printf("TermContext:\n");
  printf(" Width: %d\n", ctx->width);
  printf(" Height: %d\n", ctx->height);
  printf(" Running: %d\n", ctx->running);
  printf(" ForceFullRedraw: %d\n", ctx->dbug_ForceFullRedraw);
  // aquaurium is a pointer
  if(ctx->aquarium){  
    dbug_aquariumState(ctx->aquarium);
  }

  else{
    printf(" Aquarium: NULL\n");
  }

  return;

}

void dbug_aquariumState(Aquarium* aq){

  // call dbug_fishState for each fish and print fish count
  printf("Aquarium State:\n");
  // use fishcount
  for (unsigned int i = 0; i < MAX_FISH; i++){
    if(aq->inhabitants[i].template == NULL){
      continue; // skip empty fish slots
    }
    printf(" Fish %d:\n", i);
    dbug_fishState(&aq->inhabitants[i]);
  }
  printf(" Total Fish: %d\n", aq->fishCount);
}
void dbug_fishState(Fish* fish){
  printf("  Species: %s\n", fish->template->species_name);
  printf("  Position: (%d, %d)\n", fish->x, fish->y);
  printf("  Current Frame: %d\n", fish->curFrame);
  printf("  Direction: %s\n", fish->direction == DIR_RIGHT ? "Right" : "Left");
  printf("  Velocity: (dx: %d, dy: %d)\n", fish->dx, fish->dy);
  printf("  Frame Counter: %d\n", fish->frameCounter);
  printf("  Frame Delay: %d\n", fish->frameDelay);
  printf("  Bounce Count: %d\n", fish->bounceCount);
  printf("  Wraping?: %d\n", fish->wrap);
  applyAttrib(fish->colorAttr);
  printf("  Color Attribute: 0x%04X\n", fish->colorAttr);
  applyAttrib(DEFAULT_COLOR_STATE); // reset to default after printing
}
