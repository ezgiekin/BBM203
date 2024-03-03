
#include "ImageMatrix.h"

// Default constructor
ImageMatrix::ImageMatrix() {
    height = 0;
    width = 0;
    data = nullptr;
}

// Parameterized constructor for creating a blank image of given size
ImageMatrix::ImageMatrix(int imgHeight, int imgWidth) {
    height = imgHeight;
    width = imgWidth;
    data = new double *[height];
    for (int i = 0; i < height; ++i) {
        data[i] = new double[width]{0};
    }

}

// Parameterized constructor for loading image from file. PROVIDED FOR YOUR CONVENIENCE
ImageMatrix::ImageMatrix(const std::string &filepath) {
    // Create an ImageLoader object and load the image
    ImageLoader imageLoader(filepath);

    // Get the dimensions of the loaded image
    height = imageLoader.getHeight();
    width = imageLoader.getWidth();

    // Allocate memory for the matrix
    data = new double *[height];
    for (int i = 0; i < height; i++) {
        data[i] = new double[width];
    }

    // Copy data from imageLoader to data
    double **imageData = imageLoader.getImageData();
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            data[i][j] = imageData[i][j];
        }
    }
}


// Destructor
ImageMatrix::~ImageMatrix() {
    if (data != nullptr) {
        for (int i = 0; i < height; i++) {
            delete[] data[i];
        }
        delete[] data;
    }
    data = nullptr;

}

// Parameterized constructor - direct initialization with 2D matrix
ImageMatrix::ImageMatrix(const double **inputMatrix, int imgHeight, int imgWidth) {
    height = imgHeight;
    width = imgWidth;
    data = new double *[height];
    for (int i = 0; i < height; i++) {
        data[i] = new double[width];
        for (int j = 0; j < width; j++) {
            data[i][j] = inputMatrix[i][j];
        }
    }
}

// Copy constructor
ImageMatrix::ImageMatrix(const ImageMatrix &other) {
    height = other.height;
    width = other.width;

    data = new double *[height];
    for (int i = 0; i < height; i++) {
        data[i] = new double[width];
        for (int j = 0; j < width; j++) {
            data[i][j] = other.data[i][j];
        }
    }
}

// Copy assignment operator
ImageMatrix &ImageMatrix::operator=(const ImageMatrix &other) {
    if (this == &other) {
        return *this;
    }

    for (int i = 0; i < height; i++) {
        delete[] data[i];
    }

    delete[] data;

    height = other.height;
    width = other.width;

    data = new double *[height];
    for (int i = 0; i < height; i++) {
        data[i] = new double[width];
        for (int j = 0; j < width; j++) {
            data[i][j] = other.get_data(i, j);
        }
    }

    return *this;

}

// Overloaded operators

// Overloaded operator + to add two matrices
ImageMatrix ImageMatrix::operator+(const ImageMatrix &other) const {

    ImageMatrix resultMatrix(other.get_height(), other.get_width());

    for (int i = 0; i < other.get_height(); i++) {
        for (int j = 0; j < other.get_width(); j++) {
            resultMatrix.data[i][j] = this->data[i][j] + other.data[i][j];
        }

    }
    return resultMatrix;

}

// Overloaded operator - to subtract two matrices
ImageMatrix ImageMatrix::operator-(const ImageMatrix &other) const {
    ImageMatrix resultMatrix(other.get_height(), other.get_width());

    for (int i = 0; i < other.get_height(); i++) {
        for (int j = 0; j < other.get_width(); j++) {
            resultMatrix.data[i][j] = this->data[i][j] - other.data[i][j];

        }

    }
    return resultMatrix;
}

// Overloaded operator * to multiply a matrix with a scalar
ImageMatrix ImageMatrix::operator*(const double &scalar) const {

    ImageMatrix resultMatrix(this->get_height(), this->get_width());

    for (int i = 0; i < this->get_height(); i++) {
        for (int j = 0; j < this->get_width(); j++) {
            resultMatrix.data[i][j] = scalar * this->data[i][j];

        }

    }
    return resultMatrix;


}


// Getter function to access the data in the matrix
double **ImageMatrix::get_data() const {
    return data;
}

// Getter function to access the data at the index (i, j)
double ImageMatrix::get_data(int i, int j) const {
    if ((i >= 0 && i < height) && (j >= 0 && j < width)) {
        return data[i][j];
    } else {
        return 0.0;
    }
}

int ImageMatrix::get_height() const {
    return height;
}

int ImageMatrix::get_width() const {
    return width;
}

void ImageMatrix::set_Data(int i, int j, double value) {
    if ((i >= 0 && i < height) && (j >= 0 && j < width)) {
        data[i][j] = value;
    }
}



