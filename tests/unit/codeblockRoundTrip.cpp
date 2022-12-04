#include "helpers/imageT.hpp"
#include "helpers/encodeCodeblock.hpp"
#include "../../src/core/common/ojph_mem.h"
#include "../../src/core/common/ojph_arch.h"
#include "../../src/core/common/ojph_params.h"
#include "../../src/core/coding/ojph_block_decoder.h"
#include "../../src/core/coding/ojph_block_encoder.h"
#include "../../src/core/codestream/ojph_params_local.h"
#include "../../src/core/codestream/ojph_codestream_local.h"
#include <catch2/catch_test_macros.hpp>
#include <vector>


void gen_rev_tx_from_cb(const ojph::ui32 *sp, void *dp, ojph::ui32 K_max,
                                       float delta, ojph::ui32 count)
{
    ojph_unused(delta);
    ojph::ui32 shift = 31 - K_max;
    //convert to sign and magnitude
    ojph::si32 *p = (ojph::si32*)dp;
    for (ojph::ui32 i = count; i > 0; --i)
    {
        ojph::ui32 v = *sp++;
        ojph::si32 val = (v & 0x7FFFFFFF) >> shift;
        *p++ = (v & 0x80000000) ? -val : val;
    }
}

TEST_CASE( "CodeBlockRoundTrip", "[main]" ) {

    Imagesi32 sp(64, 64);
    size_t index = 0;
    for(ojph::ui32 h=0; h < sp.height(); h++) {
        for(ojph::ui32 w=0; w < sp.width(); w++) {
            sp.data()[index++] = (ojph::si32)w;
        }
    }
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


    Imageui32 ddp(sp.width(), sp.height());

    ojph::local::ojph_decode_codeblock(coded_cb.next_coded->buf, ddp.data(), 
                            coded_cb.missing_msbs, coded_cb.num_passes,
                            coded_cb.pass_length[0], coded_cb.pass_length[1],
                             (ojph::ui32)sp.width(),  (ojph::ui32)sp.height(),  (ojph::ui32)sp.width(),
                            false);
    Imagesi32 dsp(sp.width(), sp.height());
    for(size_t i=0; i < sp.height(); i++) {
        gen_rev_tx_from_cb(ddp.data() + (i * dsp.width()), dsp.data() + (i * dsp.width()), coded_cb.Kmax, 0.0f, (ojph::ui32)dsp.width());
    }

    REQUIRE((sp == dsp));
}
