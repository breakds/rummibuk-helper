#pragma once

#include <string>

namespace rummibuk {

enum class Color {
  WILDCARD = 0,
  BLUE     = 1,
  RED      = 2,
  ORANGE   = 3,
  BLACK    = 4,
};

struct Tile {
  Color color = Color::WILDCARD;
  int number;

  static Tile FromString(const std::string &input);

  bool IsWildcard() const {
    return color == Color::WILDCARD;
  }
};

}  // namespace rummibuk
