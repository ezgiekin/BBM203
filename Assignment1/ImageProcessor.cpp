#include "ImageProcessor.h"
#include "EdgeDetector.h"



ImageProcessor::ImageProcessor() {

}

ImageProcessor::~ImageProcessor() {

}


std::string ImageProcessor::decodeHiddenMessage(const ImageMatrix &img) {

    if (img.get_data() == nullptr) {
        return "";
    }

    ImageSharpening imageSharpener;
    ImageMatrix sharpenedImage = imageSharpener.sharpen(img, 2.0);

    EdgeDetector edgeDetector;
    std::vector<std::pair<int, int>> edgePixels = edgeDetector.detectEdges(sharpenedImage);

    DecodeMessage messageDecoder;
    std::string message = messageDecoder.decodeFromImage(sharpenedImage, edgePixels);

    return message;

}

ImageMatrix ImageProcessor::encodeHiddenMessage(const ImageMatrix &img, const std::string &message) {

    ImageSharpening imageSharpener;
    ImageMatrix sharpenedImage = imageSharpener.sharpen(img, 2.0);

    EdgeDetector edgeDetector;
    std::vector<std::pair<int, int>> edgePixels = edgeDetector.detectEdges(sharpenedImage);

    EncodeMessage messageEncoder;

    if (!edgePixels.empty() && !message.empty() && img.get_data() != nullptr) {
        ImageMatrix encodedMessage = messageEncoder.encodeMessageToImage(img, message, edgePixels);
        return encodedMessage;
    }
    return img;


}
