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
  
  //fish2 
  ctx->aquarium->inhabitants[1].template = &species_library[SPECIES_ANIM_FISH];
  ctx->aquarium->inhabitants[1].x = 10;
  ctx->aquarium->inhabitants[1].y = 10;
  ctx->aquarium->inhabitants[1].curFrame = 0;
  ctx->aquarium->inhabitants[1].direction = DIR_RIGHT;
  ctx->aquarium->inhabitants[1].frameDelay = 5; // slower animation
  ctx->aquarium->inhabitants[1].dx = 1;
  ctx->aquarium->inhabitants[1].dy = 0;
  ctx->aquarium->fishCount++;

  dbug_printTermCntxt(ctx);
  for (int i = 0; i < 100; i++){
    updateStep(ctx);
    //sleep a bit
    usleep(100000); // 100ms
  }
  // updateStep(ctx); 
  cleanupTermCntxt(ctx);
  return 0;
}
