#include "DecodeMessage.h"
#include <cmath>


// Default constructor
DecodeMessage::DecodeMessage() {
    // Nothing specific to initialize here
}

// Destructor
DecodeMessage::~DecodeMessage() {
    // Nothing specific to clean up
}


std::string
DecodeMessage::decodeFromImage(const ImageMatrix &image, const std::vector<std::pair<int, int>> &edgePixels) {


    std::string binaryString;
    for (int i = 0; i < static_cast<int>(edgePixels.size()); i++) {
        int firstIndex = edgePixels.at(i).first;
        int secondIndex = edgePixels.at(i).second;
        binaryString += std::to_string(static_cast<int>(image.get_data(firstIndex, secondIndex)) & 1);
    }

    std::string paddedBinaryString;
    if (binaryString.length() % 7 != 0) {
        int leadingZeros = 7 - (binaryString.length() % 7);
        paddedBinaryString = std::string(leadingZeros, '0') + binaryString;
    } else {
        paddedBinaryString = binaryString;
    }

    std::string asciiString;
    for (int i = 0; i < static_cast<int>(paddedBinaryString.length()); i += 7) {
        int result = 0;
        for (int j = 6; j >= 0; j--) {
            if (paddedBinaryString.substr(i, 7)[j] == '1') {
                result += pow(2, 6 - j);
            }
        }
        if (result <= 32) {
            result += 33;
        } else if (result >= 127) {
            result = 126;
        }
        asciiString += static_cast<char>(result);
    }

    return asciiString;


}



