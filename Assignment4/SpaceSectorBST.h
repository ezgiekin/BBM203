#ifndef SPACESECTORBST_H
#define SPACESECTORBST_H

#include <iostream>
#include <fstream>  
#include <sstream>
#include <vector>

#include "Sector.h"

class SpaceSectorBST {
  
public:
    Sector *root;
    SpaceSectorBST();
    ~SpaceSectorBST();
    void readSectorsFromFile(const std::string& filename); 
    void insertSectorByCoordinates(int x, int y, int z);
    void deleteSector(const std::string& sector_code);
    void displaySectorsInOrder();
    void inOrderTraversal(Sector *root);
    void displaySectorsPreOrder();
    void preOrderTraversal(Sector *root);
    void displaySectorsPostOrder();
    void postOrderTraversal(Sector *root);
    std::vector<Sector*> getStellarPath(const std::string& sector_code);
    void printStellarPath(const std::vector<Sector*>& path);
    Sector* recursiveInsert(Sector* parent,Sector*& root,int x, int y, int z);
    bool isInIt(std::string item,Sector* Root);
    std::vector<Sector *> getSectorsOnPath(Sector * root,std::vector<Sector *> &sectors,Sector* Elara);
    Sector * getSector(std::string item,Sector* Root);
    Sector * findMin(Sector *root);
    void deleteBST(Sector * root);
};

#endif // SPACESECTORBST_H
