
#include "Convolution.h"

// Default constructor
Convolution::Convolution() {
    stride = 0;
    padding = false;
    kernelHeight = 0;
    kernelWidth = 0;
    kernel = nullptr;
}

// Parametrized constructor for custom kernel and other parameters
Convolution::Convolution(double **customKernel, int kh, int kw, int stride_val, bool pad) {

    if (customKernel != nullptr && kh > 0 && kw > 0) {
        stride = stride_val;
        padding = pad;
        kernelHeight = kh;
        kernelWidth = kw;

        kernel = new double *[kernelHeight];
        for (int i = 0; i < kernelHeight; i++) {
            kernel[i] = new double[kernelWidth];
            for (int j = 0; j < kernelWidth; j++) {
                kernel[i][j] = customKernel[i][j];
            }
        }
    }


}

// Destructor
Convolution::~Convolution() {
    if (kernel != nullptr) {
        for (int i = 0; i < kernelHeight; i++) {
            delete[] kernel[i];
        }
        delete[] kernel;
    }
    kernel = nullptr;
}

// Copy constructor
Convolution::Convolution(const Convolution &other) {

    kernelHeight = other.kernelHeight;
    kernelWidth = other.kernelWidth;
    stride = other.stride;
    padding = other.padding;


    if (other.kernel != nullptr && other.kernelHeight > 0 && other.kernelWidth > 0) {
        kernel = new double *[kernelHeight];
        for (int i = 0; i < kernelHeight; i++) {
            kernel[i] = new double[kernelWidth];
            for (int j = 0; j < kernelWidth; j++) {
                kernel[i][j] = other.kernel[i][j];
            }
        }
    } else {
        kernel = nullptr;
    }

}

// Copy assignment operator
Convolution &Convolution::operator=(const Convolution &other) {
    if (this == &other) {
        return *this;
    }

    kernelHeight = other.kernelHeight;
    kernelWidth = other.kernelWidth;
    stride = other.stride;
    padding = other.padding;

    if (other.kernel != nullptr && other.kernelHeight > 0 && other.kernelWidth > 0) {
        // Perform shallow copy if the other object has a valid kernel
        if (kernel != nullptr) {
            for (int i = 0; i < kernelHeight; i++) {
                delete[] kernel[i];
            }
            delete[] kernel;
        }

        kernel = new double *[kernelHeight];
        for (int i = 0; i < kernelHeight; i++) {
            kernel[i] = new double[kernelWidth];
            for (int j = 0; j < kernelWidth; j++) {
                kernel[i][j] = other.kernel[i][j];
            }
        }
    } else {
        // If the other object's kernel is nullptr, release the existing memory and set kernel to nullptr
        if (other.kernel == nullptr) {
            for (int i = 0; i < kernelHeight; i++) {
                delete[] kernel[i];
            }
            delete[] kernel;
            kernel = nullptr;
        }
    }

    return *this;


}


// Convolve Function: Responsible for convolving the input image with a kernel and return the convolved image.
ImageMatrix Convolution::convolve(const ImageMatrix &input_image) const {

    if (kernel != nullptr && kernelHeight > 0 && kernelWidth > 0) {

        int originalHeight = input_image.get_height();
        int originalWidth = input_image.get_width();

        int outputHeight = (originalHeight - kernelHeight + 2 * padding) / stride + 1;
        int outputWidth = (originalWidth - kernelWidth + 2 * padding) / stride + 1;

        int paddedHeight = originalHeight;
        int paddedWidth = originalWidth;

        if (padding) {
            paddedHeight = originalHeight + 2;
            paddedWidth = originalWidth + 2;
        }

        if (paddedHeight < kernelHeight || paddedWidth < kernelWidth) {
            return {0, 0};
        }

        ImageMatrix paddedImageMatrix(paddedHeight, paddedWidth);

        if (padding) {
            for (int i = 0; i < originalHeight; i++) {
                for (int j = 0; j < originalWidth; j++) {
                    paddedImageMatrix.set_Data(i + (paddedHeight - originalHeight) / 2,
                                               j + (paddedWidth - originalWidth) / 2, input_image.get_data(i, j));
                }
            }
        } else {
            paddedImageMatrix = input_image;
        }


        ImageMatrix convolvedMatrix(outputHeight, outputWidth);

        for (int i = 0; i < convolvedMatrix.get_height(); i++) {
            for (int j = 0; j < convolvedMatrix.get_width(); j++) {
                double result = 0;
                for (int k = 0; k < kernelHeight; k++) {
                    for (int n = 0; n < kernelWidth; n++) {
                        result += paddedImageMatrix.get_data(i * stride + k, j * stride + n) * kernel[k][n];
                    }
                }
                convolvedMatrix.set_Data(i, j, result);
            }

        }
        return convolvedMatrix;
    } else if (stride <= 0) {
        return input_image;
    } else {
        return ImageMatrix{0, 0};
    }


}







