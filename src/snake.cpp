#include "snake.h"
#include <cmath>
#include <iostream>
#include <algorithm>

Snake::Snake(int grid_width, int grid_height)
      : grid_width(grid_width),
        grid_height(grid_height),
        head_x(grid_width / 2),
        head_y(grid_height / 2),
        head_color(liveSnakeHeadColor), body_color(liveSnakeBodyColor),
        _items(GameElement::NUM_ELEMENT_TYPES-1, std::vector<std::shared_ptr<GameElement>>()),
        _pData(new SnakeData())
{
}

Snake::~Snake()
{
  if(_pData != nullptr) {
    delete _pData;
    _pData = nullptr;
  }
}

void Snake::Update() {
  SDL_Point prev_cell{
      static_cast<int>(head_x),
      static_cast<int>(
          head_y)};  // We first capture the head's cell before updating.
  UpdateHead();
  SDL_Point current_cell{
      static_cast<int>(head_x),
      static_cast<int>(head_y)};  // Capture the head's cell after updating.

  // Update all of the body vector items if the snake head has moved to a new
  // cell.
  if (current_cell.x != prev_cell.x || current_cell.y != prev_cell.y) {
    UpdateBody(current_cell, prev_cell);
  }

  if(_invincible) {
    if(_invincibleTimer > 0) {
      _invincibleTimer--;
      if(_invincibleTimer < (DEFAULT_INVINCIBLE_TIMER/2)) {
        // toggle the color over time
        if(_invincibleTimer % (DEFAULT_INVINCIBLE_TIMER/16) == 0) {
          head_color = (head_color == liveSnakeHeadColor) ? invincibleSnakeHeadColor : liveSnakeHeadColor;
          body_color = (body_color == liveSnakeBodyColor) ? invincibleSnakeBodyColor : liveSnakeBodyColor;
        }
      }
    } else {
      _invincibleTimer = DEFAULT_INVINCIBLE_TIMER;
      _invincible = false;
      body_color = liveSnakeBodyColor;
      head_color = liveSnakeHeadColor;
      _abilityActive = false;
    }
  }
}

void Snake::UpdateHead() {
  switch (direction) {
    case Direction::kUp:
      head_y -= speed;
      break;

    case Direction::kDown:
      head_y += speed;
      break;

    case Direction::kLeft:
      head_x -= speed;
      break;

    case Direction::kRight:
      head_x += speed;
      break;
  }

  // Wrap the Snake around to the beginning if going off of the screen.
  head_x = fmod(head_x + grid_width, grid_width);
  head_y = fmod(head_y + grid_height, grid_height);
}

void Snake::UpdateBody(SDL_Point &current_head_cell, SDL_Point &prev_head_cell) {
  // Add previous head location to vector
  body.push_back(prev_head_cell);

  if (!_growing) {
    if(_shrinking) {
      std::cout << "Shrinking body" << std::endl;
      _shrinking = false;
      _abilityActive = false;

      // Remove half of the body from the vector.
      body.erase(body.begin(), body.begin() + ((body.size()/2) - 1));
      if(_pData) {
        _pData->size = body.size() + 1;
      }
    } else {
      // Remove the tail from the vector.
      body.erase(body.begin());
    }
  } else {
    std::cout << "Growing body" << std::endl;
    _growing = false;
    if(_pData) {
      _pData->size++;
    }
  }

  // Check if the snake has died.
  for (auto const &item : body) {
    if (current_head_cell.x == item.x && current_head_cell.y == item.y) {
      KillSnake();
    }
  }
}

void Snake::KillSnake()
{
  alive = false;
  body_color = deadSnakeBodyColor;
  head_color = deadSnakeHeadColor;
}

void Snake::MakeInvincible()
{
  head_color = invincibleSnakeHeadColor;
  body_color = invincibleSnakeBodyColor;
  _invincible = true;
  // _abilityActive will be set false after invincibility wears off in Update()
}

void Snake::GrowBody() { _growing = true; }

void Snake::ShrinkBody()
{
  std::cout << "Shrinking snake" << std::endl;
  _shrinking = true;
  _abilityActive = false;
}

// Inefficient method to check if cell is occupied by snake.
bool Snake::SnakeCell(int x, int y) {
  if (x == static_cast<int>(head_x) && y == static_cast<int>(head_y)) {
    return true;
  }
  for (auto const &item : body) {
    if (x == item.x && y == item.y) {
      return true;
    }
  }
  return false;
}

void Snake::SlowSnake()
{
  std::cout << "Slowing snake" << std::endl;
  speed = DEFAULT_SPEED;
  _abilityActive = false;
}

void Snake::UseElement(GameElement::ElementType type)
{
  if(_items.at(type).size() > 0 && !_abilityActive) {
    // get the first element of type
    auto &e = _items.at(type).front();

    // don't make potions or pills visible, they just get used
    if(type == GameElement::POTION) {
      _abilityActive = true;
      std::cout << "Potion used" << std::endl;
    } else if(type == GameElement::SHRINK_PILL) {
      _abilityActive = true;
      std::cout << "Shrink Pill used" << std::endl;
    } else if(type == GameElement::SLOW_PILL) {
      _abilityActive = true;
      std::cout << "Slow Pill used" << std::endl;
    } else if(type == GameElement::BOMB) {
      e->SetLocation(head_x, head_y);
      std::cout << GameElement::GetElementTypeString(type) << " (" << e->_id << ") placed at " << (int)head_x << ", " << (int)head_y << std::endl;
    }

    // use the item's callback to trigger an action, if one is set
    e->UseItem();

    // remove the element from the vector
    _items.at(type).erase(_items.at(type).begin());
  }
}

void Snake::AddPotion(std::shared_ptr<Potion> &potion)
{
  potion->Hide();
  potion->SetUnavailable();
  potion->SetUseCallbackFn(std::bind(&Snake::MakeInvincible, this));
  _items.at(GameElement::POTION).emplace_back(potion);
  std::cout << potion->GetElementTypeString() << " (" << potion->_id << ") picked up from " << (int)head_x << ", " << (int)head_y << std::endl;
}

void Snake::AddBomb(std::shared_ptr<Bomb> &bomb)
{
  bomb->Hide();
  bomb->SetUnavailable();
  // callback was set in Game
  _items.at(GameElement::BOMB).emplace_back(bomb);
  std::cout << bomb->GetElementTypeString() << " (" << bomb->_id << ") picked up from " << (int)head_x << ", " << (int)head_y << std::endl;
}

void Snake::AddShrinkPill(std::shared_ptr<ShrinkPill> &pill)
{
  pill->Hide();
  pill->SetUnavailable();
  pill->SetUseCallbackFn(std::bind(&Snake::ShrinkBody, this));
  _items.at(GameElement::SHRINK_PILL).emplace_back(pill);
  std::cout << pill->GetElementTypeString() << " (" << pill->_id << ") picked up from " << (int)head_x << ", " << (int)head_y << std::endl;
}

void Snake::AddSlowPill(std::shared_ptr<SlowPill> &pill)
{
  pill->Hide();
  pill->SetUnavailable();
  pill->SetUseCallbackFn(std::bind(&Snake::SlowSnake, this));
  _items.at(GameElement::SLOW_PILL).emplace_back(pill);
  std::cout << pill->GetElementTypeString() << " (" << pill->_id << ") picked up from " << (int)head_x << ", " << (int)head_y << std::endl;
}

void Snake::UsePotion()
{
  UseElement(GameElement::POTION);
}

void Snake::UseBomb()
{
  UseElement(GameElement::BOMB);
}

void Snake::UseShrinkPill()
{
  UseElement(GameElement::SHRINK_PILL);
}

void Snake::UseSlowPill()
{
  UseElement(GameElement::SLOW_PILL);
}

void Snake::UpdateData()
{
  if(_pData) {
    _pData->potions = PotionCount();
    _pData->bombs = BombCount();;
    _pData->shrinkpills = ShrinkPillCount();
    _pData->slowpills = SlowPillCount();
  }
}

SnakeData* Snake::GetData()
{
  UpdateData();
  return _pData;
}