#ifndef SPACESECTORLLRBT_H
#define SPACESECTORLLRBT_H

#include "Sector.h"
#include <iostream>
#include <fstream>  
#include <sstream>
#include <vector>

class SpaceSectorLLRBT {
public:
    Sector* root;
    SpaceSectorLLRBT();
    ~SpaceSectorLLRBT();
    void readSectorsFromFile(const std::string& filename);
    void insertSectorByCoordinates(int x, int y, int z);
    void displaySectorsInOrder();
    void displaySectorsPreOrder();
    void displaySectorsPostOrder();
    std::vector<Sector*> getStellarPath(const std::string& sector_code);
    void printStellarPath(const std::vector<Sector*>& path);
    Sector *recursiveInsert(Sector *parent, Sector *&root, int x, int y, int z);
    void inOrderTraversal(Sector *root);
    void preOrderTraversal(Sector *root);
    void postOrderTraversal(Sector *root);
    bool isRed(Sector* node);
    Sector* rotateLeft(Sector* node);
    Sector* rotateRight(Sector* node);
    void flipColors(Sector* node);
    bool isInIt(std::string item, Sector *root);
    Sector *getSector(std::string item, Sector *root);
    std::vector<Sector *> getSectorsOnPath(Sector *root, std::vector<Sector *> &sectors, Sector *Elara, Sector *Earth);
    void deleteLLRBT(Sector *root);
};

#endif // SPACESECTORLLRBT_H
