#include "GameController.h"
#include <iostream>
#include <fstream>
#include <vector>

bool GameController::play(BlockFall &game, const string &commands_file) {

    std::ifstream commandFile(commands_file);

    if (!commandFile.is_open()) {
        return false;
    }

    if (game.grid.empty() || game.initial_block == nullptr) {
        return game_over(game);
    }

    std::string command;
    while (std::getline(commandFile, command)) {
        if (command.find("PRINT_GRID") != std::string::npos) {
            game.print_grid();

        } else if (command.find("ROTATE_RIGHT") != std::string::npos) {
            if (!game.right_rotation_collision()) {
                int index = game.active_rotation->index;
                game.active_rotation = game.active_rotation->right_rotation;
                game.active_rotation->index = index;
            }
        } else if (command.find("ROTATE_LEFT") != std::string::npos) {
            if (!game.left_rotation_collision()) {
                int index = game.active_rotation->index;
                game.active_rotation = game.active_rotation->left_rotation;
                game.active_rotation->index = index;
            }
        } else if (command.find("MOVE_RIGHT") != std::string::npos) {
            if (!game.right_movement_collision()) {
                game.active_rotation->index += 1;
            }
        } else if (command.find("MOVE_LEFT") != std::string::npos) {
            if (!game.left_movement_collision()) {
                game.active_rotation->index -= 1;
            }
        } else if (command.find("DROP") != std::string::npos) {
            if (game.gravity_mode_on) {
                game.gravity_on();
            } else {
                game.gravity_off();
            }

            vector<int> completed_rows = game.full_row_index();
            if (game.has_power_up()) {
                game.print_before_clearing();
                game.exec_power_up();
            } else if (!completed_rows.empty()) {
                game.print_before_clearing();
                game.full_row_erase(completed_rows);
            }

            if (game.active_rotation->next_block != nullptr) {
                game.active_rotation = game.active_rotation->next_block;
                if (game.check_enter_collision()) {
                    return game_over(game);
                }
            } else {
                return win(game);
            }

        } else if (command.find("GRAVITY_SWITCH") != std::string::npos) {

            if (!game.gravity_mode_on) {
                game.drop_current_blocks();
            }
            std::vector<int> completed_rows = game.full_row_index();

            if (!completed_rows.empty()) {
                game.print_before_clearing();
                game.full_row_erase(completed_rows);
            }
            game.gravity_mode_on = !game.gravity_mode_on;

        } else {
            std::string resultString;
            if (!command.empty()) {
                for (char c: command) {
                    if (c != '\n' && c != '\r') {
                        resultString += c;
                    }
                }
            }

            std::cout << "Unknown command: " << resultString << std::endl;
        }

    }
    game_finished(game);
    return true;
}


bool GameController::game_over(BlockFall &game) {
    LeaderboardEntry *current_leader_board = new LeaderboardEntry(game.current_score, time(nullptr),
                                                                  game.player_name);
    leaderboardEntry_game = current_leader_board;
    std::cout << "GAME OVER!\nNext block that couldn't fit:" << std::endl;
    for (int i = 0; i < game.active_rotation->shape.size(); i++) {
        for (int j = 0; j < game.active_rotation->shape[0].size(); j++) {
            std::cout << (game.active_rotation->shape[i][j] ? occupiedCellChar
                                                            : unoccupiedCellChar);
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    print_for_end(game);
    game.leaderboard.insert_new_entry(current_leader_board);
    game.leaderboard.print_leaderboard();
    game.leaderboard.write_to_file(game.leaderboard_file_name);

    return false;
}

bool GameController::win(BlockFall &game) {
    LeaderboardEntry *current_leader_board = new LeaderboardEntry(game.current_score, time(nullptr),
                                                                  game.player_name);
    leaderboardEntry_game = current_leader_board;
    std::cout << "YOU WIN!\nNo more blocks." << std::endl;
    print_for_end(game);
    game.leaderboard.insert_new_entry(current_leader_board);
    game.leaderboard.print_leaderboard();
    game.leaderboard.write_to_file(game.leaderboard_file_name);

    return true;
}

void GameController::game_finished(BlockFall &game) {
    LeaderboardEntry *current_leader_board = new LeaderboardEntry(game.current_score, time(nullptr),
                                                                  game.player_name);
    leaderboardEntry_game = current_leader_board;
    std::cout << "GAME FINISHED!\nNo more commands." << std::endl;
    print_for_end(game);
    game.leaderboard.insert_new_entry(current_leader_board);
    game.leaderboard.print_leaderboard();
    game.leaderboard.write_to_file(game.leaderboard_file_name);
}

void GameController::print_for_end(BlockFall &game){
    std::cout << "Final grid and score:" << std::endl;
    std::cout << std::endl;
    std::cout << "Score: " << game.current_score << std::endl;
    if (game.leaderboard.head_leaderboard_entry == nullptr) {
        std::cout << "High Score: "
                  << game.current_score
                  << std::endl;
    } else {
        std::cout << "High Score: "
                  << (game.current_score > game.leaderboard.head_leaderboard_entry->score ? game.current_score
                                                                                          : game.leaderboard.head_leaderboard_entry->score)
                  << std::endl;
    }
    for (int i = 0; i < game.grid.size(); i++) {
        for (int j = 0; j < game.grid[0].size(); j++) {
            std::cout << (game.grid[i][j] ? occupiedCellChar
                                          : unoccupiedCellChar);

        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

}

