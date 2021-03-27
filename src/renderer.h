#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <memory>
#include "SDL.h"
#include "snake.h"

class Renderer {
 public:
  Renderer(const std::size_t screen_width, const std::size_t screen_height,
           const std::size_t grid_width, const std::size_t grid_height);
  ~Renderer();

  void Render(Snake const &snake, GameElement const &food, std::vector<std::shared_ptr<GameElement>> &elements);
  void SetRenderDrawColor(SDL_Renderer *renderer, Color color);
  void UpdateWindowTitle(int score, int multiplier, int timer, int fps, struct SnakeData *pData);

 private:
  SDL_Window *sdl_window;
  SDL_Renderer *sdl_renderer;

  const std::size_t screen_width;
  const std::size_t screen_height;
  const std::size_t grid_width;
  const std::size_t grid_height;
};

#endif