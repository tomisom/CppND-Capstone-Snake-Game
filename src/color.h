#pragma once
#include "SDL.h"

class Color {
 public:
   // constructor and destructor
   Color();
   Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
   ~Color();

   // getters and setters
   Uint8 red() const { return _red; }
   Uint8 green() const { return _green; }
   Uint8 blue() const { return _blue; }
   Uint8 alpha() const { return _alpha; }

   void red(Uint8 r) { _red = r; }
   void green(Uint8 g) { _green = g; }
   void blue(Uint8 b) { _blue = b; }
   void alpha(Uint8 a) { _alpha = a; }

   void addRed(Uint8 val);
   void addGreen(Uint8 val);
   void addBlue(Uint8 val);

   bool operator==(const Color &c) const;

 private:
   Uint8 _red;
   Uint8 _green;
   Uint8 _blue;
   Uint8 _alpha;
};