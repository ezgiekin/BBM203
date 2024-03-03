#include "EdgeDetector.h"
#include <cmath>
#include "Convolution.h"


// Default constructor
EdgeDetector::EdgeDetector() {

}

// Destructor
EdgeDetector::~EdgeDetector() {

}

// Detect Edges using the given algorithm
std::vector<std::pair<int, int>> EdgeDetector::detectEdges(const ImageMatrix &input_image) {

    double *gx[3] = {
            new double[3]{-1, 0, 1},
            new double[3]{-2, 0, 2},
            new double[3]{-1, 0, 1}
    };

    Convolution horizontal(gx, 3, 3, 1, true);
    ImageMatrix horizontalMatrix(horizontal.convolve(input_image));

    double *gy[3] = {
            new double[3]{-1, -2, -1},
            new double[3]{0, 0, 0},
            new double[3]{1, 2, 1}
    };

    Convolution vertical(gy, 3, 3, 1, true);
    ImageMatrix verticalMatrix(vertical.convolve(input_image));

    ImageMatrix gradientMatrix(input_image.get_height(), input_image.get_width());

    for (int i = 0; i < input_image.get_height(); i++) {
        for (int j = 0; j < input_image.get_width(); j++) {
            double horizontal1 = horizontalMatrix.get_data(i, j);
            double vertical2 = verticalMatrix.get_data(i, j);
            gradientMatrix.set_Data(i, j, sqrt(pow(horizontal1, 2) +
                                               pow(vertical2, 2)));
            
        }
    }

    double sum = 0;
    for (int i = 0; i < input_image.get_height(); i++) {
        for (int j = 0; j < input_image.get_width(); j++) {
            sum += gradientMatrix.get_data(i, j);
        }

    }

    double average = sum / static_cast<int>((gradientMatrix.get_height() * gradientMatrix.get_width()));

    std::vector<std::pair<int, int>> edgePixels;

    for (int i = 0; i < static_cast<int>(gradientMatrix.get_height()); i++) {
        for (int j = 0; j < static_cast<int>(gradientMatrix.get_width()); j++) {
            if (average < gradientMatrix.get_data(i, j)) {
                edgePixels.push_back(std::make_pair(i, j));
            }
        }
    }

    for (int i = 0; i < 3; i++) {
        delete[] gx[i];
        delete[] gy[i];
    }

    return edgePixels;

}

