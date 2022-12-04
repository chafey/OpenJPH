#include "imageT.hpp"
#include <ojph_arch.h>
#include <ojph_mem.h>
#include <ojph_file.h>
#include <ojph_params.h>
#include <ojph_codestream.h>
#include <memory>
#include <vector>

void encodeImage(const Image& image, std::vector<unsigned char>& encodedBuffer)
{
    ojph::mem_outfile outfile;
    outfile.open(image.width() * image.height());
    ojph::codestream codestream;

    ojph::param_siz siz = codestream.access_siz();
    siz.set_image_extent(ojph::point((ojph::ui32)image.width(), (ojph::ui32)image.height()));
    siz.set_num_components(1);
    siz.set_component(0, ojph::point(1, 1), 8, false);
    //siz.set_image_offset(ojph::point(0,0));
    //siz.set_tile_size(image.dimensions());
    //siz.set_tile_offset(ojph::point(0,0));

    ojph::param_cod cod = codestream.access_cod();
    //cod.set_num_decomposition(5);
    //cod.set_block_dims(64, 64);
    //cod.set_precinct_size(0, NULL);
    //cod.set_progression_order("RPCL");
    //cod.set_color_transform(false);
    cod.set_reversible(true);
    //codestream.set_planar(true);
    codestream.write_headers(&outfile);
    
    // Encode the image
    ojph::ui32 next_comp;
    ojph::line_buf *cur_line = codestream.exchange(NULL, next_comp);
    siz = codestream.access_siz();
    for (size_t y = 0; y < siz.get_image_extent().y; y++)
    {
      for (size_t c = 0; c < siz.get_num_components(); c++)
      {
        int *dp = cur_line->i32;
        uint8_t *pIn = (uint8_t *)(image.data() + (y * siz.get_image_extent().x * siz.get_num_components()) + c);
        for (size_t x = 0; x < siz.get_image_extent().x; x++)
        {
            *dp++ = *pIn;
            pIn += siz.get_num_components();
        }
        cur_line = codestream.exchange(cur_line, next_comp);
      }
    }
    codestream.flush();

    encodedBuffer.resize((size_t)outfile.tell());
    memcpy(encodedBuffer.data(), (unsigned char*)outfile.get_data(), (size_t)outfile.tell());

    codestream.close();
}