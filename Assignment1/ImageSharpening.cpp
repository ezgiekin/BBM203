#include "ImageSharpening.h"

// Default constructor
ImageSharpening::ImageSharpening() {

}

ImageSharpening::~ImageSharpening() {

}

ImageMatrix ImageSharpening::sharpen(const ImageMatrix &input_image, double k) {

    if (input_image.get_data() != nullptr && k > 0) {
        double *kernel[3] = {
                new double[3]{1.0 / 9, 1.0 / 9, 1.0 / 9},
                new double[3]{1.0 / 9, 1.0 / 9, 1.0 / 9},
                new double[3]{1.0 / 9, 1.0 / 9, 1.0 / 9}
        };

        int kernelHeight = 3;
        int kernelWidth = 3;
        int stride = 1;

        Convolution convolution(kernel,
                                kernelHeight, kernelWidth, stride, true);

        ImageMatrix convolvedMatrix = convolution.convolve(input_image);

        ImageMatrix matrix = input_image - convolvedMatrix;

        ImageMatrix matrix1 = matrix * k;

        ImageMatrix sharpenedmatrix(input_image + matrix1);

        for (int i = 0; i < sharpenedmatrix.get_height(); i++) {
            for (int j = 0; j < sharpenedmatrix.get_width(); j++) {
                if (sharpenedmatrix.get_data(i, j) < 0.0) {
                    sharpenedmatrix.set_Data(i, j, 0.0);
                } else if (sharpenedmatrix.get_data(i, j) > 255.0) {
                    sharpenedmatrix.set_Data(i, j, 255.0);
                }
            }
        }

        for (int i = 0; i < 3; i++) {
            delete[] kernel[i];
        }

        return sharpenedmatrix;
    } else {
        return input_image;
    }


}
