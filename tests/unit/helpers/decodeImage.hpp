#include "imageT.hpp"
#include "decode8u.hpp"
#include <ojph_arch.h>
#include <ojph_file.h>
#include <ojph_mem.h>
#include <ojph_params.h>
#include <ojph_codestream.h>
#include <memory>
#include <algorithm>
#include <vector>

void decodeImage(const std::vector<unsigned char>& encodedBuffer, Image& image)
{
    ojph::mem_infile infile;
    infile.open(encodedBuffer.data(), encodedBuffer.size());

    ojph::codestream codestream;
    codestream.read_headers(&infile);
    //ojph::param_siz siz = codestream.access_siz();
    //size_t width = siz.get_image_extent().x - siz.get_image_offset().x;
    //size_t height = siz.get_image_extent().y - siz.get_image_offset().y;
    //size_t componentCount = siz.get_num_components();
    //size_t bitsPerSample = siz.get_bit_depth(0);
    //size_t isSigned = siz.is_signed(0);
    
    // decode
    codestream.set_planar(true);
    codestream.create();

    decode8u(codestream, image);

}