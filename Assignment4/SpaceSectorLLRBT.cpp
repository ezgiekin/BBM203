#include "SpaceSectorLLRBT.h"
#include <cmath>

using namespace std;

SpaceSectorLLRBT::SpaceSectorLLRBT() : root(nullptr) {}

void SpaceSectorLLRBT::readSectorsFromFile(const std::string &filename) {
    // TODO: read the sectors from the input file and insert them into the LLRBT sector map
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

// Remember to handle memory deallocation properly in the destructor.
SpaceSectorLLRBT::~SpaceSectorLLRBT() {
    deleteLLRBT(root);
    root = nullptr;
    // TODO: Free any dynamically allocated memory in this class.
}

void SpaceSectorLLRBT::deleteLLRBT(Sector *root) {
    if (root == nullptr) {
        return;
    } else {
        deleteLLRBT(root->left);
        deleteLLRBT(root->right);
        delete root;
    }
}

void SpaceSectorLLRBT::insertSectorByCoordinates(int x, int y, int z) {
    // TODO: Instantiate and insert a new sector into the space sector LLRBT map 
    // according to the coordinates-based comparison criteria.
    recursiveInsert(root, root, x, y, z);
    root->color = false;
    root->parent = nullptr;


}

Sector *SpaceSectorLLRBT::recursiveInsert(Sector *parent, Sector *&root, int x, int y, int z) {
    if (!root) {
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
        root->right = recursiveInsert(root, root->right, x, y, z);
    } else {
        root->left = recursiveInsert(root, root->left, x, y, z);
    }

    if (isRed(root->right) && !isRed(root->left)) {
        root = rotateLeft(root);
    }
    if (isRed(root->left) && isRed(root->left->left)) {
        root = rotateRight(root);
    }
    if (isRed(root->left) && isRed(root->right)) {
        flipColors(root);
    }

    return root;


}

bool SpaceSectorLLRBT::isRed(Sector *node) {
    if (!node) {
        return false;
    }
    return node->color;
}

Sector *SpaceSectorLLRBT::rotateLeft(Sector *node) {
    Sector *temp = node->right;
    node->right = temp->left;
    if (temp->left != nullptr) {
        temp->left->parent = node;
    }
    temp->left = node;
    temp->color = node->color;
    node->color = true;
    temp->parent = node->parent;
    node->parent = temp;
    return temp;
}

Sector *SpaceSectorLLRBT::rotateRight(Sector *node) {
    Sector *temp = node->left;
    node->left = temp->right;
    if (temp->right != nullptr) {
        temp->right->parent = node;
    }
    temp->right = node;
    temp->color = node->color;
    node->color = true;
    temp->parent = node->parent;
    node->parent = temp;
    return temp;
}

void SpaceSectorLLRBT::flipColors(Sector *node) {
    node->color = !node->color;
    node->left->color = !node->left->color;
    node->right->color = !node->right->color;
}

void SpaceSectorLLRBT::displaySectorsInOrder() {
    // TODO: Traverse the space sector LLRBT map in-order and print the sectors 
    // to STDOUT in the given format.
    std::cout << "Space sectors inorder traversal:" << std::endl;
    inOrderTraversal(root);
    std::cout << std::endl;
}

void SpaceSectorLLRBT::displaySectorsPreOrder() {
    // TODO: Traverse the space sector LLRBT map in pre-order traversal and print 
    // the sectors to STDOUT in the given format.
    std::cout << "Space sectors preorder traversal:" << std::endl;
    preOrderTraversal(root);
    std::cout << std::endl;
}

void SpaceSectorLLRBT::displaySectorsPostOrder() {
    // TODO: Traverse the space sector LLRBT map in post-order traversal and print 
    // the sectors to STDOUT in the given format.
    std::cout << "Space sectors postorder traversal:" << std::endl;
    postOrderTraversal(root);
    std::cout << std::endl;
}

void SpaceSectorLLRBT::inOrderTraversal(Sector *root) {

    if (!root) {
        return;
    } else {
        inOrderTraversal(root->left);
        std::cout << (root->color ? "RED" : "BLACK") << " sector: " << root->sector_code << std::endl;
        inOrderTraversal(root->right);
    }

}

void SpaceSectorLLRBT::preOrderTraversal(Sector *root) {
    if (!root) {
        return;
    } else {
        std::cout << (root->color ? "RED" : "BLACK") << " sector: " << root->sector_code << std::endl;
        preOrderTraversal(root->left);
        preOrderTraversal(root->right);
    }
}

void SpaceSectorLLRBT::postOrderTraversal(Sector *root) {

    if (!root) {
        return;
    } else {
        postOrderTraversal(root->left);
        postOrderTraversal(root->right);
        std::cout << (root->color ? "RED" : "BLACK") << " sector: " << root->sector_code << std::endl;
    }
}

std::vector<Sector *> SpaceSectorLLRBT::getStellarPath(const std::string &sector_code) {
    std::vector<Sector *> path;
    // TODO: Find the path from the Earth to the destination sector given by its
    // sector_code, and return a vector of pointers to the Sector nodes that are on
    // the path. Make sure that there are no duplicate Sector nodes in the path!
    if (isInIt(sector_code, root)) {
        path = getSectorsOnPath(getSector("0SSS", root), path, getSector(sector_code, root), getSector("0SSS", root));
    }
    return path;
}

std::vector<Sector *>
SpaceSectorLLRBT::getSectorsOnPath(Sector *root, std::vector<Sector *> &sectors, Sector *Elara, Sector *Earth) {
    if (root == nullptr) {
        return sectors;
    }
    if (root->sector_code == Elara->sector_code) {
        sectors.push_back(root);
        return sectors;
    }
    sectors.push_back(root);
    if (Earth->parent != nullptr) {
        if (isInIt(Elara->sector_code, root->right)) {
            return getSectorsOnPath(root->right, sectors, Elara, Earth);
        } else if (isInIt(Elara->sector_code, root->left)) {
            return getSectorsOnPath(root->left, sectors, Elara, Earth);
        } else {
            return getSectorsOnPath(root->parent, sectors, Elara, Earth);
        }
    } else {
        if (isInIt(Elara->sector_code, root->right)) {
            return getSectorsOnPath(root->right, sectors, Elara, Earth);
        } else if (isInIt(Elara->sector_code, root->left)) {
            return getSectorsOnPath(root->left, sectors, Elara, Earth);
        }
    }
}

Sector *SpaceSectorLLRBT::getSector(std::string item, Sector *root) {
    if (root == nullptr) {
        return nullptr;
    }
    if (root->sector_code == item) {
        return root;
    } else {
        if (isInIt(item, root->right)) {
            return getSector(item, root->right);
        } else {
            return getSector(item, root->left);
        }
    }
}

bool SpaceSectorLLRBT::isInIt(string item, Sector *root) {
    if (root == nullptr) {
        return false;
    }
    if (item == root->sector_code) {
        return true;
    } else {
        return isInIt(item, root->left) || isInIt(item, root->right);
    }
}

void SpaceSectorLLRBT::printStellarPath(const std::vector<Sector *> &path) {
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