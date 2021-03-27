#include "controller.h"
#include <iostream>
#include "SDL.h"
#include "snake.h"
#include "game.h"

void Controller::ChangeDirection(Snake &snake, Snake::Direction input,
                                 Snake::Direction opposite) const {
  if (snake.direction != opposite || snake.GetData()->size == 1) snake.direction = input;
  return;
}

void Controller::HandleInput(bool &running, Snake &snake, Game *game) const {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      running = false;
    } else if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {
        case SDLK_UP:
          ChangeDirection(snake, Snake::Direction::kUp,
                          Snake::Direction::kDown);
          break;

        case SDLK_DOWN:
          ChangeDirection(snake, Snake::Direction::kDown,
                          Snake::Direction::kUp);
          break;

        case SDLK_LEFT:
          ChangeDirection(snake, Snake::Direction::kLeft,
                          Snake::Direction::kRight);
          break;

        case SDLK_RIGHT:
          ChangeDirection(snake, Snake::Direction::kRight,
                          Snake::Direction::kLeft);
          break;
        case SDLK_KP_1:
          if(snake.HasPotion()) snake.UsePotion();
          break;
        case SDLK_KP_2:
          if(snake.HasBomb()) snake.UseBomb();
          break;
        case SDLK_KP_3:
          if(snake.HasShrinkPill()) snake.UseShrinkPill();
          break;
        case SDLK_KP_4:
          if(snake.HasSlowPill()) snake.UseSlowPill();
          break;
        case SDLK_q:
          game->DebugPrint();
          break;
        case SDLK_PLUS:
          snake.speed += 0.01;
          break;
        case SDLK_MINUS:
          snake.speed -= 0.01;
          break;
      }
    }
  }
}