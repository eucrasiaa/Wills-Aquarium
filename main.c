// #include "main.h"
#include "utils.h"

int main(){

  startRawMode();
  wSetTermClear();
  printf("color demo\n");
  termContext* ctx = initTermCntxt();
  //add fish debug
  //fish1
  ctx->aquarium->inhabitants[0].template = &species_library[SPECIES_BASIC_FISH];
  ctx->aquarium->inhabitants[0].x = 50;
  ctx->aquarium->inhabitants[0].y = 5;
  ctx->aquarium->inhabitants[0].curFrame = 0;
  ctx->aquarium->inhabitants[0].direction = DIR_LEFT;

  ctx->aquarium->inhabitants[0].dx = -1;
  ctx->aquarium->inhabitants[0].dy = 0;
  ctx->aquarium->fishCount++;
    ctx->aquarium->inhabitants[0].wrap = 1;
  //fish2 
  ctx->aquarium->inhabitants[1].template = &species_library[SPECIES_ANIM_FISH];
  ctx->aquarium->inhabitants[1].x = 10;
  ctx->aquarium->inhabitants[1].y = 10;
  ctx->aquarium->inhabitants[1].curFrame = 0;
  ctx->aquarium->inhabitants[1].direction = DIR_LEFT;

  ctx->aquarium->inhabitants[1].frameDelay = 5; // slower animation
  ctx->aquarium->inhabitants[1].dx = -1; 
  ctx->aquarium->inhabitants[1].dy = 0;
  ctx->aquarium->fishCount++;
  ctx->aquarium->inhabitants[1].wrap = 1;

  //wide fish
  ctx->aquarium->inhabitants[2].template = &species_library[SPECIES_WIDE_FISH];
  ctx->aquarium->inhabitants[2].x = 30;
  ctx->aquarium->inhabitants[2].y = 15;
  ctx->aquarium->inhabitants[2].curFrame = 0;

  ctx->aquarium->inhabitants[2].direction = DIR_LEFT;
  ctx->aquarium->inhabitants[2].dx = -2;
  ctx->aquarium->inhabitants[2].dy = 0;
  ctx->aquarium->inhabitants[2].wrap = 1;
  ctx->aquarium->fishCount++;
  //pass colorAttr to set_fg to change fish color for all
  //
  ctx->aquarium->inhabitants[2].colorAttr = set_fg(DEFAULT_COLOR_STATE, COLOR_CYAN);
  ctx->aquarium->inhabitants[1].colorAttr = set_fg(DEFAULT_COLOR_STATE, COLOR_YELLOW);
  ctx->aquarium->inhabitants[0].colorAttr = set_fg(DEFAULT_COLOR_STATE, COLOR_MAGENTA);
  



  dbug_printTermCntxt(ctx);
  usleep(1500000); // 2s pause before starting main loop
  wSetTermClear();
  while(ctx->running){
    updateStep(ctx);
    //sleep a bit
    usleep(100000); // 100ms
  }
  // updateStep(ctx); 
  cleanupTermCntxt(ctx);
  return 0;
}
