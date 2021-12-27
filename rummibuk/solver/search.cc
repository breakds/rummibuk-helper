#include "rummibuk/solver/search.h"

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

}  // namespace rummibuk
