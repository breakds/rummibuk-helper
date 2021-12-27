#pragma once

#include <vector>

#include "rummibuk/solver/tile.h"

namespace rummibuk {

class PileEdgeList {
 public:
  PileEdgeList() = default;

  void Add(size_t index) {
    set_indices_.emplace_back(index);
  }

 private:
  std::vector<size_t> set_indices_{};
};

class SetEdgeList {
 public:
  SetEdgeList() = default;

  void Add(size_t index) {
    tile_indices_.emplace_back(index);
  }

 private:
  std::vector<size_t> tile_indices_{};
};

std::vector<ValidSet> InitializeValidSets(const Pile &pile);

// class SearchState {
//  public:
//   explicit SearchState(const Pile& pile);

//  private:
//   Pile pile_;
//   std::vector<PileEdgeList> pile_edges_{};

//   std::vector<ValidSet> valid_sets_{};
//   std::vector<SetEdgeList> set_edges_{};
// };

}  // namespace rummibuk
