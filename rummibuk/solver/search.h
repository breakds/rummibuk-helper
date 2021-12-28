#pragma once

#include <vector>

#include "rummibuk/solver/tile.h"

namespace rummibuk {

std::vector<ValidSet> InitializeValidSets(const Pile &pile);

std::vector<ValidSet> Solve(const Pile &pile);

std::vector<ValidSet> Solve2(const Pile &pile);

}  // namespace rummibuk
