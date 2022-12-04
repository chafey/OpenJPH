#include "helpers/imageT.hpp"
#include "helpers/imageGenerator.hpp"
#include "helpers/encodeImage.hpp"
#include "helpers/decodeImage.hpp"
#include <catch2/catch_test_macros.hpp>
#include <vector>

TEST_CASE( "CodestreamRoundTrip", "[main]" ) {
    Image sourceImage(128, 128);
    generateHorizontalRamp(sourceImage);
    std::vector<unsigned char> encodedBuffer;
    encodeImage(sourceImage, encodedBuffer);
    Image destImage(128, 128);
    decodeImage(encodedBuffer, destImage);
    REQUIRE((sourceImage == destImage));
}

