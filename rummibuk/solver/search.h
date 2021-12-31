#pragma once

#include <vector>

#include "rummibuk/solver/tile.h"

namespace rummibuk {

// Create a list of all the valid sets that the input pile is able to form.
std::vector<ValidSet> InitializeValidSets(const Pile &pile);

// This is the main API that solves the Rummibuk problem defined by a pile.
std::vector<ValidSet> Solve(const Pile &pile);

}  // namespace rummibuk
