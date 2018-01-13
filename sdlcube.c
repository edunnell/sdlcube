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

    float distance1, distance2;
    if(vertex.z > player->position.z) {
      distance1 = vertex.z - player->position.z;
    } else {
      distance1 = player->position.z - vertex.z;
    }
    if(vertex2.z > player->position.z) {
      distance2 = vertex2.z - player->position.z;
    } else {
      distance2 = player->position.z - vertex2.z;
    }

    float x1 = vertex.x/(X_MID/distance1), x2 = vertex2.x/(X_MID/distance2);
    float y1 = vertex.y/(Y_MID/distance1), y2 = vertex2.y/(Y_MID/distance2);
    float z1 = vertex.z, z2 = vertex2.z;

    convert_graph_to_sdl(&x1, &y1);
    convert_graph_to_sdl(&x2, &y2);
    SDL_RenderDrawLine(renderer, x1 - player->position.x, y1, x2 - player->position.x, y2);
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

void rotate_face(Vertex face[4], Vertex * center) {
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

void rotate_cube(Cube * cube) {
  rotate_face(cube->front, &cube->center);
  rotate_face(cube->back, &cube->center);
  rotate_face(cube->left, &cube->center);
  rotate_face(cube->right, &cube->center);
  rotate_face(cube->top, &cube->center);
  rotate_face(cube->bottom, &cube->center);
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
    face[i].z += 50;
  }
}

void move_face_backward(Vertex face[4]) {
  int i;
  for(i = 0; i < 4; ++i) {
    face[i].z -= 50;
  }
}

void move_left(Player * player) {
  player->position.x -= 50;
}

void move_right(Player * player) {
  player->position.x += 50;
}

void move_forward(Player * player) {
  player->position.z -= 50;
}

void move_backward(Player * player) {
  player->position.z += 50;
}

int main() {

  fp = fopen("/tmp/cube.log", "w+");

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window * window = SDL_CreateWindow("Cube", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, X_RESOLUTION, Y_RESOLUTION, SDL_WINDOW_OPENGL);
  SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  SDL_bool done = SDL_FALSE;

  Player player = {
    {0, 0, 0},
    90
  };

  Cube cube = create_cube(0, 0, 100);

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
    fprintf(fp, "\n\n");

    SDL_RenderPresent(renderer);
    rotate_cube(&cube);

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
        case SDLK_a:
        case SDLK_LEFT:
          fprintf(fp, "LEFT\n");
          move_left(&player);
          break;
        case SDLK_d:
        case SDLK_RIGHT:
          fprintf(fp, "RIGHT\n");
          move_right(&player);
          break;
        case SDLK_w:
        case SDLK_UP:
          fprintf(fp, "UP\n");
          move_forward(&player);
          break;
        case SDLK_s:
        case SDLK_DOWN:
          fprintf(fp, "DOWN\n");
          move_backward(&player);
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
