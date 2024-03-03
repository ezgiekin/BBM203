#include "SpaceSectorBST.h"
#include <cmath>

using namespace std;

SpaceSectorBST::SpaceSectorBST() : root(nullptr) {}

SpaceSectorBST::~SpaceSectorBST() {
    // Free any dynamically allocated memory in this class.
    deleteBST(root);
}

void SpaceSectorBST::deleteBST(Sector *root) {
    if (root == nullptr) {
        return;
    } else {
        deleteBST(root->left);
        deleteBST(root->right);
        delete root;
    }
}

void SpaceSectorBST::readSectorsFromFile(const std::string &filename) {
    // TODO: read the sectors from the input file and insert them into the BST sector map
    // according to the given comparison critera based on the sector coordinates.

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
    }

    string line;

    vector<vector<string>> all_coordinates;
    std::getline(file, line);
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        string coordinate;
        vector<string> coordinates;
        while (std::getline(iss, coordinate, ',')) {
            coordinates.push_back(coordinate);
        }
        all_coordinates.push_back(coordinates);
    }

    for (int i = 0; i < all_coordinates.size(); i++) {
        insertSectorByCoordinates(std::stoi(all_coordinates[i][0]), std::stoi(all_coordinates[i][1]),
                                  std::stoi(all_coordinates[i][2]));
    }

    file.close();
}

void SpaceSectorBST::insertSectorByCoordinates(int x, int y, int z) {
    // Instantiate and insert a new sector into the space sector BST map according to the 
    // coordinates-based comparison criteria.
    recursiveInsert(root, root, x, y, z);
}

Sector *SpaceSectorBST::recursiveInsert(Sector *parent, Sector *&root, int x, int y, int z) {
    if (root == nullptr) {
        root = new Sector(x, y, z);
        if (parent) {
            root->parent = parent;
        }
        root->distance_from_earth = std::sqrt(std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2));
        string sector_code;
        sector_code += std::to_string(static_cast<int>(std::floor(root->distance_from_earth)));
        if (x == 0) {
            sector_code += "S";
        } else if (x > 0) {
            sector_code += "R";
        } else {
            sector_code += "L";
        }

        if (y == 0) {
            sector_code += "S";
        } else if (y > 0) {
            sector_code += "U";
        } else {
            sector_code += "D";
        }

        if (z == 0) {
            sector_code += "S";
        } else if (z > 0) {
            sector_code += "F";
        } else {
            sector_code += "B";
        }
        root->sector_code = sector_code;
        return root;
    }

    if (x > root->x || (x >= root->x && y > root->y) || (x >= root->x && y >= root->y && z > root->z)) {
        return recursiveInsert(root, root->right, x, y, z);
    } else {
        return recursiveInsert(root, root->left, x, y, z);
    }

}

void SpaceSectorBST::deleteSector(const std::string &sector_code) {
    // TODO: Delete the sector given by its sector_code from the BST.
    if (root != nullptr && isInIt(sector_code, root)) {
        Sector *toBeDeleted = getSector(sector_code, root);
        if (toBeDeleted->right != nullptr && toBeDeleted->left != nullptr) {
            Sector *min = findMin(toBeDeleted->right);
            toBeDeleted->sector_code = min->sector_code;
            toBeDeleted->distance_from_earth = min->distance_from_earth;
            toBeDeleted->x = min->x;
            toBeDeleted->y = min->y;
            toBeDeleted->z = min->z;

            if (min == min->parent->left) {
                min->parent->left = min->right;
            } else {
                min->parent->right = min->right;
            }

            if (min->right != nullptr) {
                min->right->parent = min->parent;
            }

            delete min;
        } else if (toBeDeleted->right == nullptr && toBeDeleted->left == nullptr) {
            if (toBeDeleted == root) {
                delete root;
                root = nullptr;
            } else {
                if (toBeDeleted->parent->left == toBeDeleted) {
                    toBeDeleted->parent->left = nullptr;
                } else if (toBeDeleted->parent->right == toBeDeleted) {
                    toBeDeleted->parent->right = nullptr;
                }
                delete toBeDeleted;
            }
        } else{
            Sector *temp_c = (toBeDeleted->left != nullptr) ? toBeDeleted->left : toBeDeleted->right;
            if (toBeDeleted == root) {
                root = temp_c;
            } else {
                if (toBeDeleted->parent->left == toBeDeleted) {
                    toBeDeleted->parent->left = temp_c;
                } else {
                    toBeDeleted->parent->right = temp_c;
                }
            }

            temp_c->parent = toBeDeleted->parent;
            delete toBeDeleted;
        }

    }
}

void SpaceSectorBST::displaySectorsInOrder() {
    // TODO: Traverse the space sector BST map in-order and print the sectors 
    // to STDOUT in the given format.
    std::cout << "Space sectors inorder traversal:" << std::endl;
    inOrderTraversal(root);
    std::cout << std::endl;
}


void SpaceSectorBST::displaySectorsPreOrder() {
    // TODO: Traverse the space sector BST map in pre-order traversal and print 
    // the sectors to STDOUT in the given format.
    std::cout << "Space sectors preorder traversal:" << std::endl;
    preOrderTraversal(root);
    std::cout << std::endl;
}

void SpaceSectorBST::displaySectorsPostOrder() {
    // TODO: Traverse the space sector BST map in post-order traversal and print 
    // the sectors to STDOUT in the given format.
    std::cout << "Space sectors postorder traversal:" << std::endl;
    postOrderTraversal(root);
    std::cout << std::endl;
}

std::vector<Sector *> SpaceSectorBST::getStellarPath(const std::string &sector_code) {
    std::vector<Sector *> path;
    if (isInIt(sector_code, root)) {
        path = getSectorsOnPath(root, path, getSector(sector_code, root));
    }
    // TODO: Find the path from the Earth to the destination sector given by its
    // sector_code, and return a vector of pointers to the Sector nodes that are on
    // the path. Make sure that there are no duplicate Sector nodes in the path!
    return path;
}

void SpaceSectorBST::printStellarPath(const std::vector<Sector *> &path) {
    // TODO: Print the stellar path obtained from the getStellarPath() function 
    // to STDOUT in the given format.
    if (path.empty()) {
        std::cout << "A path to Dr. Elara could not be found." << std::endl;
    } else {
        std::cout << "The stellar path to Dr. Elara: ";
        for (int i = 0; i < path.size(); ++i) {
            if (i == path.size() - 1) {
                std::cout << path[i]->sector_code;
            } else {
                std::cout << path[i]->sector_code << "->";
            }

        }
        std::cout << std::endl;
    }
}

void SpaceSectorBST::inOrderTraversal(Sector *root) {

    if (!root) {
        return;
    } else {
        inOrderTraversal(root->left);
        std::cout << root->sector_code << std::endl;
        inOrderTraversal(root->right);
    }

}

void SpaceSectorBST::preOrderTraversal(Sector *root) {

    if (!root) {
        return;
    } else {
        std::cout << root->sector_code << std::endl;
        preOrderTraversal(root->left);
        preOrderTraversal(root->right);
    }
}

void SpaceSectorBST::postOrderTraversal(Sector *root) {

    if (!root) {
        return;
    } else {
        postOrderTraversal(root->left);
        postOrderTraversal(root->right);
        std::cout << root->sector_code << std::endl;
    }

}

bool SpaceSectorBST::isInIt(string item, Sector *root) {
    if (!root) {
        return false;
    }
    if (item == root->sector_code) {
        return true;
    } else {
        return isInIt(item, root->left) || isInIt(item, root->right);
    }

}

Sector *SpaceSectorBST::findMin(Sector *root) {
    if (root->left == nullptr) {
        return root;
    } else {
        return findMin(root->left);
    }
}

std::vector<Sector *> SpaceSectorBST::getSectorsOnPath(Sector *root, std::vector<Sector *> &sectors, Sector *Elara) {
    if (root->sector_code == Elara->sector_code) {
        sectors.push_back(root);
        return sectors;
    }
    sectors.push_back(root);
    if (isInIt(Elara->sector_code, root->right)) {
        return getSectorsOnPath(root->right, sectors, Elara);
    } else {
        return getSectorsOnPath(root->left, sectors, Elara);
    }

}

Sector *SpaceSectorBST::getSector(std::string item, Sector *root) {
    if (root == nullptr) {
        return nullptr; // Item not found
    }
    if (root->sector_code == item) {
        return root;
    } else {
        if (isInIt(item, root->right)) {
            return getSector(item, root->right);
        } else if (isInIt(item, root->left)) {
            return getSector(item, root->left);
        }
    }
}



