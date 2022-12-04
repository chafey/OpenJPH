#include "imageT.hpp"
#include <ojph_arch.h>
#include <ojph_file.h>
#include <ojph_mem.h>
#include <ojph_params.h>
#include <ojph_codestream.h>
#include <memory>
#include <algorithm>
#include <vector>

void decode8u(ojph::codestream& codestream, Image& image)
{
    unsigned int comp_num = 0;
    size_t width = image.width();
    size_t height = image.height();
    for (size_t y = 0; y < height; y++)
    {
        size_t lineStart = y * width ;
        ojph::line_buf *line = codestream.pull(comp_num);
        unsigned char *pOut = (unsigned char *)&image.data()[lineStart];
        for (size_t x = 0; x < width; x++)
        {
            int val = line->i32[x];
            pOut[x] =(unsigned char) std::max(0, std::min(val, UCHAR_MAX));
        }
    }
}