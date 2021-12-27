#pragma once

#include <string>
#include <vector>

namespace rummibuk {

enum class Color : int {
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

class Pile {
 public:
  Pile();

  static size_t IdOf(const Tile &tile);

  static const Tile &TileOf(size_t id);

  void Add(const Tile &tile);

  int wildcards() const {
    return quantities_[0];
  }

  int count(size_t id) const {
    return quantities_[id];
  }

 private:
  std::vector<int> quantities_{};
};

}  // namespace rummibuk
