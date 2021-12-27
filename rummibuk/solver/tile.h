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
  int number  = 0;

  static Tile FromString(const std::string &input);

  bool IsWildcard() const {
    return color == Color::WILDCARD;
  }

  std::string ToString() const;
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

  int count(const Tile &tile) {
    return quantities_[IdOf(tile)];
  }

 private:
  std::vector<int> quantities_{};
};

class ValidSet {
 public:
  enum class Type : int {
    GROUP = 0,  // e.g. B12 + R12 + K12
    RUN   = 1,  // e.g. B7 + B8 + W + B10
  };

  ValidSet(const ValidSet &) = default;
  ValidSet &operator=(const ValidSet &) = default;
  ValidSet(ValidSet &&) noexcept        = default;
  ValidSet &operator=(ValidSet &&) noexcept = default;

  static ValidSet MakeGroup(const std::vector<size_t> &tile_ids, int wildcards);
  static ValidSet MakeRun(const std::vector<size_t> &tile_ids, int wildcards);

  std::string ToString() const;

 private:
  ValidSet(Type type, const std::vector<size_t> &tile_ids, int wildcards)
      : type_(type), tile_ids_(std::move(tile_ids)), wildcards_(wildcards) {
    std::sort(tile_ids_.begin(), tile_ids_.end());
  }

  Type type_;
  std::vector<size_t> tile_ids_;
  int wildcards_ = 0;
};

}  // namespace rummibuk
