#include "rummibuk/solver/search.h"

#include <unordered_set>

#include "spdlog/spdlog.h"

namespace rummibuk {
namespace {

void SpellRuns(const Pile& pile,
               Color color,
               int number,
               int used_wildcards,
               std::vector<size_t>* stack,
               std::vector<ValidSet>* result) {
  if (stack->size() + used_wildcards >= 3) {
    ValidSet set = ValidSet::MakeRun(*stack, used_wildcards);
    if (std::find(result->begin(), result->end(), set) == result->end()) {
      result->emplace_back(set);
    }
  }

  if (number > 13) {
    return;
  }

  // Try inserting Wildcard
  if (used_wildcards < pile.wildcards()) {
    SpellRuns(pile, color, number + 1, used_wildcards + 1, stack, result);
  }

  // Try continue spelling
  Tile tile = {
      .color  = color,
      .number = number,
  };

  size_t tile_id = Pile::IdOf(tile);

  if (pile.count(tile_id) > 0) {
    stack->emplace_back(tile_id);
    SpellRuns(pile, color, number + 1, used_wildcards, stack, result);
    stack->pop_back();
  }
}

}  // namespace

std::vector<ValidSet> InitializeValidSets(const Pile& pile) {
  std::vector<ValidSet> valid_sets{};

  // +--------+
  // | Groups |
  // +--------+
  for (int number = 1; number <= 13; ++number) {
    std::vector<size_t> existing_ids{};

    for (int c = 1; c <= 4; ++c) {
      Tile tile = {
          .color  = static_cast<Color>(c),
          .number = number,
      };
      size_t tile_id = Pile::IdOf(tile);
      if (pile.count(tile_id) > 0) {
        existing_ids.emplace_back(tile_id);
      }
    }

    // 4-group
    if (existing_ids.size() == 4) {
      valid_sets.emplace_back(ValidSet::MakeGroup(
          {existing_ids[0], existing_ids[1], existing_ids[2], existing_ids[3]}, 0));
    }

    // 3-groups
    if (existing_ids.size() >= 3) {
      for (size_t i = 0; i + 2 < existing_ids.size(); ++i) {
        for (size_t j = i + 1; j + 1 < existing_ids.size(); ++j) {
          for (size_t k = j + 1; k < existing_ids.size(); ++k) {
            for (int wildcards = 0; wildcards <= std::min(pile.wildcards(), 1);
                 ++wildcards) {
              valid_sets.emplace_back(ValidSet::MakeGroup(
                  {existing_ids[i], existing_ids[j], existing_ids[k]}, wildcards));
            }
          }
        }
      }
    }

    // 2-group + wildcard
    if (existing_ids.size() >= 2) {
      for (size_t i = 0; i + 1 < existing_ids.size(); ++i) {
        for (size_t j = i + 1; j < existing_ids.size(); ++j) {
          for (int wildcards = 1; wildcards <= pile.wildcards(); ++wildcards) {
            valid_sets.emplace_back(
                ValidSet::MakeGroup({existing_ids[i], existing_ids[j]}, wildcards));
          }
        }
      }
    }

    // 1-group + wildcard * 2
    if (existing_ids.size() >= 1) {
      for (size_t i = 0; i < existing_ids.size(); ++i) {
        for (int wildcards = 2; wildcards <= pile.wildcards(); ++wildcards) {
          valid_sets.emplace_back(ValidSet::MakeGroup({existing_ids[i]}, wildcards));
        }
      }
    }
  }

  // +--------+
  // | Runs   |
  // +--------+

  for (int c = 1; c <= 4; ++c) {
    std::vector<size_t> stack{};
    Color color = static_cast<Color>(c);
    for (int start_number = 1; start_number <= 11; ++start_number) {
      SpellRuns(pile, color, start_number, 0, &stack, &valid_sets);
    }
  }

  return valid_sets;
}

namespace {

struct SearchState {
  SearchState(const SearchState&) = default;
  SearchState& operator=(const SearchState&) = default;
  SearchState(SearchState&&) noexcept        = default;
  SearchState& operator=(SearchState&&) noexcept = default;

  Pile pile;
  std::vector<std::unordered_set<size_t>> tile_edges{53};
  std::unordered_set<size_t> wildcard_edges{};

  std::vector<ValidSet> valid_sets{};
  std::vector<bool> remove_marks{};

  explicit SearchState(const Pile& pile_)
      : pile(pile_), valid_sets(InitializeValidSets(pile)) {
    for (size_t j = 0; j < valid_sets.size(); ++j) {
      if (valid_sets[j].wildcards() > 0) {
        wildcard_edges.insert(j);
      }

      remove_marks.emplace_back(false);

      for (size_t id : valid_sets[j].ids()) {
        tile_edges[id].insert(j);
      }
    }
  }

  // Form an instance of the j-th valid set and update the states. If a
  // contradiction is reached, return false. Othewise return true.
  bool Forge(size_t j, std::unordered_set<size_t>* removed = nullptr) {
    std::unordered_set<size_t> sets_to_remove;

    if (valid_sets[j].wildcards() > 0) {
      pile.RemoveWildcard(valid_sets[j].wildcards());
      for (size_t k : wildcard_edges) {
        if (valid_sets[k].wildcards() > pile.wildcards()) {
          sets_to_remove.insert(k);
        }
      }
    }

    for (size_t id : valid_sets[j].ids()) {
      pile.Remove(id);
      if (pile.count(id) == 0) {
        for (size_t k : tile_edges[id]) {
          sets_to_remove.insert(k);
        }
      }
    }

    bool contradiction = false;

    // Now remove the sets
    for (size_t k : sets_to_remove) {
      if (valid_sets[k].wildcards() > 0) {
        wildcard_edges.erase(k);
      }
      for (size_t id : valid_sets[k].ids()) {
        tile_edges[id].erase(k);
        if (pile.count(id) > 0 && tile_edges[id].empty()) {
          contradiction = true;
        }
      }
      remove_marks[k] = true;
    }

    if (removed != nullptr) {
      *removed = std::move(sets_to_remove);
    }

    return !contradiction;
  }

  void Restore(size_t j, const std::unordered_set<size_t>& sets_to_restore) {
    if (valid_sets[j].wildcards() > 0) {
      pile.AddWildcard(valid_sets[j].wildcards());
    }

    for (size_t id : valid_sets[j].ids()) {
      pile.Add(id);
    }

    for (size_t k : sets_to_restore) {
      remove_marks[k] = false;
      for (size_t id : valid_sets[k].ids()) {
        tile_edges[id].insert(k);
      }
      if (valid_sets[k].wildcards() > 0) {
        wildcard_edges.insert(k);
      }
    }
  }
};

static constexpr size_t DOOMED_CACHE_SIZE = 1000000;

std::vector<size_t> SolveImpl(const SearchState& input_state,
                              const std::vector<size_t>& completed,
                              std::unordered_set<Pile, PileHash>* doomed) {
  if (doomed->count(input_state.pile) > 0) {
    return {};
  }

  // Check for explicit failure.
  int remaining = input_state.pile.wildcards();
  for (size_t id = 1; id <= 52; ++id) {
    if (input_state.pile.count(id) > 0) {
      if (input_state.tile_edges[id].empty()) {
        return {};
      }
      remaining += input_state.pile.count(id);
    }
  }

  // Check for win
  if (remaining == 0) {
    return completed;
  }

  SearchState state        = input_state;
  std::vector<size_t> accu = completed;

  // Check for must-picks.
  bool altered = true;
  while (altered) {
    altered = false;
    for (size_t id = 1; id <= 52; ++id) {
      if (state.pile.count(id) > 0 && state.tile_edges[id].size() == 1) {
        size_t j = *state.tile_edges[id].begin();
        if (!state.Forge(j)) {
          return {};
        }
        accu.emplace_back(j);
        remaining -= state.valid_sets[j].total_tiles();
        altered = true;
      }
    }
  }

  // Check for win
  if (remaining == 0) {
    return accu;
  }

  if (doomed->count(state.pile) > 0) {
    return {};
  }

  // Elect the tile to work on, which should be the one that is left and with
  // the least number of (valid set) choices.
  size_t elected_id = 0;
  int num_choices   = -1;
  for (size_t id = 1; id <= 52; ++id) {
    if (state.pile.count(id) == 0) {
      continue;
    }
    if (num_choices == -1 || state.tile_edges[id].size() < num_choices) {
      elected_id  = id;
      num_choices = state.tile_edges[id].size();
    }
  }

  if (num_choices == -1) {
    spdlog::error("No tile is elected and yet it is not winning.");
    std::abort();
  }

  std::unordered_set<size_t> removed;
  // Save a copy of the tile_edges as it will be modified within the loop by
  // call to Forge().
  std::unordered_set<size_t> valid_set_ids_to_try = state.tile_edges[elected_id];
  for (size_t j : valid_set_ids_to_try) {
    bool can_forge = state.Forge(j, &removed);

    if (can_forge) {
      accu.emplace_back(j);
      std::vector<size_t> result = SolveImpl(state, accu, doomed);

      if (!result.empty()) {
        return result;
      }

      accu.pop_back();
    }

    state.Restore(j, removed);
  }
  if (doomed->size() < DOOMED_CACHE_SIZE) {
    doomed->insert(state.pile);
  }

  return {};
}

}  // namespace

std::vector<ValidSet> Solve(const Pile& pile) {
  SearchState state(pile);
  std::vector<size_t> completed{};
  std::unordered_set<Pile, PileHash> doomed{};
  completed = SolveImpl(state, completed, &doomed);
  std::vector<ValidSet> solution{};

  for (size_t j : completed) {
    solution.emplace_back(state.valid_sets[j]);
  }
  return solution;
}

}  // namespace rummibuk
