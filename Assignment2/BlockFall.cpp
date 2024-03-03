#include "BlockFall.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

BlockFall::BlockFall(string grid_file_name, string blocks_file_name, bool gravity_mode_on,
                     const string &leaderboard_file_name, const string &player_name) : gravity_mode_on(
        gravity_mode_on), leaderboard_file_name(leaderboard_file_name), player_name(player_name) {
    initialize_grid(grid_file_name);
    read_blocks(blocks_file_name);
    leaderboard.read_from_file(leaderboard_file_name);
}


void BlockFall::read_blocks(const string &input_file) {

    std::ifstream blockFile(input_file);

    if (!blockFile.is_open()) {
        return;
    }

    Block *q;
    Block *lastBefore;
    std::string line;
    std::vector<bool> row;
    std::vector<std::vector<bool>> shape;

    while (std::getline(blockFile, line)) {
        if (!line.empty()) {
            for (char c: line) {
                if (c == '0') {
                    row.push_back(false);
                } else if (c == '1') {
                    row.push_back(true);
                }
            }

            if (line.find(']') != std::string::npos) {
                shape.push_back(row);
                Block *newNode = new Block;
                newNode->shape = shape;
                if (initial_block == nullptr) {
                    initial_block = newNode;
                    q = initial_block;
                } else {
                    lastBefore = q;
                    q->next_block = newNode;
                    q = newNode;
                }
                row.clear();
                shape.clear();
            } else {
                if (!row.empty()) {
                    shape.push_back(row);
                }
                row.clear();
            }

        }

    }

    vector<bool> power_row;
    for (int i = 0; i < q->shape.size(); i++) {
        {
            for (int j = 0; j < q->shape[0].size(); j++) {
                power_row.push_back(q->shape[i][j]);
            }
            power_up.push_back(power_row);
            power_row.clear();
        }

    }
    delete q;
    lastBefore->next_block = nullptr;

    active_rotation = initial_block;

    Block *x = initial_block;
    while (x != nullptr) {
        create_doubly_circular_linked_list(x);
        x = x->next_block;
    }

    blockFile.close();

}

// TODO: Read the blocks from the input file and initialize "initial_block" and "active_rotation" member variables
// TODO: For every block, generate its rotations and properly implement the multilevel linked list structure
//       that represents the game blocks, as explained in the PA instructions.
// TODO: Initialize the "power_up" member variable as the last block from the input file (do not add it to the linked list!)


void BlockFall::initialize_grid(const string &input_file) {

    std::ifstream gridFile(input_file);

    if (!gridFile.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(gridFile, line)) {
        if (!line.empty()) {
            std::istringstream iss(line);
            std::vector<int> row;
            int value;
            while (iss >> value) {
                if (value == 0 || value == 1) {
                    row.push_back(value);
                }

            }
            grid.push_back(row);
        }
    }
    if (!grid.empty()) {
        rows = grid.size();
        if (!grid[0].empty()) {
            cols = grid[0].size();
        }

    }

    gridFile.close();
}


void BlockFall::create_doubly_circular_linked_list(Block *block) {
    Block *head = block;
    Block *right;
    while (true) {
        right = rotate_right(block);
        if (right != nullptr) {
            if (right->shape != head->shape) {
                block->right_rotation = right;
                right->left_rotation = block;
                right->next_block = head->next_block;
            } else {
                block->right_rotation = head;
                head->left_rotation = block;
                block->next_block = head->next_block;
                delete right;
                break;
            }
            block = block->right_rotation;
        }

    }

}


Block *BlockFall::rotate_right(Block *block) {
    if (!block) {
        return nullptr;
    }
    Block *right = new Block();
    std::vector<std::vector<bool>> rmatrix;

    // right
    for (int j = 0; j < block->shape[0].size(); j++) {
        std::vector<bool> row1;
        for (int i = block->shape.size() - 1; i >= 0; i--) {
            row1.push_back(block->shape[i][j]);
        }
        rmatrix.push_back(row1);
    }

    right->shape = rmatrix;
    return right;
}


bool BlockFall::check_enter_collision() {
    for (int i = 0; i < active_rotation->shape.size(); i++) {
        for (int j = 0; j < active_rotation->shape[0].size(); j++) {
            if (i < grid.size() && j < grid[i].size()) {
                if (grid[i][j] == 1 && active_rotation->shape[i][j] == 1) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool BlockFall::right_rotation_collision() {
    if (!active_rotation || !active_rotation->right_rotation) {
        return false;
    }
    for (int i = 0; i < active_rotation->right_rotation->shape.size(); i++) {
        for (int j = 0; j < active_rotation->right_rotation->shape[0].size(); j++) {
            if (active_rotation->right_rotation->shape[i][j]) {
                if (j + active_rotation->index > grid[0].size() - 1 || j + active_rotation->index < 0 ||
                    grid[i][j + active_rotation->index] == 1) {
                    return true;
                }

            }
        }
    }

    return false;
}

bool BlockFall::left_rotation_collision() {
    if (!active_rotation || !active_rotation->right_rotation) {
        return false;
    }
    for (int i = 0; i < active_rotation->left_rotation->shape.size(); i++) {
        for (int j = 0; j < active_rotation->left_rotation->shape[0].size(); j++) {
            if (active_rotation->left_rotation->shape[i][j]) {
                if (j + active_rotation->index < 0 || j + active_rotation->index > grid[0].size() - 1 ||
                    grid[i][j + active_rotation->index] == 1) {
                    return true;
                }

            }
        }
    }

    return false;
}

bool BlockFall::right_movement_collision() {
    for (int i = 0; i < active_rotation->shape.size(); i++) {
        for (int j = 0; j < active_rotation->shape[0].size(); j++) {
            if (active_rotation->shape[i][j]) {
                if ((j + 1 + active_rotation->index) > grid[0].size() - 1 ||
                    grid[i][j + 1 + active_rotation->index] == 1
                        ) {
                    return true;
                }


            }
        }
    }

    return false;

}

bool BlockFall::left_movement_collision() {
    for (int i = 0; i < active_rotation->shape.size(); i++) {
        for (int j = 0; j < active_rotation->shape[0].size(); j++) {
            if (active_rotation->shape[i][j]) {
                if ((j - 1 + active_rotation->index) < 0 || (j - 1 + active_rotation->index) > grid[0].size() - 1 ||
                    grid[i][j - 1 + active_rotation->index] == 1) {
                    return true;
                }
            }
        }
    }
    return false;

}

bool BlockFall::is_row_full(std::vector<int> row) {
    for (int i = 0; i < row.size(); i++) {
        if (row[i] == 0) {
            return false;
        }
    }
    return true;
}

int BlockFall::full_index_num() {
    int full_index_num = 0;
    for (int i = 0; i < active_rotation->shape.size(); i++) {
        for (int j = 0; j < active_rotation->shape[0].size(); j++) {
            if (active_rotation->shape[i][j]) {
                full_index_num++;
            }
        }
    }
    return full_index_num;
}

bool BlockFall::has_power_up() {
    for (int i = 0; i <= grid.size() - power_up.size(); i++) {
        for (int j = 0; j <= grid[0].size() - power_up[0].size(); j++) {
            bool found = true;
            for (int k = 0; k < power_up.size(); k++) {
                for (int l = 0; l < power_up[0].size(); l++) {
                    if (i + k < grid.size() && j + l < grid[0].size()) {
                        if (grid[i + k][j + l] != power_up[k][l]) {
                            found = false;
                            break;
                        }
                    }
                }
                if (!found) {
                    break;
                }
            }
            if (found) {
                return true;
            }
        }
    }
    return false;
}


void BlockFall::print_before_clearing() {
    std::cout << "Before clearing:" << std::endl;
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            std::cout << (grid[i][j] ? occupiedCellChar
                                     : unoccupiedCellChar);
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

void BlockFall::drop_current_blocks() {
    for (int i = grid.size() - 2; i >= 0; i--) {
        for (int j = 0; j < grid[0].size(); j++) {
            if (grid[i][j] == 1) {
                int drop_gravity = 0;
                while (drop_gravity + i + 1 < grid.size() &&
                       grid[i + 1 + drop_gravity][j] == 0) {
                    drop_gravity++;
                }
                if (drop_gravity != 0) {
                    grid[i][j] = 0;
                    grid[i + drop_gravity][j] = 1;
                }
            }
        }
    }
}

void BlockFall::full_row_erase(vector<int> completed_rows) {
    for (int i = completed_rows.size() - 1; i >= 0; i--) {
        grid.erase(grid.begin() + completed_rows[i]);
    }
    current_score += grid[0].size() * completed_rows.size();
    grid.insert(grid.begin(), completed_rows.size(), std::vector<int>(cols, 0));
}

void BlockFall::exec_power_up() {
    current_score += 1000;
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++)
            if (grid[i][j] == 1) {
                current_score += 1;
            }
    }
    for (int i = 0; i < grid.size(); ++i) {
        for (int j = 0; j < grid[0].size(); ++j)
            grid[i][j] = 0;
    }
}

std::vector<int> BlockFall::full_row_index() {
    std::vector<int> completed_rows;
    for (int i = 0; i < grid.size(); i++) {
        if (is_row_full(grid[i])) {
            completed_rows.push_back(i);
        }
    }
    return completed_rows;
}

void BlockFall::gravity_off() {
    int drop_index = -1;
    for (int i = active_rotation->shape.size() - 1; i >= 0; i--) {
        for (int j = 0; j < active_rotation->shape[0].size(); j++) {
            if (active_rotation->shape[i][j] == 1) {
                int current_drop_index = 0;
                while (current_drop_index + i + 1 < grid.size() &&
                       grid[current_drop_index + i + 1][active_rotation->index + j] == 0
                        ) {
                    current_drop_index++;
                }
                if (drop_index == -1) {
                    drop_index = current_drop_index;
                }
                if (current_drop_index < drop_index) {
                    drop_index = current_drop_index;
                }

            }

        }
    }

    for (int i = active_rotation->shape.size() - 1; i >= 0; i--) {
        for (int j = 0; j < active_rotation->shape[0].size(); j++) {
            if (active_rotation->shape[i][j] == 1) {
                grid[drop_index + i][
                        active_rotation->index + j] = 1;
            }

        }
    }
    if (drop_index >= 0) {
        current_score += drop_index * full_index_num();
    }

}

void BlockFall::gravity_on() {
    int less_drop_index = -1;
    for (int i = active_rotation->shape.size() - 1; i >= 0; i--) {
        for (int j = 0; j < active_rotation->shape[0].size(); j++) {
            if (active_rotation->shape[i][j] == 1) {
                int drop_index = 0;
                while (drop_index + i + 1 < grid.size() &&
                       grid[drop_index + i + 1][active_rotation->index + j] == 0) {
                    drop_index++;
                }
                if (less_drop_index == -1) {
                    less_drop_index = drop_index;
                } else if (less_drop_index > drop_index) {
                    less_drop_index = drop_index;
                }
                grid[drop_index + i][active_rotation->index + j] = 1;
            }

        }
    }
    if (less_drop_index >= 0) {
        current_score += less_drop_index * full_index_num();
    }
}

void BlockFall::print_grid() {
    std::cout << "Score: " << current_score << std::endl;
    if (leaderboard.head_leaderboard_entry == nullptr) {
        std::cout << "High Score: "
                  << current_score
                  << std::endl;
    } else {
        std::cout << "High Score: "
                  << (current_score > leaderboard.head_leaderboard_entry->score ? current_score
                                                                                : leaderboard.head_leaderboard_entry->score)
                  << std::endl;
    }
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            if (i < active_rotation->shape.size() && (active_rotation->index <= j) &&
                (j < active_rotation->shape[0].size() + active_rotation->index)) {
                if (j - active_rotation->index < grid[0].size() &&
                    j - active_rotation->index >= 0 &&
                    grid[i][j] == 1 &&
                    active_rotation->shape[i][j] == 0) {
                    std::cout << (grid[i][j - active_rotation->index] ? occupiedCellChar
                                                                      : unoccupiedCellChar);
                } else {
                    std::cout << (active_rotation->shape[i][j - active_rotation->index]
                                  ? occupiedCellChar
                                  : unoccupiedCellChar);
                }

            } else {
                std::cout << (grid[i][j] ? occupiedCellChar
                                         : unoccupiedCellChar);

            }

        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << std::endl;

}

void BlockFall::destroy_circular_linkedlist(Block *current) {

    while (current->right_rotation != current) {
        Block *p = current->right_rotation;
        p->right_rotation->left_rotation = current;
        current->right_rotation = p->right_rotation;
        delete p;
    }
    delete current;

}

BlockFall::~BlockFall() {
    Block *current = initial_block;
    Block *temp;
    while (current != nullptr) {
        temp = current->next_block;
        destroy_circular_linkedlist(current);
        current = temp;
    }
    // TODO: Free dynamically allocated memory used for storing game blocks
}

