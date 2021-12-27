#include "rummibuk/solver/tile.h"

#include <charconv>
#include <cstdlib>

#include "spdlog/spdlog.h"

namespace rummibuk {
namespace {

void ReportInvalidTileAndDie(const std::string &input) {
  spdlog::critical("'{}' is not a valid tile.", input);
  std::abort();
}

}  // namespace

Tile Tile::FromString(const std::string &input) {
  Tile result;

  if (input.size() == 0 || input.size() > 3) {
    ReportInvalidTileAndDie(input);
  }

  switch (input[0]) {
    case 'W':
      result.color = Color::WILDCARD;
      return result;
    case 'B':
      result.color = Color::BLUE;
      break;
    case 'R':
      result.color = Color::RED;
      break;
    case 'O':
      result.color = Color::ORANGE;
      break;
    case 'K':
      result.color = Color::BLACK;
      break;
    default:
      ReportInvalidTileAndDie(input);
  }

  int number     = 0;
  auto [ptr, ec] = std::from_chars(input.data() + 1, input.data() + input.size(), number);
  if (ec != std::errc()) {
    ReportInvalidTileAndDie(input);
  }
  if (number < 1 || number > 13) {
    ReportInvalidTileAndDie(input);
  }

  result.number = number;
  return result;
}

Pile::Pile() {
  for (size_t i = 0; i < 53; ++i) {
    quantities_.emplace_back(0);
  }
}

size_t Pile::IdOf(const Tile &tile) {
  if (tile.color == Color::WILDCARD) {
    return 0;
  }
  return (static_cast<int>(tile.color) - 1) * 13 + tile.number;
}

const Tile &Pile::TileOf(size_t id) {
  static const std::vector<Tile> ALL_TILES = []() {
    std::vector<Tile> result;
    result.reserve(53);
    result.emplace_back(Tile{});
    for (int j = 1; j <= 4; ++j) {
      Color color = static_cast<Color>(j);
      for (int i = 1; i <= 13; ++i) {
        Tile tile = {
            .color  = color,
            .number = i,
        };
        result.emplace_back(tile);
      }
    }
    return result;
  }();
  return ALL_TILES[id];
}

void Pile::Add(const Tile &tile) {
  ++quantities_[IdOf(tile)];
}

}  // namespace rummibuk
