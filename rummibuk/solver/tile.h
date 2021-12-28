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

  Pile(const Pile &) = default;
  Pile &operator=(const Pile &) = default;
  Pile(Pile &&) noexcept        = default;
  Pile &operator=(Pile &&) noexcept = default;

  static size_t IdOf(const Tile &tile);

  static const Tile &TileOf(size_t id);

  void Add(const Tile &tile);
  void Add(size_t id);
  void Add(const std::string &input);

  void AddWildcard(int num = 1);

  void RemoveWildcard(int num = 1);
  void Remove(size_t id);

  int wildcards() const {
    return quantities_[0];
  }

  int count(size_t id) const {
    return quantities_[id];
  }

  int count(const Tile &tile) {
    return quantities_[IdOf(tile)];
  }

  size_t Hash() const;

  bool operator==(const Pile &other) const;

 private:
  std::vector<int> quantities_{};
};

class PileHash {
 public:
  size_t operator()(const Pile &pile) const {
    return pile.Hash();
  };
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
  static ValidSet FromString(const std::string &input);

  std::string ToString() const;

  bool operator==(const ValidSet &other) const {
    return wildcards_ == other.wildcards_ && tile_ids_ == other.tile_ids_;
  }

  int wildcards() const {
    return wildcards_;
  }

  const std::vector<size_t> &ids() const {
    return tile_ids_;
  }

  int total_tiles() const {
    return wildcards_ + tile_ids_.size();
  }

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
