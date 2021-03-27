#include <iostream>
#include "game_element.h"

int GameElement::_debugId = 0;

GameElement::GameElement() :
    _location({0,0}),
    _currentColor(screenBackgroundColor),
    _defaultColor(screenBackgroundColor),
    _actionColor(screenBackgroundColor),
    _solid(false),
    _visibility(Hidden),
    _elementType(Unknown),
    _vecActionColors(),
    _id(_debugId++),
    _actionThread(),
    _colorMtx()
{
}

GameElement::GameElement(int x, int y, Color color, ElementType type) :
    _location({x,y}),
    _currentColor(screenBackgroundColor),
    _defaultColor(color),
    _actionColor(color),
    _solid(false),
    _visibility(Hidden),
    _elementType(type),
    _vecActionColors(),
    _id(_debugId++),
    _actionThread(),
    _colorMtx()
{
}

GameElement::GameElement(SDL_Point location, Color color, ElementType type) :
    _location(location),
    _currentColor(screenBackgroundColor),
    _defaultColor(color),
    _actionColor(color),
    _solid(false),
    _visibility(Hidden),
    _elementType(type),
    _vecActionColors(),
    _id(_debugId++),
    _actionThread(),
    _colorMtx()
{
}

GameElement::GameElement(Color color, ElementType type) :
    _location({-1, -1}),
    _currentColor(color),
    _defaultColor(color),
    _actionColor(screenBackgroundColor),
    _solid(false),
    _visibility(Hidden),
    _elementType(type),
    _vecActionColors(),
    _id(_debugId++),
    _actionThread(),
    _colorMtx()
{
}

GameElement::~GameElement()
{
    if (_actionThread.joinable()) {
        _actionThread.join();
    }
}

// copy constructor
GameElement::GameElement(const GameElement& element) noexcept :
    _location(element._location),
    _currentColor(element._currentColor),
    _defaultColor(element._defaultColor),
    _actionColor(element._actionColor),
    _solid(element._solid),
    _visibility(element._visibility),
    _elementType(element._elementType),
    _vecActionColors(element._vecActionColors),
    _id(DEFAULT_DEBUG_ID),
    _actionThread(),
    _colorMtx()
{ std::cout << "GE copy constructor" << std::endl; }

// copy assignment operator
GameElement& GameElement::operator=(const GameElement& element) noexcept
{
    std::cout << "GE copy assignment" << std::endl; 
    if(this != &element) {
        // set the values from the given element
        _location = element._location;
        _currentColor = element._currentColor;
        _defaultColor = element._defaultColor;
        _actionColor = element._actionColor;
        _solid = element._solid;
        _visibility = element._visibility;
        _elementType = element._elementType;
        _vecActionColors = element._vecActionColors;
        _id = DEFAULT_DEBUG_ID;
    }
    return *this;
}

// move constructor
GameElement::GameElement(GameElement&& element) noexcept :
    _location({-1, -1}),
    _currentColor(screenBackgroundColor),
    _defaultColor(screenBackgroundColor),
    _actionColor(screenBackgroundColor),
    _solid(false),
    _visibility(Hidden),
    _elementType(Unknown),
    _vecActionColors(),
    _id(DEFAULT_DEBUG_ID),
    _actionThread(),
    _colorMtx()
{
    std::cout << "GE move constructor" << std::endl; 
    // set the values from the given element
    _location = element._location;
    _currentColor = element._currentColor;
    _defaultColor = element._defaultColor;
    _actionColor = element._actionColor;
    _solid = element._solid;
    _visibility = element._visibility;
    _elementType = element._elementType;
    _vecActionColors = element._vecActionColors;
    _id = element._id;

    // reset the values in the given element
    element._location = {-1, -1};
    element._currentColor = screenBackgroundColor;
    element._defaultColor = screenBackgroundColor;
    element._actionColor = screenBackgroundColor;
    element._solid = false;
    element._visibility = Hidden;
    element._elementType = Unknown;
    element._vecActionColors.clear();
    _id = DEFAULT_DEBUG_ID;
}

// move assignment operator
GameElement& GameElement::operator=(GameElement&& element) noexcept
{
    std::cout << "GE move assignment" << std::endl; 
    if(this != &element) {
        // set the values from the given element
        _location = element._location;
        _currentColor = element._currentColor;
        _defaultColor = element._defaultColor;
        _actionColor = element._actionColor;
        _solid = element._solid;
        _visibility = element._visibility;
        _elementType = element._elementType;
        _vecActionColors = element._vecActionColors;
        _id = element._id;

        // reset the values in the given element
        element._location = {-1, -1};
        element._currentColor = screenBackgroundColor;
        element._defaultColor = screenBackgroundColor;
        element._actionColor = screenBackgroundColor;
        element._solid = false;
        element._visibility = Hidden;
        element._elementType = Unknown;
        element._vecActionColors.clear();
        _id = DEFAULT_DEBUG_ID;
    }

}

std::string GameElement::GetElementTypeString(ElementType type)
{
    switch(type) {
        case Potion:
            return "Potion";
        case Bomb:
            return "Bomb";
        case ShrinkPill:
            return "Shrink Pill";
        case SlowPill:
            return "SlowPill";
        case Wall:
            return "Wall";
        case Food:
            return "Food";
        default:
            return "Unknown";
    }
}

std::string GameElement::GetElementTypeString()
{
    return GetElementTypeString(_elementType);
}

void GameElement::SetLocation(int x, int y)
{
    _location.x = x;
    _location.y = y;
}

void GameElement::SetColor(int r, int g, int b, int a)
{
    // lock the mutex
    std::unique_lock<std::mutex>(_colorMtx);

    // update the color
    _currentColor.red(r);
    _currentColor.green(g);
    _currentColor.blue(b);
    _currentColor.alpha(a);
}

void GameElement::SetVisibility(bool isVisible)
{ 
    if(isVisible) {
        if(_appearanceTimer > 0) {
            _visibility = Appearing;
        } else {
            _visibility = Visible;
        }
    } else {
        _visibility = Hidden;
    }
}

void GameElement::Show()
{
    _visibility = Visible;
    _appearanceTimer = DEFAULT_APPEARANCE_TIMER;
}

void GameElement::Hide()
{
    _visibility = Hidden;
    _location.x = 127;
    _location.y = 127;
}

void GameElement::SetInstantAppear()
{
    _appearanceTimer = 0;
}

void GameElement::SetAlpha(int a)
{
    // lock the mutex
    std::unique_lock<std::mutex>(_colorMtx);

    // update the color
    _currentColor.alpha(a);
}

void GameElement::SetColor(Color c)
{
    // lock the mutex
    std::unique_lock<std::mutex>(_colorMtx);

    // update the color
    _currentColor = c;
}

void GameElement::MakeSolid() { _solid = true; }

void GameElement::MakeIntangible() { _solid = false; }

Color GameElement::getColor() const
{
    // lock the mutex while we return the color
    std::unique_lock<std::mutex>(_colorMtx);
    return _currentColor;
}

void GameElement::UpdateColor()
{
    if(_visibility == Appearing) {
        if(_appearanceTimer > 0) {
            // lock the mutex
            std::unique_lock<std::mutex> colorLock(_colorMtx);

            // update the color
            if(_currentColor.red() < _defaultColor.red()) {
                _currentColor.addRed(1);
            } else if(_currentColor.red() > _defaultColor.red()) {
                _currentColor.addRed(-1);
            }

            if(_currentColor.green() < _defaultColor.green()) {
                _currentColor.addGreen(1);
            } else if(_currentColor.green() > _defaultColor.green()) {
                _currentColor.addGreen(-1);
            }
            
            if(_currentColor.blue() < _defaultColor.blue()) {
                _currentColor.addBlue(1);
            } else if(_currentColor.blue() > _defaultColor.blue()) {
                _currentColor.addBlue(-1);
            }

            --_appearanceTimer;

            // unlock the mutex before printing
            colorLock.unlock();
        } else {
            std::cout << "Setting  " << GetElementTypeString(_elementType) << " (" << _id << ") at " << _location.x << ", " << _location.y << " visible" << std::endl;
            Show();
            MakeSolid();
        }
    }
}

/* class Food */
Food::Food() : GameElement(foodColor, GameElement::Food) { SetInstantAppear(); }

Food::Food(int x, int y) : GameElement(x, y, foodColor, GameElement::Food) { SetInstantAppear(); }

Food::Food(SDL_Point location) : GameElement(location, foodColor, GameElement::Food) { SetInstantAppear(); }

Food::Food(Color color) : GameElement(color, ElementType::Food) { SetInstantAppear(); }

Food::~Food() { }



/* class Wall */
Wall::Wall() : GameElement(wallColor, GameElement::Wall) { }

Wall::Wall(int x, int y) : GameElement(x, y, wallColor, GameElement::Wall) { }

Wall::Wall(SDL_Point location) : GameElement(location, wallColor, GameElement::Wall) { }

Wall::Wall(Color color) : GameElement(color, ElementType::Wall) { }

Wall::~Wall() { }



/* class Potion */
Potion::Potion() : GameElement(potionColor, GameElement::Potion) { }

Potion::Potion(int x, int y) : GameElement(x, y, potionColor, GameElement::Potion) { }

Potion::Potion(SDL_Point location) : GameElement(location, potionColor, GameElement::Potion) { }

Potion::Potion(Color color) : GameElement(color, ElementType::Potion) { }

Potion::~Potion() { }

void Potion::UseItem()
{
    SetAvailable();
    DrinkPotion();
}

void Potion::DrinkPotion()
{
    if(_useCallbackFn) _useCallbackFn(_location);
}



/* class Bomb */
Bomb::Bomb() : GameElement(bombColor, GameElement::Bomb) { LoadActionColors(); }

Bomb::Bomb(int x, int y) : GameElement(x, y, bombColor, GameElement::Bomb) { LoadActionColors(); }

Bomb::Bomb(SDL_Point location) : GameElement(location, bombColor, GameElement::Bomb) { LoadActionColors(); }

Bomb::Bomb(Color color) : GameElement(color, ElementType::Bomb) { LoadActionColors(); }

Bomb::~Bomb() { }

void Bomb::LoadActionColors()
{
    _vecActionColors.push_back(bombLit1);
    _vecActionColors.push_back(bombLit2);
    _vecActionColors.push_back(bombLit3);
    _vecActionColors.push_back(bombLit4);
    _vecActionColors.push_back(bombLit5);
    _vecActionColors.push_back(bombLit6);
    _vecActionColors.push_back(bombLit7);
    _vecActionColors.push_back(bombLit8);
}

void Bomb::UseItem()
{
    std::cout << "Using bomb (" << _id << ")!!" << std::endl;

    // if thread is running, join it
    if(_actionThread.joinable()) {
        _actionThread.join();
    }

    // lock the mutex
    std::unique_lock<std::mutex> colorLock(_colorMtx);

    // update the color
    _currentColor = _vecActionColors[0];
    _actionColor = bombExplodesColor;
    _actionTimer = DEFAULT_ACTION_TIMER;

    _appearanceTimer = 0;

    // unlock the mutex
    colorLock.unlock();

    // set the visibility
    SetVisibility(true);

    // start thread
    _actionThread = std::thread(&Bomb::LightFuse, this);
}

void Bomb::LightFuse()
{
    std::cout << "Running action for " << GetElementTypeString() << " (" << _id << ") - " << _visibility << std::endl;

    const int one_eighths   = _actionTimer / 8;
    const int two_eighths   = 2 * one_eighths;
    const int three_eighths = 3 * one_eighths;
    const int four_eighths  = 4 * one_eighths;
    const int five_eighths  = 5 * one_eighths;
    const int six_eighths   = 6 * one_eighths;
    const int seven_eighths = 7 * one_eighths;
    
    while(_actionTimer > 0) {
        // lock the mutex
        std::unique_lock<std::mutex> colorLock(_colorMtx);

        // update the color
        if(_actionTimer == seven_eighths) {
            _currentColor = _vecActionColors[1];
        } else if(_actionTimer == six_eighths) {
            _currentColor = _vecActionColors[2];
        } else if(_actionTimer == five_eighths) {
            _currentColor = _vecActionColors[3];
        } else if(_actionTimer == four_eighths) {
            _currentColor = _vecActionColors[4];
        } else if(_actionTimer == three_eighths) {
            _currentColor = _vecActionColors[5];
        } else if(_actionTimer == two_eighths) {
            _currentColor = _vecActionColors[6];
        } else if(_actionTimer == one_eighths) {
            _currentColor = _vecActionColors[7];
        }


        --_actionTimer;

        // unlock the mutex before printing
        colorLock.unlock();
        std::cout << "Setting " << GetElementTypeString(_elementType) << " (" << _id << ") at " << _location.x << ", " << _location.y << " to color " << std::to_string(_currentColor.red()) << ", " << std::to_string(_currentColor.green()) << ", " << std::to_string(_currentColor.blue()) << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    // lock the mutex
    std::unique_lock<std::mutex> colorLock(_colorMtx);

    // update the color
    _currentColor = bombColor;

    // unlock the mutex before printing
    colorLock.unlock();

    // call the callback if it exists
    if(_useCallbackFn) _useCallbackFn(_location);
    std::cout << GetElementTypeString() << " (" << _id << ") - has exploded!!" << _visibility << std::endl;

    // bomb exploded, hide the instance and make it available to use again
    Hide();
    SetAvailable();
}





/* class ShrinkPill */
ShrinkPill::ShrinkPill() : GameElement(shrinkPillColor, GameElement::ShrinkPill) { }

ShrinkPill::ShrinkPill(int x, int y) : GameElement(x, y, shrinkPillColor, GameElement::ShrinkPill) { }

ShrinkPill::ShrinkPill(SDL_Point location) : GameElement(location, shrinkPillColor, GameElement::ShrinkPill) { }

ShrinkPill::ShrinkPill(Color color) : GameElement(color, ElementType::ShrinkPill) { }

ShrinkPill::~ShrinkPill() { }

void ShrinkPill::UseItem()
{
    SetAvailable();
    PopPill();
}

void ShrinkPill::PopPill()
{
    if(_useCallbackFn) _useCallbackFn(_location);
}


/* class SlowPill */
SlowPill::SlowPill() : GameElement(slowPillColor, GameElement::SlowPill) { }

SlowPill::SlowPill(int x, int y) : GameElement(x, y, slowPillColor, GameElement::SlowPill) { }

SlowPill::SlowPill(SDL_Point location) : GameElement(location, slowPillColor, GameElement::SlowPill) { }

SlowPill::SlowPill(Color color) : GameElement(color, ElementType::SlowPill) { }

SlowPill::~SlowPill() { }

void SlowPill::UseItem()
{
    SetAvailable();
    PopPill();
}

void SlowPill::PopPill()
{
    if(_useCallbackFn) _useCallbackFn(_location);
}

