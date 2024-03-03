#ifndef PA2_BLOCK_H
#define PA2_BLOCK_H

#include <vector>

using namespace std;

class Block {
public:

    vector<vector<bool>> shape; // Two-dimensional vector corresponding to the block's shape
    Block *right_rotation = nullptr; // Pointer to the block's clockwise neighbor block (its right rotation)
    Block *left_rotation = nullptr; // Pointer to the block's counter-clockwise neighbor block (its left rotation)
    Block *next_block = nullptr; // Pointer to the next block to appear in the game
    int index = 0;

    bool operator==(const Block &other) const {
        for (int i = 0; i < other.shape.size(); i++) {
            for (int j = 0; j < other.shape[0].size(); j++) {
                if (shape[i][j] != other.shape[i][j]){
                    return false;
                }
            }
        }
        return true;
    }

    bool operator!=(const Block &other) const {
        for (int i = 0; i < other.shape.size(); i++) {
            for (int j = 0; j < other.shape[0].size(); j++) {
                if (shape[i][j] != other.shape[i][j]){
                    return true;
                }
            }
        }
        return false;
    }
};


#endif //PA2_BLOCK_H
