#include "game.h"
#include <iostream>
#include "SDL.h"

using namespace std::placeholders;

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height),
      food(foodColor),
      _elements(),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width-1)),
      random_h(0, static_cast<int>(grid_height-1)) {
  CreateWalls();
  PlaceFood();
  std::cout << "w_min: " << random_w.min() << " w_max: " << random_w.max() << "  h_min: " << random_h.min() << " h_max: " << random_h.max() << std::endl;
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;

  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake, this);
    Update();
    renderer.Render(snake, food, _elements);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, _multiplier, _multiplierTimer/60, frame_count, snake.GetData());
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::DebugPrint()
{
  std::cout << "DEBUG" << std::endl;
}

void Game::CreateWalls()
{
  // create walls around the perimiter of the board
  // but leave a 10% gap in the middle
  int x_start = random_w.min();
  int x_end = random_w.max();
  int x_grid_count = x_end - x_start;

  int y_start = random_h.min();
  int y_end = random_h.max();
  int y_grid_count =  - y_end - y_start;

  int x_half_gap_width = x_grid_count / 5;
  int y_half_gap_width = y_grid_count / 5;

  for(int x = x_start; x < x_end; ++x) {
    if(x < ((x_grid_count/2) - x_half_gap_width) || x > ((x_grid_count/2) + x_half_gap_width)) {
      std::shared_ptr<GameElement> g1 = GameElement::CreateGameElement(GameElement::WALL, x, y_start);
      std::shared_ptr<GameElement> g2 = GameElement::CreateGameElement(GameElement::WALL, x, y_end);
      _elements.emplace_back(g1);
      _elements.emplace_back(g2);
      std::cout << "Wall " << g1->_id << ") placed at " << x << ", " << y_start << " (" << g1->GetLocation().x << ", " << g1->GetLocation().y << ")" << std::endl;
      std::cout << "Wall " << g2->_id << ") placed at " << x << ", " << y_end << " (" << g2->GetLocation().x << ", " << g2->GetLocation().y << ")" << std::endl;
      _wallCount += 2;
    }
  }

  for(int y = y_start; y < y_end; ++y) {
    if(y < ((y_grid_count/2) - y_half_gap_width) || y > ((y_grid_count/2) + y_half_gap_width)) {
      std::shared_ptr<GameElement> g1 = GameElement::CreateGameElement(GameElement::WALL, x_start, y);
      std::shared_ptr<GameElement> g2 = GameElement::CreateGameElement(GameElement::WALL, x_end, y);
      _elements.emplace_back(g1);
      _elements.emplace_back(g2);
      std::cout << "Wall " << g1->_id << ") placed at " << x_start << ", " << y << " (" << g1->GetLocation().x << ", " << g1->GetLocation().y << ")" << std::endl;
      std::cout << "Wall " << g2->_id << ") placed at " << x_end << ", " << y << " (" << g2->GetLocation().x << ", " << g2->GetLocation().y << ")" << std::endl;
      _wallCount += 2;
    }
  }
}

// get the next non-visible wall element, if one exists
std::shared_ptr<Wall> Game::GetNextWall()
{
  // generate a random entry into the vector of walls
  // this allows for an exploded wall to rematerialize later instead of 
  // right away
  int randIndex = random_w(engine) % _wallCount;

  // set the iterator to that element
  auto it = _elements.begin() + randIndex;

  while(it != _elements.end()) {
    if((*it)->IsWall() && (*it)->IsHidden()) {
      return std::static_pointer_cast<Wall>((*it));
    }
    ++it;
  }
  return std::shared_ptr<Wall>();
}

// Get the next non-wall element that's not visible, if one exists
std::shared_ptr<GameElement> Game::GetNextElement(GameElement::ElementType type)
{
  for(auto &g : _elements) {
    if((g->GetType() == type) && g->IsHidden() && g->IsAvailable()) {
      return g;
    }
  }
  return std::shared_ptr<GameElement>();
}

SDL_Point Game::GetUnoccupiedLocation()
{
  int x = -1, y = -1;
  int counter = 20; // try to find a spot to place the element a number of times
  while(counter-- > 0) {
    // keep the non-wall elements inside the walls
    x = (random_w(engine) % (random_w.max()-random_w.min()-2)) + 1;
    y = (random_h(engine) % (random_h.max()-random_h.min()-2)) + 1;

    // if that spot is unoccupied, break out of loop
    if(((x >= 0) && (x < 128)) && ((y >= 0) && (y < 128)) && !board_bits[x].test(y) && !snake.SnakeCell(x, y))
    {
      break;
    }
  }

  return {x,y};
}

void Game::PlaceNextWall()
{
  std::shared_ptr<Wall> pCurElement = GetNextWall();

  if(pCurElement) {
    pCurElement->SetVisibility(true);

    // set the bits
    board_bits[pCurElement->GetLocation().x].set(pCurElement->GetLocation().y);

    std::cout << "Placed " << pCurElement->GetElementTypeString() << " (" << pCurElement->_id << ") at " << pCurElement->GetLocation().x << ", " << pCurElement->GetLocation().y << std::endl;
  }
}

void Game::PlaceNextElement()
{
  std::cout << "PlaceNextElement" << std::endl;

  int choice = random_w(engine);
  int chance = random_h(engine);

  // only place something half the time
  if(chance % 2 == 0) return;

  GameElement::ElementType eType = static_cast<GameElement::ElementType>(choice % GameElement::NUM_ELEMENT_TYPES);

  if(eType == GameElement::WALL) {
    PlaceNextWall();
  } else {
    std::shared_ptr<GameElement> pCurElement = GetNextElement(eType);

    std::cout << "GetNextElement returned item " << pCurElement.get() << std::endl;

    if(!pCurElement) {
      pCurElement = GameElement::CreateGameElement(eType);

      // if this is a bomb, add the callback here
      if(eType == GameElement::BOMB) {
        pCurElement->SetUseCallbackFn(std::bind(&Game::ExplodeBomb, this, _1));
      }

      if(pCurElement) {
        _elements.emplace_back(pCurElement);
      }
    }

    if(pCurElement) {
      SDL_Point pt = GetUnoccupiedLocation();
      std::cout << "New Loc: x: " << pt.x << "  y: " << pt.y << std::endl;
      if(pt.x >= 0) {
        pCurElement->SetLocation(pt.x, pt.y);
        pCurElement->SetVisibility(true);

        // set the bits - duplicates are ok for now
        board_bits[pCurElement->GetLocation().x].set(pCurElement->GetLocation().y);

        std::cout << "Placed " << pCurElement->GetElementTypeString() << " (" << pCurElement->_id << ") at " << pCurElement->GetLocation().x << ", " << pCurElement->GetLocation().y << "(" << choice << ")" << std::endl;
      }
    }
  }
}

void Game::PlaceFood()
{
  int x, y;
  while (true) {
    SDL_Point pt = GetUnoccupiedLocation();

    // Check that the location is not occupied by a snake item before placing food
    if(pt.x >= 0) {
      std::cout << "New Food Loc: x: " << pt.x << "  y: " << pt.y << std::endl;
      food.SetLocation(pt.x, pt.y);
      board_bits[pt.x].set(pt.y);
      food.SetVisibility(true);
      std::cout << "Food (" << food._id << ") added at " << food.GetLocation().x << ", " << food.GetLocation().y << std::endl;
      return;
    }
  }
}

void Game::Update() {
  if (!snake.alive) return;

  snake.Update();

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  if(_multiplierTimer-- <= 0) {
    _multiplierTimer = MULTIPLIER_TIMER;
    _multiplier = 1;
  }

  // check the bitsets to see if an object is in that position
  if(((new_x >= 0) && (new_x < 128)) && ((new_y >= 0) && (new_y < 128)) && board_bits[new_x].test(new_y)) {
    // check if the snake collided with a game element
    for(auto g : _elements) {
      if(g->GetLocation().x == new_x && g->GetLocation().y == new_y) {
        board_bits[new_x].reset(new_y);
        if(g->IsPotion()) {
          std::shared_ptr<Potion> p = std::dynamic_pointer_cast<Potion>(g);
          snake.AddPotion(p);
        } else if(g->IsBomb()) {
          std::shared_ptr<Bomb> b = std::dynamic_pointer_cast<Bomb>(g);
          snake.AddBomb(b);
        } else if(g->IsShrinkPill()) {
          std::shared_ptr<ShrinkPill> s = std::dynamic_pointer_cast<ShrinkPill>(g);
          snake.AddShrinkPill(s);
        } else if(g->IsSlowPill()) {
          std::shared_ptr<SlowPill> s = std::dynamic_pointer_cast<SlowPill>(g);
          snake.AddSlowPill(s);
        } else if(g->IsWall()) {
          if(g->IsVisible() && g->IsSolid()) {
            if(!snake.IsInvincible()) {
              snake.KillSnake();
            }
            break;
          }
        }
        _multiplierTimer = MULTIPLIER_TIMER;
        _multiplier = 1;
      }
    }    
    
    // Check if there's food over here
    if (food.GetLocation().x == new_x && food.GetLocation().y == new_y) {
      score += (1 * _multiplier);
      PlaceNextWall();
      PlaceNextElement();
      PlaceFood();
      // Grow snake and increase speed.
      snake.GrowBody();
      snake.speed += 0.02;

      _multiplierTimer = MULTIPLIER_TIMER;
      _multiplier++;
    }
  }
}

void Game::ExplodeBomb(SDL_Point location)
{
  std::cout << "Bomb at " << location.x << ", " << location.y << " go boom!" << std::endl;

  if(((location.x >= 0) && (location.x < 128)) && ((location.y >= 0) && (location.y < 128))) {

    // build a vector of the possible locations to check
    std::vector<SDL_Point> points;

    // temp vectors to help build the points
    std::vector<int> xs;
    std::vector<int> ys;
    
    // add the current point
    xs.push_back(location.x);
    ys.push_back(location.y);

    if(location.x == 0) {
      // no squares before (no wrap around)
      xs.push_back(location.x + 1);
    } else if(location.x == 31) {
      // no squares after
      xs.push_back(location.x - 1);
    } else {
      // squares on both sides
      xs.push_back(location.x - 1);
      xs.push_back(location.x + 1);
    }
    
    if(location.y == 0) {
      // no squares before (no wrap around)
      ys.push_back(location.y + 1);
    } else if(location.x == 31) {
      // no squares after
      ys.push_back(location.y - 1);
    } else {
      // squares on both sides
      ys.push_back(location.y - 1);
      ys.push_back(location.y + 1);
    }

    for(int x : xs) {
      for(int y : ys) {
        SDL_Point pt{x, y};
        points.emplace_back(pt);
      }
    }
    
    for(auto &g : _elements) {
      SDL_Point p = g->GetLocation();
      for(SDL_Point &t : points) {
        if(p.x == t.x && p.y == t.y) {
          // oops, this object is in the blast radius
          board_bits[p.x].reset(p.y);
          g->SetColor(screenBackgroundColor);
          g->SetVisibility(false);  // can't use Hide() here since we want to maintain wall positions for re-use
          g->SetAvailable();
        }
      }
    }

  }
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.GetSize(); }
int Game::GetPotionCount() const { return snake.PotionCount(); }
int Game::GetBombCount() const { return snake.BombCount(); }