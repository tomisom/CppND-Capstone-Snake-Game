#include "color.h"

Color::Color() :
  _red(0), _green(0), _blue(0), _alpha(0)
{
}

Color::Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a) :
  _red(r), _green(g), _blue(b), _alpha(a)
{
}

Color::~Color() { }

void Color::addRed(Uint8 val)
{
  _red += val;
}

void Color::addGreen(Uint8 val)
{
  _green += val;
}

void Color::addBlue(Uint8 val)
{
  _blue += val;
}

bool Color::operator==(const Color &c) const
{
  bool equal = (_red == c.red());
  equal = equal && (_green == c.green());
  equal = equal && (_blue == c.blue());
  equal = equal && (_alpha == c.alpha());
  return equal;
}