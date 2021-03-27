#pragma once

// Ideally these colors would be read in from a file so the user
// has control over the color scheme, so they are kept
// separate from the color.h or game.h header files

#include "color.h"

// Screen colors
const Color screenBackgroundColor{0x1E, 0x1E, 0x1E, 0xFF};

// Snake colors
const Color liveSnakeHeadColor{0x00, 0x4B, 0x19, 0xFF};
const Color deadSnakeHeadColor{0x80, 0x00, 0x00, 0xFF};
const Color liveSnakeBodyColor{0x00, 0xE1, 0x19, 0xFF};
const Color deadSnakeBodyColor{0x4B, 0x00, 0x00, 0xFF};

const Color invincibleSnakeHeadColor{0x00, 0x4B, 0x60, 0xFF};
const Color invincibleSnakeBodyColor{0xC8, 0xE1, 0x19, 0xFF};

const Color slowSnakeHeadColor{0x00, 0x30, 0x19, 0xFF};
const Color slowSnakeBodyColor{0x00, 0x7B, 0x19, 0xFF};

// GameElement colors
const Color foodColor{0xFF, 0x00, 0x00, 0xFF};
const Color wallColor{0x73, 0x73, 0x73, 0xFF};
const Color potionColor{0x80, 0x00, 0x80, 0xFF};

const Color bombColor{0x00, 0x00, 0x00, 0xFF};
const Color bombLit1{0X5F, 0X00, 0X00, 0XFF};
const Color bombLit2{0X99, 0X2B, 0X38, 0XFF};
const Color bombLit3{0XBF, 0X66, 0X00, 0XFF};
const Color bombLit4{0XFF, 0X66, 0X00, 0XFF};
const Color bombLit5{0XFF, 0XC6, 0X00, 0XFF};
const Color bombLit6{0XFF, 0XFF, 0X00, 0XFF};
const Color bombLit7{0XFF, 0XFF, 0X55, 0XFF};
const Color bombLit8{0XFF, 0XFF, 0XB9, 0XFF};
const Color bombExplodesColor{0xFF, 0xFF, 0xFF, 0xFF};

const Color shrinkPillColor{0xFF, 0xFF, 0x00, 0xFF};

const Color slowPillColor{0X00, 0XFF, 0XFF, 0XFF};