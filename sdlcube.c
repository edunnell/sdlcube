#include <SDL2/SDL.h>
#include <math.h>

#define X_RESOLUTION 1600
#define Y_RESOLUTION 900
#define X_MID X_RESOLUTION/2
#define Y_MID Y_RESOLUTION/2

struct Vertex {
  float x;
  float y;
  float z;
};

FILE * fp;

void draw(SDL_Renderer * renderer, struct Vertex face[4]) {
  int i;
  for(i = 0; i < 4; ++i) {
    struct Vertex vertex = face[i];
    int vertex2i = (i == 3 ? i-3 : i+1);
    struct Vertex vertex2 = face[vertex2i];
    fprintf(fp, "x1=%g|y1=%g|x2=%g|y2=%g\n", vertex.x, vertex.y, vertex2.x, vertex2.y);
    SDL_RenderDrawLine(renderer, vertex.x, vertex.y, vertex2.x, vertex2.y);
  }
}

void convert_sdl_to_graph(float * x, float * y) {
  *x = *x - X_MID;
  *y = Y_MID - *y;
}

void convert_graph_to_sdl(float * x, float * y) {
  *x = *x + X_MID;
  *y = Y_MID - *y;
}

void rotate_vertex_clockwise_z(float * x, float * y, float degrees) {
  float angle_radians = degrees * (M_PI/180);
  float sin_angle = sin(angle_radians);
  float cos_angle = cos(angle_radians);
  float tx = *x;
  float ty = *y;

  convert_sdl_to_graph(&tx, &ty);

  float txc = tx * cos_angle;
  float tys = ty * sin_angle;
  float txs = tx * sin_angle;
  float tyc = ty * cos_angle;

  tx = txc - tys;
  ty = txs + tyc;

  float sx = tx;
  float sy = ty;
  convert_graph_to_sdl(&tx, &ty);

  *x = tx;
  *y = ty;
}

void rotate_vertex_clockwise_y(float * x, float * z, float degrees) {
  float angle_radians = degrees * (M_PI/180);
  float sin_angle = sin(angle_radians);
  float cos_angle = cos(angle_radians);
  float tx = *x;
  float tz = *z;

  tx = tx - X_MID;

  float txc = tx * cos_angle;
  float tzs = tz * sin_angle;
  float txs = tx * sin_angle;
  float tzc = tz * cos_angle;

  tx = tzs + txc;
  tz = tzc - txs;

  *x = tx + X_MID;
  *z = tz;
}

void rotate_vertex_clockwise_x(float * y, float * z, float degrees) {
  float angle_radians = degrees * (M_PI/180);
  float sin_angle = sin(angle_radians);
  float cos_angle = cos(angle_radians);
  float ty = *y;
  float tz = *z;

  ty = Y_MID - ty;

  float tyc = ty * cos_angle;
  float tzs = tz * sin_angle;
  float tys = ty * sin_angle;
  float tzc = tz * cos_angle;

  ty = tyc - tzs;
  tz = tys + tzc;

  *y = Y_MID - ty;
  *z = tz;
}

void rotate(struct Vertex face[4]) {
  int i;
  for(i = 0; i < 4; ++i) {
    rotate_vertex_clockwise_z(&face[i].x, &face[i].y, 1);
    rotate_vertex_clockwise_y(&face[i].x, &face[i].z, 1);
    rotate_vertex_clockwise_x(&face[i].y, &face[i].z, 1);
  }
}

int main() {

  fp = fopen("/tmp/cube.log", "w+");

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window * window = SDL_CreateWindow("Cube", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, X_RESOLUTION, Y_RESOLUTION, SDL_WINDOW_OPENGL);
  SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  SDL_bool done = SDL_FALSE;

  int x_sub = X_MID-75;
  int y_sub = Y_MID-75;
  int x_add = X_MID+75;
  int y_add = Y_MID+75;

  struct Vertex front[4] = {
    {x_sub, y_sub, 75},
    {x_sub, y_add, 75},
    {x_add, y_add, 75},
    {x_add, y_sub, 75}
  };

  struct Vertex back[4] = {
    {x_sub, y_sub, -75},
    {x_sub, y_add, -75},
    {x_add, y_add, -75},
    {x_add, y_sub, -75}
  };

  struct Vertex left[4] = {
    {x_sub, y_sub, -75},
    {x_sub, y_add, -75},
    {x_sub, y_add, 75},
    {x_sub, y_sub, 75}
  };

  struct Vertex right[4] = {
    {x_add, y_sub, 75},
    {x_add, y_add, 75},
    {x_add, y_add, -75},
    {x_add, y_sub, -75}
  };

  struct Vertex top[4] = {
    {x_sub, y_sub, -75},
    {x_sub, y_sub, 75},
    {x_add, y_sub, 75},
    {x_add, y_sub, -75}
  };

  struct Vertex bottom[4] = {
    {x_sub, y_add, -75},
    {x_sub, y_add, 75},
    {x_add, y_add, 75},
    {x_add, y_add, -75}
  };

  while(!done) {
    SDL_Event event;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 50, 50, SDL_ALPHA_OPAQUE);
    draw(renderer, front);

    SDL_SetRenderDrawColor(renderer, 50, 255, 50, SDL_ALPHA_OPAQUE);
    draw(renderer, back);

    SDL_SetRenderDrawColor(renderer, 50, 50, 255, SDL_ALPHA_OPAQUE);
    draw(renderer, left);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    draw(renderer, right);

    SDL_SetRenderDrawColor(renderer, 180, 180, 180, SDL_ALPHA_OPAQUE);
    draw(renderer, top);

    SDL_SetRenderDrawColor(renderer, 20, 30, 40, SDL_ALPHA_OPAQUE);
    draw(renderer, bottom);

    SDL_RenderPresent(renderer);
    rotate(front);
    rotate(back);
    rotate(left);
    rotate(right);
    rotate(top);
    rotate(bottom);

    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_QUIT) {
        done = SDL_TRUE;
      }
    }

    SDL_Delay(25);
  }

  if(renderer)
    SDL_DestroyRenderer(renderer);
  if(window)
    SDL_DestroyWindow(window);

  fclose(fp);

  SDL_Quit();
  return 0;

}
