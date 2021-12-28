#include "rummibuk/solver/search.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "spdlog/spdlog.h"

using ::testing::ElementsAre;

namespace rummibuk::testing {

bool CheckEqual(const std::vector<std::string> expected,
                const std::vector<ValidSet> result) {
  std::vector<ValidSet> expected_sets;

  for (const std::string &input : expected) {
    expected_sets.emplace_back(ValidSet::FromString(input));
  }

  for (const ValidSet &x : result) {
    if (std::find(expected_sets.begin(), expected_sets.end(), x) == expected_sets.end()) {
      spdlog::error("Unexpected result set {}", x.ToString());
      return false;
    }
  }

  bool success = true;

  for (const ValidSet &x : expected_sets) {
    if (std::find(result.begin(), result.end(), x) == result.end()) {
      spdlog::error("Expects {} but it is not found in the result.", x.ToString());
      success = false;
      break;
    }
  }

  if (!success) {
    spdlog::info("See below for the result set");
    for (const ValidSet x : result) {
      spdlog::info("{}", x.ToString());
    }
  }

  return success && expected_sets.size() == result.size();
}

TEST(InitializeValidSetsTest, NoValidSets) {
  {
    Pile pile;
    auto sets = InitializeValidSets(pile);
    EXPECT_THAT(sets, ElementsAre());
  }

  {
    Pile pile;
    pile.Add("R8");
    pile.Add("R9");
    auto sets = InitializeValidSets(pile);
    EXPECT_THAT(sets, ElementsAre());
  }

  {
    Pile pile;
    pile.Add("R7");
    pile.Add("R8");
    pile.Add("R9");
    auto sets = InitializeValidSets(pile);
    EXPECT_THAT(sets, ElementsAre());
  }

  {
    Pile pile;
    pile.Add("O7");
    pile.Add("R7");
    pile.Add("R8");
    pile.Add("R9");
    auto sets = InitializeValidSets(pile);
    EXPECT_THAT(sets, ElementsAre());
  }
}

TEST(InitializeValidSetsTest, SimpleGroups) {
  {
    Pile pile;
    pile.Add("R9");
    pile.Add("O9");
    pile.Add("K9");
    auto sets = InitializeValidSets(pile);
    EXPECT_TRUE(CheckEqual({"GRP(R9 O9 K9)"}, sets));
  }

  {
    Pile pile;
    pile.Add("R9");
    pile.Add("O9");
    pile.Add("K9");
    pile.Add("[]");
    auto sets = InitializeValidSets(pile);
    EXPECT_TRUE(CheckEqual({"GRP(R9 O9 K9)",
                            "GRP(R9 O9 K9 [])",
                            "GRP(R9 O9 [])",
                            "GRP(R9 K9 [])",
                            "GRP(O9 K9 [])"},
                           sets));
  }

  {
    Pile pile;
    pile.Add("R9");
    pile.Add("O9");
    pile.Add("[]");
    auto sets = InitializeValidSets(pile);
    EXPECT_TRUE(CheckEqual({"GRP(R9 O9 [])"}, sets));
  }

  {
    Pile pile;
    pile.Add("R9");
    pile.Add("[]");
    pile.Add("[]");
    auto sets = InitializeValidSets(pile);
    EXPECT_TRUE(CheckEqual({"GRP(R9 [] [])"}, sets));
  }

  {
    Pile pile;
    pile.Add("R9");
    pile.Add("K9");
    pile.Add("[]");
    pile.Add("[]");
    auto sets = InitializeValidSets(pile);
    EXPECT_TRUE(CheckEqual(
        {"GRP(R9 [] [])", "GRP(K9 [] [])", "GRP(R9 K9 [] [])", "GRP(R9 K9 []"}, sets));
  }

  {
    Pile pile;
    pile.Add("O9");
    pile.Add("R9");
    pile.Add("K9");
    pile.Add("[]");
    pile.Add("[]");
    auto sets = InitializeValidSets(pile);
    EXPECT_TRUE(CheckEqual({"GRP(R9 [] [])",
                            "GRP(K9 [] [])",
                            "GRP(O9 [] [])",
                            "GRP(R9 K9 [])",
                            "GRP(O9 K9 [])",
                            "GRP(R9 O9 [])",
                            "GRP(R9 K9 [] [])",
                            "GRP(O9 K9 [] [])",
                            "GRP(R9 O9 [] [])",
                            "GRP(R9 O9 K9)",
                            "GRP(R9 O9 K9 []"},
                           sets));
  }

  {
    Pile pile;
    pile.Add("[]");
    pile.Add("[]");
    pile.Add("O9");
    pile.Add("R9");
    pile.Add("B9");
    pile.Add("K9");
    auto sets = InitializeValidSets(pile);
    EXPECT_TRUE(CheckEqual(
        {"GRP(R9 [] [])",    "GRP(K9 [] [])",    "GRP(O9 [] [])",    "GRP(B9 [] [])",
         "GRP(R9 K9 [])",    "GRP(O9 K9 [])",    "GRP(R9 O9 [])",    "GRP(B9 R9 [])",
         "GRP(B9 O9 [])",    "GRP(B9 K9 [])",    "GRP(R9 K9 [] [])", "GRP(O9 K9 [] [])",
         "GRP(R9 O9 [] [])", "GRP(B9 R9 [] [])", "GRP(B9 O9 [] [])", "GRP(B9 K9 [] [])",
         "GRP(R9 O9 K9)",    "GRP(B9 O9 K9)",    "GRP(B9 R9 K9)",    "GRP(B9 R9 O9)",
         "GRP(R9 O9 K9 [])", "GRP(B9 O9 K9 [])", "GRP(B9 R9 K9 [])", "GRP(B9 R9 O9 [])",
         "GRP(B9 R9 O9 K9)"},
        sets));
  }
}

TEST(InitializeValidSetsTest, SimpleRuns) {
  {
    Pile pile;
    pile.Add("O1");
    pile.Add("O2");
    pile.Add("O3");
    auto sets = InitializeValidSets(pile);
    EXPECT_TRUE(CheckEqual({"RUN(O1 O2 O3)"}, sets));
  }

  {
    Pile pile;
    pile.Add("O1");
    pile.Add("O3");
    pile.Add("[]");
    auto sets = InitializeValidSets(pile);
    EXPECT_TRUE(CheckEqual({"RUN(O1 O3 [])"}, sets));
  }

  {
    Pile pile;
    pile.Add("O2");
    pile.Add("O3");
    pile.Add("[]");
    auto sets = InitializeValidSets(pile);
    EXPECT_TRUE(CheckEqual({"RUN(O2 O3 [])"}, sets));
  }

  {
    Pile pile;
    pile.Add("O3");
    pile.Add("O1");
    pile.Add("O2");
    pile.Add("[]");
    auto sets = InitializeValidSets(pile);
    EXPECT_TRUE(CheckEqual({"RUN(O1 O2 O3 [])",
                            "RUN(O1 O2 O3)",
                            "RUN(O1 O2 [])",
                            "RUN(O2 O3 [])",
                            "RUN(O1 O3 [])"},
                           sets));
  }

  {
    Pile pile;
    pile.Add("O4");
    pile.Add("O1");
    pile.Add("O2");
    pile.Add("[]");
    auto sets = InitializeValidSets(pile);
    EXPECT_TRUE(CheckEqual({"RUN(O1 O2 [] O4)", "RUN(O1 O2 [])", "RUN(O2 O4 [])"}, sets));
  }

  {
    Pile pile;
    pile.Add("O9");
    pile.Add("O10");
    pile.Add("[]");
    pile.Add("[]");
    auto sets = InitializeValidSets(pile);
    EXPECT_TRUE(CheckEqual(
        {"RUN(O9 O10 [])", "RUN(O9 O10 [] [])", "GRP(O9 [] [])", "GRP(O10 [] [])"},
        sets));
  }

  {
    Pile pile;
    pile.Add("O9");
    pile.Add("O11");
    pile.Add("[]");
    pile.Add("O12");
    auto sets = InitializeValidSets(pile);
    EXPECT_TRUE(
        CheckEqual({"RUN(O9 O11 [])", "RUN(O11 O12 [])", "RUN(O9 O11 O12 [])"}, sets));
  }
}

TEST(InitializeValidSetsTest, Case1) {
  Pile pile;
  pile.Add("B3");
  pile.Add("B4");
  pile.Add("B5");
  pile.Add("B6");
  pile.Add("O4");
  pile.Add("R4");
  pile.Add("K4");
  auto sets = InitializeValidSets(pile);
  EXPECT_TRUE(CheckEqual({"RUN(B3 B4 B5)",
                          "RUN(B4 B5 B6)",
                          "RUN(B3 B4 B5 B6)",
                          "GRP(B4 R4 O4)",
                          "GRP(B4 O4 K4)",
                          "GRP(R4 O4 K4)",
                          "GRP(B4 R4 K4)",
                          "GRP(B4 R4 O4 K4)"},
                         sets));
}

TEST(InitializeValidSetsTest, Case2) {
  Pile pile;
  pile.Add("B3");
  pile.Add("B4");
  pile.Add("B5");
  pile.Add("B6");
  pile.Add("O4");
  pile.Add("R4");
  pile.Add("[]");
  pile.Add("K4");
  auto sets = InitializeValidSets(pile);
  EXPECT_TRUE(CheckEqual(
      {"RUN(B3 B4 B5)",    "RUN(B3 B4 B5 [])", "RUN(B4 B5 B6)",    "RUN(B4 B5 B6 [])",
       "RUN(B3 B5 B6 [])", "RUN(B3 B4 B6 [])", "RUN(B3 B4 B5 B6)", "RUN(B3 B4 B5 B6 [])",
       "RUN(B3 B4 [])",    "RUN(B3 B5 [])",    "RUN(B4 B5 [])",    "RUN(B4 B6 [])",
       "RUN(B5 B6 [])",    "GRP(B4 R4 [])",    "GRP(B4 O4 [])",    "GRP(B4 K4 [])",
       "GRP(R4 O4 [])",    "GRP(R4 K4 [])",    "GRP(O4 K4 [])",    "GRP(B4 R4 O4)",
       "GRP(B4 R4 O4 [])", "GRP(B4 O4 K4)",    "GRP(B4 O4 K4 [])", "GRP(R4 O4 K4)",
       "GRP(R4 O4 K4 [])", "GRP(B4 R4 K4)",    "GRP(B4 R4 K4 [])", "GRP(B4 R4 O4 K4)"},
      sets));
}

TEST(SolveTest, Case1) {
  Pile pile;
  pile.Add("K3");
  pile.Add("K4");
  pile.Add("K6");
  pile.Add("K7");
  pile.Add("K8");
  pile.Add("[]");

  auto solution = Solve(pile);
  spdlog::info("Solution is shown below:");
  for (const auto &valid_set : solution) {
    spdlog::info("{}", valid_set.ToString());
  }
}

TEST(SolveTest, Case2) {
  Pile pile;
  pile.Add("K3");
  pile.Add("K4");
  pile.Add("K5");
  pile.Add("K6");
  pile.Add("K7");
  pile.Add("R6");
  pile.Add("R7");
  pile.Add("R8");
  pile.Add("R8");
  pile.Add("B8");
  pile.Add("K8");

  auto solution = Solve(pile);
  spdlog::info("Solution is shown below:");
  for (const auto &valid_set : solution) {
    spdlog::info("{}", valid_set.ToString());
  }
}

TEST(SolveTest, Case3) {
  Pile pile;
  pile.Add("K3");
  pile.Add("K4");
  pile.Add("K5");
  pile.Add("K6");
  pile.Add("K7");
  pile.Add("R6");
  pile.Add("R7");
  pile.Add("R8");
  pile.Add("R8");
  pile.Add("B8");
  pile.Add("K8");
  pile.Add("B3");
  pile.Add("O3");
  pile.Add("K3");
  pile.Add("B9");
  pile.Add("O9");
  pile.Add("K9");
  pile.Add("B12");
  pile.Add("R12");
  pile.Add("O12");
  pile.Add("K2");
  pile.Add("B2");
  pile.Add("R2");
  pile.Add("O2");
  pile.Add("B9");
  pile.Add("R9");
  pile.Add("O9");
  pile.Add("R2");
  pile.Add("R3");
  pile.Add("R4");
  pile.Add("R9");
  pile.Add("R10");
  pile.Add("R11");
  pile.Add("R12");
  pile.Add("R13");
  pile.Add("B8");
  pile.Add("B9");
  pile.Add("B10");
  pile.Add("O9");
  pile.Add("O10");
  pile.Add("O11");
  pile.Add("B2");
  pile.Add("B3");
  pile.Add("B4");
  pile.Add("B1");
  pile.Add("O1");
  pile.Add("K1");
  pile.Add("[]");
  pile.Add("O11");
  pile.Add("K11");

  auto solution = Solve(pile);
  spdlog::info("Solution is shown below:");
  for (const auto &valid_set : solution) {
    spdlog::info("{}", valid_set.ToString());
  }
}

}  // namespace rummibuk::testing
