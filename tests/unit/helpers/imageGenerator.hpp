
#include "imageT.hpp"

void generateHorizontalRamp(Image8u& image) {
    unsigned char* pOut = image.data();
    for(size_t y=0; y < image.height(); y++) {
        for(size_t x=0; x < image.width(); x++) {
            *pOut = (unsigned char)(x % 256);
            pOut++;
        } 
    }
}