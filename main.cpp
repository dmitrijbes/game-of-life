#include <array>
#include <cstdio>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

struct Cell {
    bool is_alive = false;
};

constexpr int grid_size = 20;
using Row = std::array<Cell, grid_size>;
using Matrix = std::array<Row, grid_size>;

class Grid {
    Matrix matrix;

    Row calculateRow(const int row_index);
    bool calculateCell(const int row_index, const int cell_index);

  public:
    Row& operator[](const int row_index);

    int size();
    void print();
    void turn();

    void placeGlider();
};

Row Grid::calculateRow(const int row_index) {
    Row row;
    for (int i = 0; i != row.size(); ++i) {
        row[i].is_alive = calculateCell(row_index, i);
    }

    return row;
}
bool Grid::calculateCell(const int row_index, const int cell_index) {
    int alive_neighbours = 0;

    std::vector<std::pair<int, int>> directions{
        {0, 1}, {1, 0}, {-1, 0}, {0, -1}, {1, 1}, {-1, -1}, {-1, 1}, {1, -1}};
    for (auto& direction : directions) {
        const int neighbour_row_index = row_index + direction.first;
        const int neighbour_cell_index = cell_index + direction.second;

        if (neighbour_row_index < 0 || neighbour_row_index >= size()) continue;
        if (neighbour_cell_index < 0 || neighbour_cell_index >= size())
            continue;

        if (matrix[neighbour_row_index][neighbour_cell_index].is_alive)
            ++alive_neighbours;
    }

    if (matrix[row_index][cell_index].is_alive) {
        if (alive_neighbours < 2 || alive_neighbours > 3) {
            return false;
        } else {
            return true;
        }
    } else {
        if (alive_neighbours == 3) {
            return true;
        } else {
            return false;
        }
    }
}

Row& Grid::operator[](const int row_index) { return matrix[row_index]; }

int Grid::size() { return matrix.size(); }

void Grid::print() {
    for (auto& row : matrix) {
        for (auto& cell : row) {
            if (cell.is_alive) {
                std::cout << '#';
            } else {
                std::cout << "~";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Grid::turn() {
    // Array of futures to get result from each thread.
    std::array<std::future<Row>, grid_size> futures;

    // Create thread for each Row calculation.
    for (int i = 0; i != size(); ++i) {
        std::packaged_task<Row(Grid*, int)> task(&Grid::calculateRow);
        futures[i] = task.get_future();
        std::thread thread(std::move(task), this, i);
        thread.join();
    }

    for (int i = 0; i != size(); ++i) {
        matrix[i] = futures[i].get();
    }
}

void Grid::placeGlider() {
    matrix[3][3].is_alive = true;
    matrix[3][4].is_alive = true;
    matrix[3][5].is_alive = true;
    matrix[2][5].is_alive = true;
    matrix[1][4].is_alive = true;
}

int main() {
    Grid world;
    world.placeGlider();

    constexpr int turns = 20;
    for (int i = 0; i < turns; ++i) {
        world.turn();
        world.print();
    }

    // Keep console window open until you press any key.
    std::getchar();
}
