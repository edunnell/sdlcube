#include <SDL2/SDL.h>
#include <math.h>

#define X_RESOLUTION 1600
#define Y_RESOLUTION 900
#define X_MID X_RESOLUTION/2
#define Y_MID Y_RESOLUTION/2

typedef struct Vertex {
  float x;
  float y;
  float z;
} Vertex;

typedef struct Cube {
  Vertex front[4];
  Vertex back[4];
  Vertex left[4];
  Vertex right[4];
  Vertex top[4];
  Vertex bottom[4];
} Cube;

FILE * fp;

Cube create_cube(float x, float y, float z) {

  float x_sub = x-75;
  float y_sub = y-75;
  float z_sub = z-75;
  float x_add = x+75;
  float y_add = y+75;
  float z_add = z+75;

  Cube cube = {
    {
      {x_sub, y_sub, z_add},
      {x_sub, y_add, z_add},
      {x_add, y_add, z_add},
      {x_add, y_sub, z_add}
    },
    {
      {x_sub, y_sub, z_sub},
      {x_sub, y_add, z_sub},
      {x_add, y_add, z_sub},
      {x_add, y_sub, z_sub}
    },
    {
      {x_sub, y_sub, z_sub},
      {x_sub, y_add, z_sub},
      {x_sub, y_add, z_add},
      {x_sub, y_sub, z_add}
    },
    {
      {x_add, y_sub, z_add},
      {x_add, y_add, z_add},
      {x_add, y_add, z_sub},
      {x_add, y_sub, z_sub}
    },
    {
      {x_sub, y_sub, z_sub},
      {x_sub, y_sub, z_add},
      {x_add, y_sub, z_add},
      {x_add, y_sub, z_sub}
    },
    {
      {x_sub, y_add, z_sub},
      {x_sub, y_add, z_add},
      {x_add, y_add, z_add},
      {x_add, y_add, z_sub}
    }
  };

  return cube;
}

void draw(SDL_Renderer * renderer, Vertex face[4]) {
  int i;
  for(i = 0; i < 4; ++i) {
    Vertex vertex = face[i];
    int vertex2i = (i == 3 ? i-3 : i+1);
    Vertex vertex2 = face[vertex2i];
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

void rotate(Vertex face[4]) {
  int i;
  for(i = 0; i < 4; ++i) {
    rotate_vertex_clockwise_z(&face[i].x, &face[i].y, 1);
    rotate_vertex_clockwise_y(&face[i].x, &face[i].z, 1);
    rotate_vertex_clockwise_x(&face[i].y, &face[i].z, 1);
  }
}

void rotate_face_x(Vertex face[4]) {
  int i;
  for(i = 0; i < 4; ++i) {
    rotate_vertex_clockwise_x(&face[i].y, &face[i].z, 5);
  }
}

void rotate_face_y(Vertex face[4]) {
  int i;
  for(i = 0; i < 4; ++i) {
    rotate_vertex_clockwise_y(&face[i].x, &face[i].z, 5);
  }
}

void rotate_face_z(Vertex face[4]) {
  int i;
  for(i = 0; i < 4; ++i) {
    rotate_vertex_clockwise_z(&face[i].x, &face[i].z, 5);
  }
}

void rotate_cube_x(Cube * cube) {
  rotate_face_x(cube->front);
  rotate_face_x(cube->back);
  rotate_face_x(cube->left);
  rotate_face_x(cube->right);
  rotate_face_x(cube->top);
  rotate_face_x(cube->bottom);
}

void rotate_cube_y(Cube * cube) {
  rotate_face_y(cube->front);
  rotate_face_y(cube->back);
  rotate_face_y(cube->left);
  rotate_face_y(cube->right);
  rotate_face_y(cube->top);
  rotate_face_y(cube->bottom);
}

void rotate_cube_z(Cube * cube) {
  rotate_face_z(cube->front);
  rotate_face_z(cube->back);
  rotate_face_z(cube->left);
  rotate_face_z(cube->right);
  rotate_face_z(cube->top);
  rotate_face_z(cube->bottom);
}

int main() {

  fp = fopen("/tmp/cube.log", "w+");

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window * window = SDL_CreateWindow("Cube", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, X_RESOLUTION, Y_RESOLUTION, SDL_WINDOW_OPENGL);
  SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  SDL_bool done = SDL_FALSE;

  Cube cube = create_cube(X_MID, Y_MID, 0);

  while(!done) {
    SDL_Event event;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 50, 50, SDL_ALPHA_OPAQUE);
    draw(renderer, cube.front);

    SDL_SetRenderDrawColor(renderer, 50, 255, 50, SDL_ALPHA_OPAQUE);
    draw(renderer, cube.back);

    SDL_SetRenderDrawColor(renderer, 50, 50, 255, SDL_ALPHA_OPAQUE);
    draw(renderer, cube.left);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    draw(renderer, cube.right);

    SDL_SetRenderDrawColor(renderer, 180, 180, 180, SDL_ALPHA_OPAQUE);
    draw(renderer, cube.top);

    SDL_SetRenderDrawColor(renderer, 20, 30, 40, SDL_ALPHA_OPAQUE);
    draw(renderer, cube.bottom);

    SDL_RenderPresent(renderer);
    rotate(cube.front);
    rotate(cube.back);
    rotate(cube.left);
    rotate(cube.right);
    rotate(cube.top);
    rotate(cube.bottom);

    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_QUIT) {
        done = SDL_TRUE;
        break;
      }
      switch(event.type) {
      case SDL_QUIT:
        done = SDL_TRUE;
        break;
      case SDL_KEYDOWN:
        fprintf(fp, "SDL_KEYDOWN\n");
        switch(event.key.keysym.sym) {
        case SDLK_LEFT:
          fprintf(fp, "LEFT\n");
          rotate_cube_y(&cube);
          break;
        case SDLK_RIGHT:
          fprintf(fp, "RIGHT\n");
          rotate_cube_y(&cube);
          break;
        case SDLK_UP:
          fprintf(fp, "UP\n");
          rotate_cube_x(&cube);
          break;
        case SDLK_DOWN:
          fprintf(fp, "DOWN\n");
          rotate_cube_x(&cube);
          break;
        }
        break;
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
