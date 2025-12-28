#include <MazeGen.hpp>
#include <gtest/gtest.h>

TEST(MazeGenTest, Instantiation) {
    MazeGen gen;
    // Verify object creation
    SUCCEED();
}

TEST(MazeGenTest, InitializationShowsFirstGrid) {
    MazeGen gen;
    gen.initialize(5, 5);

    // After initialization, grid should be available
    const auto &grid = gen.get_grid();
    EXPECT_EQ(grid.size(), 25);  // 5x5 = 25 cells

    // First cell (0,0) should be marked as visited
    EXPECT_TRUE(grid[0].visited);

    // All other cells should not be visited yet
    for (size_t i = 1; i < grid.size(); ++i) {
        EXPECT_FALSE(grid[i].visited);
    }
}

TEST(MazeGenTest, StepByStepGeneration) {
    MazeGen gen;
    gen.initialize(3, 3);

    // Grid should be available immediately after init
    auto grid = gen.get_grid();
    EXPECT_EQ(grid.size(), 9);

    // Perform one step
    gen.step();

    // Grid should be updated after step
    grid = gen.get_grid();
    EXPECT_EQ(grid.size(), 9);

    // At least the starting cell should be visited
    EXPECT_TRUE(grid[0].visited);
}
