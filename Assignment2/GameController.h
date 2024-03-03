#ifndef PA2_GAMECONTROLLER_H
#define PA2_GAMECONTROLLER_H

#include "BlockFall.h"

using namespace std;

class GameController {
public:
    bool play(BlockFall &game, const string &commands_file); // Function that implements the gameplay
    LeaderboardEntry *leaderboardEntry_game = nullptr;
    bool game_over(BlockFall &game);
    bool win(BlockFall &game);
    void game_finished(BlockFall &game);
    void print_for_end(BlockFall &game);


};


#endif //PA2_GAMECONTROLLER_H
