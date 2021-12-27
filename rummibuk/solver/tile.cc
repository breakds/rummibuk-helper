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

}  // namespace rummibuk
