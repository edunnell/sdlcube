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

typedef struct Player {
  Vertex position;
  int angle;
} Player;

typedef struct Cube {
  Vertex front[4];
  Vertex back[4];
  Vertex left[4];
  Vertex right[4];
  Vertex top[4];
  Vertex bottom[4];
  Vertex center;
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
    },
    {x, y, z}
  };

  return cube;
}

void convert_sdl_to_graph(float * x, float * y) {
  *x = *x - X_MID;
  *y = Y_MID - *y;
}

void convert_graph_to_sdl(float * x, float * y) {
  *x = *x + X_MID;
  *y = Y_MID - *y;
}

void draw(SDL_Renderer * renderer, Vertex face[4], Vertex * center, Player * player) {
  int i;
  for(i = 0; i < 4; ++i) {
    Vertex vertex = face[i];
    int vertex2i = (i == 3 ? i-3 : i+1);
    Vertex vertex2 = face[vertex2i];
    convert_graph_to_sdl(&x1, &y1);
    convert_graph_to_sdl(&x2, &y2);
    fprintf(fp, "x1=%g|y1=%g|x2=%g|y2=%g\n", x1, y1, x2, y2);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
  }
}

void rotate_vertex_clockwise_z(float * x, float * y, float * center_x, float * center_y, float degrees) {
  float angle_radians = degrees * (M_PI/180);
  float sin_angle = sin(angle_radians);
  float cos_angle = cos(angle_radians);
  float sx = *x;
  float sy = *y;
  float tx = *x - *center_x;
  float ty = *y - *center_y;

  float txc = tx * cos_angle;
  float tys = ty * sin_angle;
  float txs = tx * sin_angle;
  float tyc = ty * cos_angle;

  tx = txc - tys;
  ty = txs + tyc;

  *x = tx + *center_x;
  *y = ty + *center_y;
}

void rotate_vertex_clockwise_y(float * x, float * z, float * center_x, float * center_z, float degrees) {
  float angle_radians = degrees * (M_PI/180);
  float sin_angle = sin(angle_radians);
  float cos_angle = cos(angle_radians);
  float tx = *x - *center_x;
  float tz = *z - *center_z;

  float txc = tx * cos_angle;
  float tzs = tz * sin_angle;
  float txs = tx * sin_angle;
  float tzc = tz * cos_angle;

  tx = tzs + txc;
  tz = tzc - txs;

  *x = tx + *center_x;
  *z = tz + *center_z;
}

void rotate_vertex_clockwise_x(float * y, float * z, float * center_y, float * center_z, float degrees) {
  float angle_radians = degrees * (M_PI/180);
  float sin_angle = sin(angle_radians);
  float cos_angle = cos(angle_radians);
  float ty = *y - *center_y;
  float tz = *z - *center_z;

  float tyc = ty * cos_angle;
  float tzs = tz * sin_angle;
  float tys = ty * sin_angle;
  float tzc = tz * cos_angle;

  ty = tyc - tzs;
  tz = tys + tzc;

  *y = ty + *center_y;
  *z = tz + *center_z;
}

void rotate(Vertex face[4], Vertex * center) {
  int i;
  for(i = 0; i < 4; ++i) {
    rotate_vertex_clockwise_z(&face[i].x, &face[i].y, &center->x, &center->y, 1);
    rotate_vertex_clockwise_y(&face[i].x, &face[i].z, &center->x, &center->z, 1);
    rotate_vertex_clockwise_x(&face[i].y, &face[i].z, &center->y, &center->z, 1);
  }
}

void rotate_face_x(Vertex face[4], Vertex * center) {
  int i;
  for(i = 0; i < 4; ++i) {
    rotate_vertex_clockwise_x(&face[i].y, &face[i].z, &center->y, &center->z, 5);
  }
}

void rotate_face_y(Vertex face[4], Vertex * center) {
  int i;
  for(i = 0; i < 4; ++i) {
    rotate_vertex_clockwise_y(&face[i].x, &face[i].z, &center->x, &center->z, 5);
  }
}

void rotate_face_z(Vertex face[4], Vertex * center) {
  int i;
  for(i = 0; i < 4; ++i) {
    rotate_vertex_clockwise_z(&face[i].x, &face[i].y, &center->x, &center->y, 5);
  }
}

void rotate_cube_x(Cube * cube) {
  rotate_face_x(cube->front, &cube->center);
  rotate_face_x(cube->back, &cube->center);
  rotate_face_x(cube->left, &cube->center);
  rotate_face_x(cube->right, &cube->center);
  rotate_face_x(cube->top, &cube->center);
  rotate_face_x(cube->bottom, &cube->center);
}

void rotate_cube_y(Cube * cube) {
  rotate_face_y(cube->front, &cube->center);
  rotate_face_y(cube->back, &cube->center);
  rotate_face_y(cube->left, &cube->center);
  rotate_face_y(cube->right, &cube->center);
  rotate_face_y(cube->top, &cube->center);
  rotate_face_y(cube->bottom, &cube->center);
}

void rotate_cube_z(Cube * cube) {
  rotate_face_z(cube->front, &cube->center);
  rotate_face_z(cube->back, &cube->center);
  rotate_face_z(cube->left, &cube->center);
  rotate_face_z(cube->right, &cube->center);
  rotate_face_z(cube->top, &cube->center);
  rotate_face_z(cube->bottom, &cube->center);
}

void move_face_left(Vertex face[4]) {
  int i;
  for(i = 0; i < 4; ++i) {
    face[i].x -= 5;
  }
}

void move_face_right(Vertex face[4]) {
  int i;
  for(i = 0; i < 4; ++i) {
    face[i].x += 5;
  }
}

void move_face_forward(Vertex face[4]) {
  int i;
  for(i = 0; i < 4; ++i) {
    face[i].z += 5;
  }
}

void move_face_backward(Vertex face[4]) {
  int i;
  for(i = 0; i < 4; ++i) {
    face[i].z -= 5;
  }
}

void move_left(Cube * cube) {
  move_face_left(cube->front);
  move_face_left(cube->back);
  move_face_left(cube->left);
  move_face_left(cube->right);
  move_face_left(cube->top);
  move_face_left(cube->bottom);
  cube->center.x -= 5;
}

void move_right(Cube * cube) {
  move_face_right(cube->front);
  move_face_right(cube->back);
  move_face_right(cube->left);
  move_face_right(cube->right);
  move_face_right(cube->top);
  move_face_right(cube->bottom);
  cube->center.x += 5;
}

void move_forward(Cube * cube) {
  move_face_forward(cube->front);
  move_face_forward(cube->back);
  move_face_forward(cube->left);
  move_face_forward(cube->right);
  move_face_forward(cube->top);
  move_face_forward(cube->bottom);
  cube->center.z += 5;
}

void move_backward(Cube * cube) {
  move_face_backward(cube->front);
  move_face_backward(cube->back);
  move_face_backward(cube->left);
  move_face_backward(cube->right);
  move_face_backward(cube->top);
  move_face_backward(cube->bottom);
  cube->center.z -= 5;
}

int main() {

  fp = fopen("/tmp/cube.log", "w+");

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window * window = SDL_CreateWindow("Cube", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, X_RESOLUTION, Y_RESOLUTION, SDL_WINDOW_OPENGL);
  SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  SDL_bool done = SDL_FALSE;

  Player player = {
    {X_MID, Y_MID, 0},
    90
  };

  Cube cube = create_cube(0, 0, 0);

  while(!done) {
    SDL_Event event;

    SDL_SetRenderDrawColor(renderer, 110, 220, 20, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 50, 50, SDL_ALPHA_OPAQUE);
    draw(renderer, cube.front, &cube.center, &player);

    SDL_SetRenderDrawColor(renderer, 50, 255, 50, SDL_ALPHA_OPAQUE);
    draw(renderer, cube.back, &cube.center, &player);

    SDL_SetRenderDrawColor(renderer, 50, 50, 255, SDL_ALPHA_OPAQUE);
    draw(renderer, cube.left, &cube.center, &player);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    draw(renderer, cube.right, &cube.center, &player);

    SDL_SetRenderDrawColor(renderer, 180, 180, 180, SDL_ALPHA_OPAQUE);
    draw(renderer, cube.top, &cube.center, &player);

    SDL_SetRenderDrawColor(renderer, 20, 30, 40, SDL_ALPHA_OPAQUE);
    draw(renderer, cube.bottom, &cube.center, &player);

    SDL_RenderPresent(renderer);
    rotate(cube.front, &cube.center);
    rotate(cube.back, &cube.center);
    rotate(cube.left, &cube.center);
    rotate(cube.right, &cube.center);
    rotate(cube.top, &cube.center);
    rotate(cube.bottom, &cube.center);

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
          move_left(&cube);
          break;
        case SDLK_RIGHT:
          fprintf(fp, "RIGHT\n");
          move_right(&cube);
          break;
        case SDLK_UP:
          fprintf(fp, "UP\n");
          move_forward(&cube);
          break;
        case SDLK_DOWN:
          fprintf(fp, "DOWN\n");
          move_backward(&cube);
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
