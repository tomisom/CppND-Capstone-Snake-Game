#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include <memory>
#include <functional>
#include "SDL.h"
#include "color.h"
#include "game_element.h"
#include "color_defines.h"

#define DEFAULT_INVINCIBLE_TIMER 512
#define DEFAULT_SPEED 0.1f

struct SnakeData {
  int size{1};
  int potions{0};
  int bombs{0};
  int shrinkpills{0};
  int slowpills{0};
};

class Snake {
 public:
  enum class Direction { kUp, kDown, kLeft, kRight };

  Snake(int grid_width, int grid_height);
  ~Snake();

  void Update();

  // game action functions
  void GrowBody();
  bool SnakeCell(int x, int y);
  void KillSnake();
  
  // element action functions
  void MakeInvincible();
  void ShrinkBody();
  void SlowSnake();

  // add element functions  
  void AddPotion(std::shared_ptr<Potion> &potion);
  void AddBomb(std::shared_ptr<Bomb> &bomb);
  void AddShrinkPill(std::shared_ptr<ShrinkPill> &pill);
  void AddSlowPill(std::shared_ptr<SlowPill> &pill);

  // element tests
  bool HasPotion() { return (_items.at(GameElement::Potion).size() > 0); }
  bool HasBomb() { return (_items.at(GameElement::Bomb).size() > 0); }
  bool HasShrinkPill() { return (_items.at(GameElement::ShrinkPill).size() > 0); }
  bool HasSlowPill() { return (_items.at(GameElement::SlowPill).size() > 0); }

  // use element functions
  void UsePotion();
  void UseBomb();
  void UseShrinkPill();
  void UseSlowPill();

  // element count functions
  int  PotionCount() const { return _items.at(GameElement::Potion).size(); }
  int  BombCount() const { return _items.at(GameElement::Bomb).size(); }
  int  ShrinkPillCount() const { return _items.at(GameElement::ShrinkPill).size(); }
  int  SlowPillCount() const { return _items.at(GameElement::SlowPill).size(); }
  int  GetSize() const
  {
    if(_pData) return _pData->size;
    return 0;
  }
  
  bool IsInvincible() const { return _invincible; }

  void UpdateData();
  SnakeData* GetData();

  Direction direction = Direction::kUp;

  float speed{DEFAULT_SPEED};
  bool alive{true};
  float head_x;
  float head_y;
  std::vector<SDL_Point> body;

  Color head_color;
  Color body_color;

 private:
  void UpdateHead();
  void UpdateBody(SDL_Point &current_cell, SDL_Point &prev_cell);

  void UseElement(GameElement::ElementType type);

  bool _growing{false};
  bool _shrinking{false};
  int grid_width;
  int grid_height;
  std::vector<std::vector<std::shared_ptr<GameElement>>> _items;
  int _invincibleTimer{DEFAULT_INVINCIBLE_TIMER};
  bool _invincible{false};
  bool _abilityActive{false};

  struct SnakeData* _pData;

};

#endif