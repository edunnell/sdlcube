#include <SDL2/SDL.h>

struct Vertex {
  int x;
  int y;
  int z;
};

void draw(SDL_Renderer * renderer, struct Vertex face[4], FILE * fp) {
  int i;
  for(i = 0; i < 4; ++i) {
    struct Vertex vertex = face[i];
    int vertex2i = (i == 3 ? i-3 : i+1);
    struct Vertex vertex2 = face[vertex2i];
    fprintf(fp, "x1=%d - y1=%d - x2=%d - y2=%d\n", vertex.x, vertex.y, vertex2.x, vertex2.y);
    SDL_RenderDrawLine(renderer, vertex.x, vertex.y, vertex2.x, vertex2.y);
  }
}

int main() {

  FILE * fp;
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
    
  while(!done) {
    SDL_Event event;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    
    SDL_SetRenderDrawColor(renderer, 150, 255, 50, SDL_ALPHA_OPAQUE);

    draw(renderer, face, fp);
    SDL_RenderPresent(renderer);
    
    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_QUIT) {
        done = SDL_TRUE;
      }
    }
    
  }

  if(renderer)
    SDL_DestroyRenderer(renderer);
  if(window)
    SDL_DestroyWindow(window);

  fclose(fp);
  
  SDL_Quit();  
  return 0;
}
