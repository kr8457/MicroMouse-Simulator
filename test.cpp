#include "MazeSolver.hpp"
#include "MazeGen.hpp"

using Graph = std::vector<std::vector<size_t>>;

auto convert_to_graph(std::vector<Cell> &grid, size_t rows, size_t cols) -> Graph {
    // 10 x 10 maze
    // dimensions of graph: ( , )

    // Cell is a struct that contains (top, botttom, left, right)

    //In graph:
    // Every row = cell
    // Every column = whether wall exists or not (4 binary values for 4 cardinal directions (N,S,W,E))

    Graph graph(rows*cols, std::vector<size_t>(4, 1));
    
    for(int i = 0; i <  rows*cols; i++) {
        
        // 1 1 1 1 (1 for wall and 0 for no wall)
        // l r t b (what they represent)
        // 0 1 2 3 (graph colum indexes)

        //Check all 4 directions for each node and appropriatly update the graph edge value
        if(not grid[i].left) graph[i][0] = 0;
        if(not grid[i].right) graph[i][1] = 0;
        if(not grid[i].top) graph[i][2] = 0;
        if(not grid[i].bottom) graph[i][3] = 0;
    }
}  