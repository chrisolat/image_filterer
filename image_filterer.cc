#include <iostream>
#include <string>
#include <vector>
#include <cmath>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <math.h>

class Filterer{
    
public:
    Filterer(){}
    Filterer(int h, int w, int c){
        imgHeight = h;
        imgWidth = w;
        imgChannels = c;
    }
    void set(){}
    void apply_box_filter(){}

    
    void apply_threshold(unsigned char* imgData, float threshold=0.5f){
        if(!imgHeight || !imgWidth || !imgChannels){
            return;
        }
        // unsigned char* blankImage = new unsigned char[imgHeight * imgWidth * imgChannels];
        // for (int i = 0; i < imgHeight; i++){
        //     for (int j = 0; j < imgWidth; j++){
        //         set(i,j,)
        //     }
        // }
    }
    void AverageFilter(unsigned char* imgData, std::string filename="Avg_Filter.jpg"){
        if(!imgHeight || !imgWidth || !imgChannels){
            std::cout << "Provide imgae dimensions" << std::endl;
            return;
        }
        std::vector<unsigned char> blurred_image = applyAverageFilter(imgData);
        // for(auto i : blurred_image){std::cout << i << " ";}
        std::cout << std::endl;
        stbi_write_jpg(filename.c_str(), imgWidth, imgHeight, imgChannels, blurred_image.data(), 100);
    }
    void GaussianBlur(unsigned char* imgData, float sigma, std::string filename="G_Blur.jpg"){
        if(!imgHeight || !imgWidth || !imgChannels){
            std::cout << "Provide imgae dimensions" << std::endl;
            return;
        }
        std::vector<unsigned char> blurred_image = applyGaussianBlur(imgData, sigma);
        // for(auto i : blurred_image){std::cout << i << " ";}
        std::cout << std::endl;
        stbi_write_jpg(filename.c_str(), imgWidth, imgHeight, imgChannels, blurred_image.data(), 100);
    }
    void GreyScale(unsigned char* imgData, std::string filename="G_Scale.jpg"){
        if(!imgHeight || !imgWidth || !imgChannels){
            std::cout << "Provide imgae dimensions" << std::endl;
            return;
        }
        std::vector<unsigned char> grey_scaled_image = applyGreyScale(imgData);
        stbi_write_jpg(filename.c_str(), imgWidth, imgHeight, imgChannels, grey_scaled_image.data(), 100);
    }
    void create_blank_image(int height = 3, int width = 3, int numChannels = 3, std::string filename="blank_image.jpg"){

        unsigned char* blankImage = new unsigned char[height * width * numChannels];
        unsigned char backgroudColor[] = {255,255,255};

        for (int i = 0; i < height; i++){
            for (int j = 0; j < width; j++){
                int index = (i * width + j) * numChannels;
                for(int c = 0; c < numChannels; c++){
                    blankImage[index + c] = backgroudColor[c];
                }
            }
        }

        stbi_write_jpg(filename.c_str(), width, height, numChannels, blankImage, 100);

    }
    void sharpen(){}
private:

    int imgHeight=0, imgWidth=0, imgChannels=0;

    std::vector<unsigned char> applyGaussianBlur(unsigned char* imageData, float sigma) {
        // Define Gaussian kernel size and compute radius
        int kernelSize = 2 * std::ceil(2 * sigma) + 1;
        int radius = kernelSize / 2;

        // Define Gaussian kernel
        std::vector<float> kernel(kernelSize * kernelSize);
        float sum = 0.0f;
        for (int y = -radius; y <= radius; ++y) {
            for (int x = -radius; x <= radius; ++x) {
                float value = std::exp(-(x * x + y * y) / (2 * sigma * sigma)) / (2 * M_PI * sigma * sigma);
                kernel[(y + radius) * kernelSize + (x + radius)] = value;
                sum += value;
            }
        }
        // Normalize the kernel
        for (int i = 0; i < kernelSize * kernelSize; ++i) {
            kernel[i] /= sum;
        }

        // Apply the Gaussian blur
        std::vector<unsigned char> blurredImage(imgWidth * imgHeight * imgChannels);
        for (int y = 0; y < imgHeight; ++y) {
            for (int x = 0; x < imgWidth; ++x) {
                for (int c = 0; c < imgChannels; ++c) {
                    float blurredPixel = 0.0f;
                    for (int ky = -radius; ky <= radius; ++ky) {
                        for (int kx = -radius; kx <= radius; ++kx) {
                            int px = std::min(std::max(x + kx, 0), imgWidth - 1);
                            int py = std::min(std::max(y + ky, 0), imgHeight - 1);
                            blurredPixel += imageData[(py * imgWidth + px) * imgChannels + c] * kernel[(ky + radius) * kernelSize + (kx + radius)];
                        }
                    }
                    // TODO - remove
                    if(blurredPixel > 200.f){
                        blurredPixel = 1.0f;
                    }
                    blurredImage[(y * imgWidth + x) * imgChannels + c] = static_cast<unsigned char>(blurredPixel);
                }
            }
        }

        // Copy blurred image back to original image data
        // std::copy(blurredImage.begin(), blurredImage.end(), imageData);
        return blurredImage;
    }

    std::vector<unsigned char> applyGreyScale(unsigned char* imgData){
        std::vector<unsigned char> blurredImage(imgWidth * imgHeight * imgChannels);
        for (int y = 0; y < imgHeight; ++y) {
            for (int x = 0; x < imgWidth; ++x) {
                int index = (y * imgWidth + x) * imgChannels;
                unsigned char r = imgData[index];
                unsigned char g = imgData[index + 1];
                unsigned char b = imgData[index + 2];
                unsigned char grayscale = static_cast<unsigned char>(0.2126 * r + 0.7152 * g + 0.0722 * b);
                // unsigned char grayscale = static_cast<unsigned char>(r*0.22 + g*0.72 + b*0.72);

                // Set the same grayscale value for all RGB channels
                blurredImage[index] = grayscale;
                blurredImage[index + 1] = grayscale;
                blurredImage[index + 2] = grayscale;
                // std::cout << grayscale << std::endl;
            }
        }
        return blurredImage;
    }

    // same as greyscale
    std::vector<unsigned char> applyAverageFilter(unsigned char* imgData){
        std::vector<unsigned char> outImage(imgWidth * imgHeight * imgChannels);
        for (int y = 0; y < imgHeight; ++y) {
            for (int x = 0; x < imgWidth; ++x) {
                int index = (y * imgWidth + x) * imgChannels;
                unsigned char r = imgData[index];
                unsigned char g = imgData[index + 1];
                unsigned char b = imgData[index + 2];
                unsigned char pixel = (0.2126 *r+0.7152 *g+0.0722 *b/3); 

                // Set the same grayscale value for all RGB channels
                outImage[index] = pixel;
                outImage[index + 1] = pixel;
                outImage[index + 2] = pixel;
                // std::cout << grayscale << std::endl;
            }
        }
        return outImage;
    }
};

int main(int argc, char** argv){
    std::string filename;
    if(argc < 2){
        std::cout << "filename?" << std::endl;
        return 0;
    }
    filename = argv[1];
    if (filename.empty()){
        std::cout << "filename? " <<std::endl;
        return 0;
    }
    int width, height, numChannels;
    unsigned char* imageData = stbi_load(filename.c_str(), &width, &height, &numChannels, 0);
    if (!imageData){
        std::cout << "Could not load Image" << std::endl;
        return 0;
    }
    std::cout << "Width: " << width << "px, Height: " << height << "px, Channels: " << numChannels << std::endl;

    Filterer filterer(height, width, numChannels);
    // filterer.GaussianBlur(imageData,5.0f);
    // filterer.GreyScale(imageData);
    filterer.AverageFilter(imageData);



    stbi_image_free(imageData);
    return 0;
}