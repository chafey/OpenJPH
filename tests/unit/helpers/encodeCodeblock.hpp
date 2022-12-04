#include "imageT.hpp"
#include "../../../src/core/common/ojph_mem.h"
#include "../../../src/core/common/ojph_arch.h"
#include "../../../src/core/common/ojph_params.h"
#include "../../../src/core/coding/ojph_block_decoder.h"
#include "../../../src/core/coding/ojph_block_encoder.h"
#include "../../../src/core/codestream/ojph_params_local.h"
#include "../../../src/core/codestream/ojph_codestream_local.h"
#include <vector>

void gen_rev_tx_to_cb(const void *sp, ojph::ui32 *dp, ojph::ui32 K_max, 
                                    float delta_inv, ojph::ui32 count, 
                                    ojph::ui32* max_val)
{
    ojph_unused(delta_inv);
    ojph::ui32 shift = 31 - K_max;
    // convert to sign and magnitude and keep max_val
    ojph::ui32 tmax = *max_val;
    ojph::si32 *p = (ojph::si32*)sp;
    for (ojph::ui32 i = count; i > 0; --i)
    {
        ojph::si32 v = *p++;
        ojph::ui32 sign = v >= 0 ? 0 : 0x80000000;
        ojph::ui32 val = (ojph::ui32)(v >= 0 ? v : -v);
        val <<= shift;
        *dp++ = sign | val;
        tmax |= val; // it is more efficient to use or than max
    }
    *max_val = tmax;
}

void encodeCodeblock(Imagesi32& sp, std::vector<unsigned char>& encoded) {

    Imageui32 dp(sp.width(), sp.height());
    ojph::mem_elastic_allocator *elastic = new ojph::mem_elastic_allocator(1048576);
    ojph::local::coded_cb_header coded_cb;
    coded_cb.Kmax = 10;
    coded_cb.missing_msbs = coded_cb.Kmax - 1;
    coded_cb.num_passes = 1;
    coded_cb.pass_length[0] = 0;
    coded_cb.pass_length[1] = 0;
    coded_cb.next_coded = 0;
    ojph::ui32 max_val[8];
    for(size_t i=0; i < sp.height(); i++) {
        gen_rev_tx_to_cb(sp.data() + (i * sp.width()), dp.data() + (i * sp.width()), coded_cb.Kmax, 0.0f, (ojph::ui32)sp.width(), max_val);
    }

    ojph::local::ojph_encode_codeblock(dp.data(), coded_cb.missing_msbs, coded_cb.num_passes,
                         (ojph::ui32)sp.width(),  (ojph::ui32)sp.height(), (ojph::ui32)sp.width(),
                        coded_cb.pass_length, 
                        elastic,
                        coded_cb.next_coded);

    encoded.resize(coded_cb.next_coded->buf);
    
}