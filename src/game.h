#ifndef GAME_H
#define GAME_H

#include <random>
#include <vector>
#include <bitset>
#include <memory>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include "game_element.h"

#define MULTIPLIER_TIMER 600

class Controller;

class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;
  int GetPotionCount() const;
  int GetBombCount() const;

  int GetMultiplier() const { return _multiplier; }

  void DebugPrint();

  void ExplodeBomb(SDL_Point location);

 private:
  Snake snake;
  Food food;
  std::vector<std::shared_ptr<GameElement>> _elements;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  // the bitsets will track rows and columns
  // using 128 will allow for game boards up to 128x128
  // if we want larger we can expand the bitset
  // but it must be defined at compile time
  std::bitset<128> board_bits[128];

  int _wallCount{0};

  int score{0};
  int _multiplier{1};
  int _multiplierTimer{MULTIPLIER_TIMER};

  void PlaceFood();
  void CreateWalls();
  void PlaceNextWall();
  void PlaceNextElement();
  std::shared_ptr<Wall> GetNextWall();
  std::shared_ptr<GameElement> GetNextElement(GameElement::ElementType type);
  SDL_Point GetUnoccupiedLocation();
  void Update();
};

#endif