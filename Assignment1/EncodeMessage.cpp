#include "EncodeMessage.h"


// Default Constructor
EncodeMessage::EncodeMessage() {

}

// Destructor
EncodeMessage::~EncodeMessage() {

}

// Function to encode a message into an image matrix
ImageMatrix EncodeMessage::encodeMessageToImage(const ImageMatrix &img, const std::string &message,
                                                const std::vector<std::pair<int, int>> &positions) {


    if (!message.empty() && !positions.empty() && img.get_data() != nullptr) {

        ImageMatrix encodedImg(img);

        std::string transformatedString;

        for (int i = 0; i < static_cast<int>(message.length()); i++) {
            int decimalResult = 0;
            if (isPrime(i)) {
                decimalResult = static_cast<int>(message.at(i)) + findFibonacci(i);
                if (decimalResult >= 127) {
                    decimalResult = 126;
                } else if (decimalResult <= 32) {
                    decimalResult += 33;
                }
                transformatedString += static_cast<char>(decimalResult);
            } else {
                transformatedString += message.at(i);
            }

        }

        std::string shiftedString = rightCircularShifting(transformatedString);

        std::string binaryEncoding;
        for (int i = 0; i < static_cast<int>(shiftedString.length()); i++) {
            binaryEncoding += decimalToBinary(static_cast<int>(shiftedString.at(i)));
        }


        for (int i = 0; i < static_cast<int>(binaryEncoding.length()); i++) {
            if (i >= static_cast<int>(positions.size())) {
                break;
            }
            int value1 = encodedImg.get_data(positions.at(i).first, positions.at(i).second);
            if (binaryEncoding.at(i) == '0') {
                value1 &= ~1;
            } else {
                value1 |= 1;
            }

            encodedImg.set_Data(positions.at(i).first, positions.at(i).second, value1);
        }
        return encodedImg;
    } else {
        return img;
    }


}

bool EncodeMessage::isPrime(int a) {

    if (a <= 1) {
        return false;
    }

    for (int i = 2; i < a; i++) {
        if (a % i == 0) {
            return false;
        }

    }

    return true;


}

int EncodeMessage::findFibonacci(int n) {
    if (n <= 1) {
        return n;
    }
    return findFibonacci(n - 1) + findFibonacci(n - 2);

}

std::string EncodeMessage::rightCircularShifting(const std::string &string) {
    std::string string2 = string;
    if (string2.length() <= 1) {
        return string2;
    }
    int length = string2.length() / 2;
    std::string splitString = string.substr(string2.length() - length, length);
    std::string circuledString = splitString + string2.substr(0, string2.length() - length);

    return circuledString;
}

std::string EncodeMessage::decimalToBinary(int a) {
    int num = a;
    std::string binaryString;
    while (num != 0) {
        int rem = num % 2;
        num = num / 2;
        binaryString = std::to_string(rem) + binaryString;
    }
    std::string paddedBinaryString;
    if (binaryString.length() % 7 != 0) {
        int leadingZeros = 7 - (binaryString.length() % 7);
        paddedBinaryString = std::string(leadingZeros, '0') + binaryString;
    } else {
        paddedBinaryString = binaryString;
    }
    return paddedBinaryString;
}

