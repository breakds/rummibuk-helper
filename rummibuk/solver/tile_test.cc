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

}  // namespace rummibuk::testing
