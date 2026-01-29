#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <termios.h>
#include <unistd.h>
#include "art.h"
#include <time.h>
 #include <sys/ioctl.h>
#define MAXFISH 10

#define A_ESC "\033["
#define A_RESET "\033[0m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"
#define WHITE "\033[1;37m"
#define BLACK "\033[1;30m"
void startRawMode() {
  struct termios raw;
  tcgetattr(STDIN_FILENO, &raw);
  raw.c_lflag &= ~(ECHO | ICANON); 
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
  // clear screen 


}

typedef enum {
  SPECIES_BASIC_FISH,
  SPECIES_ANIM_FISH,
  SPECIES_MAX 
} speciesID;


typedef struct {
  char **frames; 
  int num_frames;
  int width;
  int height;
  char *species_name; 
} fishTemplate;

fishTemplate species_library[SPECIES_MAX] = {
  [SPECIES_BASIC_FISH] = { .species_name = "Basic Fish", .width = 5, .height = 1, .num_frames = 1, .frames = (char **)basic_fish_anim },
  [SPECIES_ANIM_FISH]  = { .species_name = "Animated Fish", .width = 5, .height = 1, .num_frames = 3, .frames = (char **)anim_fish_anim }

};

typedef struct {
  fishTemplate* template;
  unsigned int x, y;
  unsigned int current_frame;
  int direction; 
  int dx, dy;
} Fish;

typedef struct {
  char ch;
  uint8_t fg_color;
  uint8_t bg_color;
} cell;

typedef struct {
    Fish inhabitants[MAXFISH];
} Aquarium;

typedef struct {
  unsigned int width;
  unsigned int height;
  cell *back_buffer;
  cell *front_buffer;
  uint8_t is_running;
  uint8_t force_full_redraw;
  Aquarium aquarium;
} termContext;

int wSetTermClear(){
    printf(A_ESC "2J"); // clear screen
    printf(A_ESC "H");  // reset cursor
    fflush(stdout);
    return 0;
}

void updateFish(Fish* fishy, unsigned int screen_width, unsigned int screen_height) {
  // Update position
  fishy->x += fishy->dx * fishy->direction;
  fishy->y += fishy->dy;
  // Bounce off walls
  if (fishy->x >= screen_width - fishy->template->width || fishy->x <= 0) {
    fishy->direction *= -1; // reverse direction
  }
}


// what this does is draw to the back buffer only if the character is not a space
void draw_to_buffer( termContext *ctx, int x, int y, char ch, int color) {
    if (ch == ' ') return;
    // if fish is in bounds, draw to back buffer
    if (x >= 0 && x < ctx->width && y >= 0 && y < ctx->height) {
        int index = y * ctx->width + x;
        ctx->back_buffer[index].ch = ch;
        ctx->back_buffer[index].fg_color = color;
  }
}


void cleanup(termContext* ctx) {
  free(ctx->back_buffer);
  free(ctx->front_buffer);
}


double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}
int main(){
  startRawMode();
  termContext ctx;

  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  //
  ctx.width = w.ws_col;
  ctx.height = w.ws_row;
  ctx.back_buffer = (cell *)calloc(ctx.width * ctx.height, sizeof(cell));
  ctx.front_buffer = (cell *)calloc(ctx.width * ctx.height, sizeof(cell));
  ctx.is_running = 1;
  ctx.force_full_redraw = 1;
  wSetTermClear();
  printf("screen is %dx%d\n", ctx.width, ctx.height);
  //initialize aquarium 
  for (int i = 0; i < MAXFISH; i++) {
    ctx.aquarium.inhabitants[i].template = NULL;
    ctx.aquarium.inhabitants[i].x = 0;
    ctx.aquarium.inhabitants[i].y = 0;
    ctx.aquarium.inhabitants[i].current_frame = 0;
    ctx.aquarium.inhabitants[i].direction = 1;
    ctx.aquarium.inhabitants[i].dx = 0;
    ctx.aquarium.inhabitants[i].dy = 0;
  }

  // add fish to Aquarium
  // Fish 1 
  ctx.aquarium.inhabitants[0].template = &species_library[SPECIES_BASIC_FISH];
  ctx.aquarium.inhabitants[0].x = 5;
  ctx.aquarium.inhabitants[0].y = 5;
  ctx.aquarium.inhabitants[0].current_frame = 0;
  ctx.aquarium.inhabitants[0].direction = 1;
  ctx.aquarium.inhabitants[0].dx = 1;
  ctx.aquarium.inhabitants[0].dy = 0;


  double last_time = get_time();

  while (ctx.is_running) {
    // deltatime
    double current_time = get_time();
    double delta_time = current_time - last_time;
    last_time = current_time;
   
    for (int i = 0; i < MAXFISH; i++) {
      Fish *fish = &ctx.aquarium.inhabitants[i];
      if (fish->template != NULL) {
        updateFish(fish, ctx.width, ctx.height);
      }
    }
    // clear back buffer 
    for (unsigned int i = 0; i < ctx.width * ctx.height; i++) {
      ctx.back_buffer[i].ch = ' ';
      ctx.back_buffer[i].fg_color = 37; // default white
      ctx.back_buffer[i].bg_color = 30; // default black
    }
    // draw fish to back buffer 
    for (int i = 0; i < MAXFISH; i++) {
      Fish *fish = &ctx.aquarium.inhabitants[i];
      printf("Drawing fish %d at (%d, %d)\n", i, fish->x, fish->y);
      if(fish != NULL){
      if (fish->template != NULL) {
        char *frame = fish->template->frames[fish->current_frame];
        for (int fx = 0; fx < fish->template->width; fx++) {
          draw_to_buffer(&ctx, fish->x + fx, fish->y, frame[fx], 32); // green color
        }
      }
      }
    }
    // swap buffers
    cell *temp = ctx.front_buffer;
    ctx.front_buffer = ctx.back_buffer;
    ctx.back_buffer = temp;
    // render front buffer to terminal
    printf(A_ESC "H"); // move cursor to home
    for (unsigned int y = 0; y < ctx.height; y++) {
      for (unsigned int x = 0; x < ctx.width; x++) {
        int index = y * ctx.width + x;
        cell *c = &ctx.front_buffer[index];
        printf("\033[%dm%c", c->fg_color, c->ch);
      }
    }
    fflush(stdout);
    // simple frame delay
    usleep(100000); // 100 ms
  }
  cleanup(&ctx);
  return 0;
}
