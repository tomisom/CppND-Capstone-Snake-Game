#include "renderer.h"
#include <iostream>
#include <string>

Renderer::Renderer(const std::size_t screen_width,
                   const std::size_t screen_height,
                   const std::size_t grid_width, const std::size_t grid_height)
    : screen_width(screen_width),
      screen_height(screen_height),
      grid_width(grid_width),
      grid_height(grid_height) {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create Window
  sdl_window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, screen_width,
                                screen_height, SDL_WINDOW_SHOWN);

  if (nullptr == sdl_window) {
    std::cerr << "Window could not be created.\n";
    std::cerr << " SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create renderer
  sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);
  if (nullptr == sdl_renderer) {
    std::cerr << "Renderer could not be created.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }
}

Renderer::~Renderer() {
  SDL_DestroyWindow(sdl_window);
  SDL_Quit();
}

void Renderer::SetRenderDrawColor(SDL_Renderer *renderer, Color color)
{
  SDL_SetRenderDrawColor(renderer, color.red(), color.green(), color.blue(), color.alpha());
}

void Renderer::Render(Snake const &snake, GameElement const &food, std::vector<std::shared_ptr<GameElement>> &elements) {
  SDL_Rect block;
  block.w = screen_width / grid_width;
  block.h = screen_height / grid_height;

  // Clear screen
  SetRenderDrawColor(sdl_renderer, screenBackgroundColor);
  SDL_RenderClear(sdl_renderer);

  // Render the game elements
  for(auto &g : elements) {
    if(g->IsVisible()) {
      g->UpdateColor();
      SetRenderDrawColor(sdl_renderer, g->getColor());
      block.x = g->GetLocation().x * block.w;
      block.y = g->GetLocation().y * block.h;
      SDL_RenderFillRect(sdl_renderer, &block);
    }
  }

  // Render food
  SetRenderDrawColor(sdl_renderer, food.getColor());
  block.x = food.GetLocation().x * block.w;
  block.y = food.GetLocation().y * block.h;
  SDL_RenderFillRect(sdl_renderer, &block);

  // Render snake's body
  SetRenderDrawColor(sdl_renderer, snake.body_color);
  for (SDL_Point const &point : snake.body) {
    block.x = point.x * block.w;
    block.y = point.y * block.h;
    SDL_RenderFillRect(sdl_renderer, &block);
  }

  // Render snake's head
  block.x = static_cast<int>(snake.head_x) * block.w;
  block.y = static_cast<int>(snake.head_y) * block.h;
  SetRenderDrawColor(sdl_renderer, snake.head_color);
  SDL_RenderFillRect(sdl_renderer, &block);

  // Update Screen
  SDL_RenderPresent(sdl_renderer);
}

//void Renderer::UpdateWindowTitle(int score, int multiplier, int timer, int fps, int potions, int bombs, int shrinkpills) {
void Renderer::UpdateWindowTitle(int score, int multiplier, int timer, int fps, struct SnakeData *pData)
{
  std::string title{"Snake Score: " + std::to_string(score) + " (x" + std::to_string(multiplier) + ") [" + std::to_string(timer) + "] FPS: " + std::to_string(fps) + " Potions: " + std::to_string(pData->potions) + " Bombs: " + std::to_string(pData->bombs) + " Shrink: " + std::to_string(pData->shrinkpills) + " Slow: " + std::to_string(pData->slowpills)};
  SDL_SetWindowTitle(sdl_window, title.c_str());
}
