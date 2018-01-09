#include <SDL2/SDL.h>
#include <math.h>

struct Vertex {
  float x;
  float y;
  int z;
};

FILE * fp;

void draw(SDL_Renderer * renderer, struct Vertex face[4]) {
  int i;
  for(i = 0; i < 4; ++i) {
    struct Vertex vertex = face[i];
    int vertex2i = (i == 3 ? i-3 : i+1);
    struct Vertex vertex2 = face[vertex2i];
    fprintf(fp, "x1=%g - y1=%g - x2=%g - y2=%g\n", vertex.x, vertex.y, vertex2.x, vertex2.y);
    SDL_RenderDrawLine(renderer, vertex.x, vertex.y, vertex2.x, vertex2.y);
  }
}

convert_sdl_to_graph(float * x, float * y) {
  *x = *x - 400;
  *y = 300 - *y;
}

convert_graph_to_sdl(float * x, float * y) {
  *x = *x + 400;
  *y = 300 - *y;
}

void rotate_vertex_clockwise(float * x, float * y, float degrees) {
  float angle_radians = degrees * (M_PI/180);
  float sin_angle = sin(angle_radians);
  float cos_angle = cos(angle_radians);
  float tx = *x;
  float ty = *y;
  
  /* *x -= 400; */
  /* *y -= 300; */
  convert_sdl_to_graph(&tx, &ty);
  fprintf(fp, "s=%f c=%f\n", sin_angle, cos_angle);
  fprintf(fp, "sdl: x=%g - y=%g graph: x=%g - y=%g\n", *x, *y, tx, ty);

  float txc = tx * cos_angle;
  float tys = ty * sin_angle;
  float txs = tx * sin_angle;
  float tyc = ty * cos_angle;

  tx = txc - tys;
  ty = txs + tyc;

  fprintf(fp, "txc=%g tys=%g txs=%g tyc=%g rtx=%g rty=%g\n", txc, tys, txs, tyc, tx, ty);
  fprintf(fp, "tx=%g ty=%g\n", tx, ty);
  /* *x += 400; */
  /* *y += 300; */
  float sx = tx;
  float sy = ty;
  convert_graph_to_sdl(&tx, &ty);
  fprintf(fp, "sdl: x=%g - y=%g graph: x=%g - y=%g\n\n\n", tx, ty, sx, sy);

  *x = tx;
  *y = ty;
}

void rotate(struct Vertex face[4]) {
  int i;
  for(i = 0; i < 4; ++i) {
    rotate_vertex_clockwise(&face[i].x, &face[i].y, 30);
  }
}

int main() {

  fp = fopen("/tmp/test.txt", "w+");

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window * window = SDL_CreateWindow("Cube", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL);
  SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  SDL_bool done = SDL_FALSE;

  struct Vertex face[4] = {
    {325, 225, 0},
    {325, 375, 0},
    {475, 375, 0},
    {475, 225, 0}
  };
   
  /* float x = 402; */
  /* float y = 300; */

  while(!done) {
    SDL_Event event;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    /* SDL_SetRenderDrawColor(renderer, 10, 0, 250, SDL_ALPHA_OPAQUE); */
    /* SDL_RenderDrawLine(renderer, 390, 300, 410, 300); */
    /* SDL_RenderDrawLine(renderer, 400, 290, 400, 310); */
    
    SDL_SetRenderDrawColor(renderer, 150, 255, 50, SDL_ALPHA_OPAQUE);

    draw(renderer, face);

    /* SDL_RenderDrawPoint(renderer, x, y); */
    
    SDL_RenderPresent(renderer);
    rotate(face);

    /* rotate_vertex_clockwise(&x, &y, 45); */

    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_QUIT) {
        done = SDL_TRUE;
      }
    }
    
    SDL_Delay(500);
    
  }

  if(renderer)
    SDL_DestroyRenderer(renderer);
  if(window)
    SDL_DestroyWindow(window);

  fclose(fp);
  
  SDL_Quit();  
  return 0;

}
