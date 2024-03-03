#ifndef BLOCKFALL_H
#define BLOCKFALL_H

#define occupiedCellChar "██"
#define unoccupiedCellChar "▒▒"

#include <vector>
#include <string>

#include "Block.h"
#include "LeaderboardEntry.h"
#include "Leaderboard.h"

using namespace std;

class BlockFall {
public:

    BlockFall(string grid_file_name, string blocks_file_name, bool gravity_mode_on, const string &leaderboard_file_name,
              const string &player_name);
    virtual ~BlockFall();

    int rows;  // Number of rows in the grid
    int cols;  // Number of columns in the grid
    vector<vector<int>> grid;  // 2D game grid
    vector<vector<bool>> power_up; // 2D matrix of the power-up shape
    Block * initial_block = nullptr; // Head of the list of game blocks. Must be filled up and initialized after a call to read_blocks()
    Block * active_rotation = nullptr; // Currently active rotation of the active block. Must start with the initial_block
    bool gravity_mode_on = false; // Gravity mode of the game
    unsigned long current_score = 0; // Current score of the game
    string leaderboard_file_name; // Leaderboard file name, taken from the command-line argument 5 in main
    string player_name; // Player name, taken from the command-line argument 6 in main
    Leaderboard leaderboard;

    void initialize_grid(const string & input_file); // Initializes the grid using the command-line argument 1 in main
    void read_blocks(const string & input_file); // Reads the input file and calls the read_block() function for each block;
    Block* rotate_right( Block * block);
    void create_doubly_circular_linked_list( Block* block);
    bool check_enter_collision();
    bool right_rotation_collision();
    bool left_rotation_collision();
    bool right_movement_collision();
    bool left_movement_collision();
    bool is_row_full(std::vector<int> a);
    int full_index_num();
    bool has_power_up();
    void destroy_circular_linkedlist(Block *block);
    void drop_current_blocks();
    void print_before_clearing();
    void exec_power_up();
    void full_row_erase(vector<int> completed_rows);
    void print_grid();
    void gravity_on();
    void gravity_off();
    std::vector<int> full_row_index();
};



#endif // BLOCKFALL_H
