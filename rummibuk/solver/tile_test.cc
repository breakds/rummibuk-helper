#include "rummibuk/solver/tile.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::AllOf;
using ::testing::Field;

namespace rummibuk::testing {

TEST(TileTest, FromString) {
  {
    Tile tile = Tile::FromString("B10");
    EXPECT_THAT(tile, AllOf(Field(&Tile::color, Color::BLUE), Field(&Tile::number, 10)));
  }

  {
    Tile tile = Tile::FromString("R5");
    EXPECT_THAT(tile, AllOf(Field(&Tile::color, Color::RED), Field(&Tile::number, 5)));
  }

  {
    Tile tile = Tile::FromString("K1");
    EXPECT_THAT(tile, AllOf(Field(&Tile::color, Color::BLACK), Field(&Tile::number, 1)));
  }

  {
    Tile tile = Tile::FromString("O12");
    EXPECT_THAT(tile,
                AllOf(Field(&Tile::color, Color::ORANGE), Field(&Tile::number, 12)));
  }

  {
    Tile tile = Tile::FromString("W");
    EXPECT_TRUE(tile.IsWildcard());
  }
}

TEST(PileTest, IdOfAndTileOf) {
  for (size_t id = 0; id < 53; ++id) {
    EXPECT_EQ(id, Pile::IdOf(Pile::TileOf(id)));
  }
}

TEST(PileTest, Add) {
  Pile pile;

  EXPECT_EQ(0, pile.wildcards());

  Tile tile;
  pile.Add(tile);

  tile.color  = Color::BLACK;
  tile.number = 10;
  pile.Add(tile);
  pile.Add(tile);

  tile.color  = Color::ORANGE;
  tile.number = 7;
  pile.Add(tile);

  EXPECT_EQ(1, pile.wildcards());

  for (size_t id = 1; id <= 52; ++id) {
    if (id == 49) {
      EXPECT_EQ(2, pile.count(id));
    } else if (id == 33) {
      EXPECT_EQ(1, pile.count(id));
    } else {
      EXPECT_EQ(0, pile.count(id));
    }
  }
}

ValidSet MakeGroup(const std::vector<std::pair<Color, int>> &tiles, int wildcards) {
  std::vector<size_t> tile_ids{};
  for (const auto &[color, number] : tiles) {
    tile_ids.emplace_back(Pile::IdOf(Tile{
        .color  = color,
        .number = number,
    }));
  }
  return ValidSet::MakeGroup(tile_ids, wildcards);
}

TEST(ValidSetTest, Groups) {
  {
    EXPECT_EQ(
        "GRP(B8 R8 O8 K8)",
        MakeGroup(
            {{Color::BLACK, 8}, {Color::RED, 8}, {Color::BLUE, 8}, {Color::ORANGE, 8}}, 0)
            .ToString());
  }

  {
    EXPECT_EQ(
        "GRP(B8 R8 O8 [])",
        MakeGroup({{Color::RED, 8}, {Color::BLUE, 8}, {Color::ORANGE, 8}}, 1).ToString());
  }

  {
    EXPECT_EQ(
        "GRP(B8 R8 O8)",
        MakeGroup({{Color::RED, 8}, {Color::BLUE, 8}, {Color::ORANGE, 8}}, 0).ToString());
  }

  {
    EXPECT_EQ("GRP(B8 O8 [])",
              MakeGroup({{Color::BLUE, 8}, {Color::ORANGE, 8}}, 1).ToString());
  }

  {
    EXPECT_EQ("GRP(B8 O8 [] [])",
              MakeGroup({{Color::BLUE, 8}, {Color::ORANGE, 8}}, 2).ToString());
  }

  { EXPECT_EQ("GRP(O8 [] [])", MakeGroup({{Color::ORANGE, 8}}, 2).ToString()); }
}

ValidSet MakeRun(const std::vector<std::pair<Color, int>> &tiles, int wildcards) {
  std::vector<size_t> tile_ids{};
  for (const auto &[color, number] : tiles) {
    tile_ids.emplace_back(Pile::IdOf(Tile{
        .color  = color,
        .number = number,
    }));
  }
  return ValidSet::MakeRun(tile_ids, wildcards);
}

TEST(ValidSetTest, Runs) {
  {
    EXPECT_EQ(
        "RUN(K7 K8 K9 K10)",
        MakeRun(
            {{Color::BLACK, 7}, {Color::BLACK, 8}, {Color::BLACK, 9}, {Color::BLACK, 10}},
            0)
            .ToString());
  }

  {
    EXPECT_EQ("RUN(K7 K8 K10 [])",
              MakeRun({{Color::BLACK, 7}, {Color::BLACK, 8}, {Color::BLACK, 10}}, 1)
                  .ToString());
  }
}

TEST(ValidSetTest, Equality) {
  {
    EXPECT_EQ(MakeGroup({{Color::ORANGE, 8}, {Color::BLUE, 8}}, 1),
              MakeGroup({{Color::BLUE, 8}, {Color::ORANGE, 8}}, 1));
  }

  { EXPECT_EQ(MakeGroup({{Color::ORANGE, 8}}, 2), MakeRun({{Color::ORANGE, 8}}, 2)); }
}

}  // namespace rummibuk::testing
