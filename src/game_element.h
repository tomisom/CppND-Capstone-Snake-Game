#pragma once

/*
    file: game_element.h - contains class GameElement and its subclasses. A GameElement represents an object on the gameboard, e.g. wall, food, etc.
*/

#include <functional>
#include <mutex>
#include <thread>
#include <future>
#include <vector>
#include "SDL.h"
#include "color.h"
#include "color_defines.h"

#define DEFAULT_APPEARANCE_TIMER 512
#define DEFAULT_ACTION_TIMER 512
#define DEFAULT_DEBUG_ID -99

class GameElement {
public:

    enum Visibility {
        Hidden,
        Appearing,
        Visible
    };

    typedef enum ELEMENT_TYPE {
        Potion,
        Bomb,
        ShrinkPill,
        SlowPill,
        Wall,
        NumElementTypes,
        Unknown,
        Food // this is a special type
    } ElementType;

    static int _debugId;    // will be incremented with each object and used to populate _id, unless copied, then will use default val
    int _id;
    
    // constructor and destructor
    GameElement();
    GameElement(int x, int y, Color color, ElementType type);
    GameElement(SDL_Point location, Color color, ElementType type);
    GameElement(Color color, ElementType type);
    virtual ~GameElement();

    // copy constructor and assignment operator
    GameElement(const GameElement& element) noexcept;
    GameElement& operator=(const GameElement& element) noexcept;

    // move constructor and assignment operator
    GameElement(GameElement&& element) noexcept;
    GameElement& operator=(GameElement&& element) noexcept;

    std::string GetElementTypeString();
    static std::string GetElementTypeString(ElementType type);
    
    SDL_Point GetLocation() const { return _location; }
    Color getColor() const;
    void UpdateColor();

    bool IsSolid() { return _solid; }
    bool IsHidden() { return (_visibility == Hidden); }
    bool IsAppearing() { return (_visibility == Appearing); }
    bool IsVisible() { return (_visibility != Hidden); }
    bool IsAvailable() { return _available; }
    
    ElementType GetType() { return _elementType; }
    bool IsWall() { return (_elementType == Wall); }
    bool IsPotion() { return (_elementType == Potion); }
    bool IsBomb() { return (_elementType == Bomb); }
    bool IsShrinkPill() { return (_elementType == ShrinkPill); }
    bool IsSlowPill() { return (_elementType == SlowPill); }

    void SetColor(Color c);
    void SetColor(int r, int g, int b, int a);
    void SetAlpha(int a);

    void SetAvailable() { _available = true; }
    void SetUnavailable() { _available = false; }
    
    void SetLocation(int x, int y);
    
    void SetVisibility(bool isVisible);
    void Show();
    void Hide();
    void MakeSolid();
    void MakeIntangible();
    void SetInstantAppear();
    
    // set the callback function for when item is used
    void SetUseCallbackFn(std::function<void(SDL_Point)> fn) { _useCallbackFn = fn; }

    // each subclass must implement this function
    virtual void UseItem() = 0;

 protected:
    SDL_Point _location;
    Color _currentColor;
    Color _defaultColor;
    Color _actionColor;
    bool _solid;
    Visibility _visibility;
    int _appearanceTimer{DEFAULT_APPEARANCE_TIMER};
    int _actionTimer{0};
    ElementType _elementType;
    std::function<void(SDL_Point)> _useCallbackFn;
    std::vector<Color> _vecActionColors;
    bool _available{true};

    std::mutex _colorMtx;
    std::thread _actionThread;

    // most subclasses don't use this, but bomb will override it
    virtual void LoadActionColors() { }
};


class Food : public GameElement
{
 public:
    Food();
    Food(int x, int y);
    Food(SDL_Point location);
    Food(Color color);
    ~Food();

    virtual void UseItem() override { }; // no action for a food
};

class Wall : public GameElement
{
 public:
    Wall();
    Wall(int x, int y);
    Wall(SDL_Point location);
    Wall(Color color);
    ~Wall();

    virtual void UseItem() override { }; // no action for a wall
};

class Potion : public GameElement
{
 public:
    Potion();
    Potion(int x, int y);
    Potion(SDL_Point location);
    Potion(Color color);
    ~Potion();

    void DrinkPotion();
    virtual void UseItem() override;
};

class Bomb : public GameElement
{
 public:
    Bomb();
    Bomb(int x, int y);
    Bomb(SDL_Point location);
    Bomb(Color color);
    ~Bomb();

    void LightFuse();

    virtual void UseItem() override;
    virtual void LoadActionColors() override;
};

class ShrinkPill : public GameElement
{
 public:
    ShrinkPill();
    ShrinkPill(int x, int y);
    ShrinkPill(SDL_Point location);
    ShrinkPill(Color color);
    ~ShrinkPill();

    void PopPill();
    virtual void UseItem() override;
};

class SlowPill : public GameElement
{
 public:
    SlowPill();
    SlowPill(int x, int y);
    SlowPill(SDL_Point location);
    SlowPill(Color color);
    ~SlowPill();

    void PopPill();
    virtual void UseItem() override;
};
