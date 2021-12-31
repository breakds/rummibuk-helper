#include <fstream>
#include <iostream>

#include "rummibuk/solver/search.h"
#include "rummibuk/solver/tile.h"
#include "spdlog/spdlog.h"

int main(int argc, char **argv) {
  rummibuk::Pile pile;

  {
    std::ifstream input(argv[1]);

    if (!input.is_open()) {
      spdlog::critical("Cannot read file {}", argv[1]);
      std::abort();
    }

    std::string tile;
    while (std::getline(input, tile)) {
      pile.Add(tile);
    }
    input.close();
  }

  spdlog::info("Successfully read tiles from {}", argv[1]);
  spdlog::info("Solving ...");
  auto solution = rummibuk::Solve(pile);

  if (solution.empty()) {
    spdlog::warn("[FAIL] No Solution.");
  } else {
    spdlog::info("Found solution.");
    for (const rummibuk::ValidSet &v : solution) {
      spdlog::info("  {}", v.ToString());
    }
  }

  return 0;
}
