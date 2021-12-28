#include "rummibuk/solver/tile.h"

#include <charconv>
#include <cstdlib>

#include "absl/strings/str_split.h"
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
    case '[':
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

std::string Tile::ToString() const {
  std::string result;

  switch (color) {
    case Color::WILDCARD:
      return "[]";
    case Color::BLUE:
      result += "B";
      break;
    case Color::RED:
      result += "R";
      break;
    case Color::ORANGE:
      result += "O";
      break;
    case Color::BLACK:
      result += "K";
      break;
    default:
      break;
  }

  result += std::to_string(number);
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

void Pile::Add(size_t id) {
  ++quantities_[id];
}

void Pile::Add(const std::string &input) {
  ++quantities_[IdOf(Tile::FromString(input))];
}

void Pile::AddWildcard(int num) {
  quantities_[0] += num;
}

void Pile::RemoveWildcard(int num) {
  if (quantities_[0] < num) {
    spdlog::critical("Removing {} wildcards but pile has only {}", num, quantities_[0]);
    std::abort();
  }
  quantities_[0] -= num;
}

void Pile::Remove(size_t id) {
  if (quantities_[id] <= 0) {
    spdlog::critical("Removing {}  but pile does not have any",
                     Pile::TileOf(id).ToString());
    std::abort();
  }
  --quantities_[id];
}

size_t Pile::Hash() const {
  static constexpr size_t PRIME = 351843722677;

  size_t hash = 0;
  for (size_t i = 0; i < 53; ++i) {
    hash = (hash * 3 + i) % PRIME;
  }
  return hash;
}

bool Pile::operator==(const Pile &other) const {
  for (size_t i = 0; i < 53; ++i) {
    if (quantities_[i] != other.quantities_[i]) {
      return false;
    }
  }
  return true;
}

ValidSet ValidSet::MakeGroup(const std::vector<size_t> &tile_ids, int wildcards) {
  return ValidSet(ValidSet::Type::GROUP, tile_ids, wildcards);
}

ValidSet ValidSet::MakeRun(const std::vector<size_t> &tile_ids, int wildcards) {
  return ValidSet(ValidSet::Type::RUN, tile_ids, wildcards);
}

ValidSet ValidSet::FromString(const std::string &input) {
  std::string body               = input.substr(4, input.size() - 5);
  std::vector<std::string> parts = absl::StrSplit(body, ' ');
  std::vector<size_t> tile_ids{};
  int wildcards = 0;
  for (const std::string &part : parts) {
    Tile tile = Tile::FromString(part);
    if (tile.IsWildcard()) {
      ++wildcards;
    } else {
      tile_ids.emplace_back(Pile::IdOf(tile));
    }
  }

  if (input[0] == 'G') {
    return ValidSet::MakeGroup(tile_ids, wildcards);
  } else {
    return ValidSet::MakeRun(tile_ids, wildcards);
  }
}

std::string ValidSet::ToString() const {
  std::string result;
  if (type_ == ValidSet::Type::GROUP) {
    result += "GRP(";
  } else if (type_ == ValidSet::Type::RUN) {
    result += "RUN(";
  }

  result += Pile::TileOf(tile_ids_.front()).ToString();
  for (size_t i = 1; i < tile_ids_.size(); ++i) {
    result += " ";
    result += Pile::TileOf(tile_ids_[i]).ToString();
  }
  for (int i = 0; i < wildcards_; ++i) {
    result += " ";
    result += "[]";
  }
  result += ")";

  return result;
}

}  // namespace rummibuk
