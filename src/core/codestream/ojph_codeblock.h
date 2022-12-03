
//***************************************************************************/
// This software is released under the 2-Clause BSD license, included
// below.
//
// Copyright (c) 2019, Aous Naman 
// Copyright (c) 2019, Kakadu Software Pty Ltd, Australia
// Copyright (c) 2019, The University of New South Wales, Australia
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
// IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//***************************************************************************/
// This file is part of the OpenJPH software implementation.
// File: ojph_codeblock.h
// Author: Aous Naman
// Date: 28 August 2019
//***************************************************************************/


#ifndef OJPH_CODEBLOCK_H
#define OJPH_CODEBLOCK_H

#include "ojph_defs.h"
#include "ojph_file.h"
#include "ojph_params_local.h"

namespace ojph {

  ////////////////////////////////////////////////////////////////////////////
  //defined elsewhere
  struct line_buf;
  class mem_fixed_allocator;
  class mem_elastic_allocator;
  class codestream;
  struct coded_lists;

  namespace local {

    //////////////////////////////////////////////////////////////////////////
    //defined here
    class tile;
    class tile_comp;
    class resolution;
    struct precinct;
    class subband;
    class codeblock;
    struct coded_cb_header;

    //////////////////////////////////////////////////////////////////////////
    class codeblock
    {
      friend struct precinct;
    public:
      static void pre_alloc(codestream *codestream, const size& nominal);
      void finalize_alloc(codestream *codestream, subband* parent,
                          const size& nominal, const size& cb_size,
                          coded_cb_header* coded_cb,
                          ui32 K_max, int tbx0);
      void push(line_buf *line);
      void encode(mem_elastic_allocator *elastic);
      void recreate(const size& cb_size, coded_cb_header* coded_cb);

      void decode();
      void pull_line(line_buf *line);

    private:
      ui32* buf;
      size nominal_size;
      size cb_size;
      ui32 stride;
      ui32 buf_size;
      subband* parent;
      int line_offset;
      ui32 cur_line;
      float delta, delta_inv;
      ui32 K_max;
      bool reversible;
      bool resilient;
      bool stripe_causal;
      bool zero_block; // true when the decoded block is all zero
      ui32 max_val[8]; // supports up to 256 bits
      coded_cb_header* coded_cb;

    private:
      // define function signature simple memory clearing
      typedef void (*mem_clear_fun)(void* addr, size_t count);
      // a pointer to the max value finding function
      mem_clear_fun mem_clear;
      static void gen_mem_clear(void* addr, size_t count);

      // define function signature for max value finding
      typedef ui32 (*find_max_val_fun)(ui32* addr);
      // a pointer to the max value finding function
      find_max_val_fun find_max_val;
      static ui32 gen_find_max_val(ui32* addr) { return addr[0]; }

      // define line transfer function signature from subbands to codeblocks
      typedef void (*tx_to_cb_fun)(const void *sp, ui32 *dp, ui32 K_max,
                                   float delta_inv, ui32 count, ui32* max_val);
      // a pointer to function transferring samples from subbands to codeblocks
      tx_to_cb_fun tx_to_cb;
      static void gen_rev_tx_to_cb(const void *sp, ui32 *dp, ui32 K_max,
                                   float delta_inv, ui32 count, ui32* max_val);
      static void gen_irv_tx_to_cb(const void *sp, ui32 *dp, ui32 K_max,
                                   float delta_inv, ui32 count, ui32* max_val);

      // define line transfer function signature from codeblock to subband
      typedef void (*tx_from_cb_fun)(const ui32 *sp, void *dp, ui32 K_max,
                                     float delta, ui32 count);
      // a pointer to function transferring samples from codeblocks to subbands
      tx_from_cb_fun tx_from_cb;
      static void gen_rev_tx_from_cb(const ui32 *sp, void *dp, ui32 K_max,
                                     float delta, ui32 count);
      static void gen_irv_tx_from_cb(const ui32 *sp, void *dp, ui32 K_max,
                                     float delta, ui32 count);

      // define the block decoder function signature
      typedef bool (*cb_decoder_fun)(ui8* coded_data, ui32* decoded_data,
        ui32 missing_msbs, ui32 num_passes, ui32 lengths1, ui32 lengths2,
        ui32 width, ui32 height, ui32 stride, bool stripe_causal); 
      // a pointer to the decoder function
      static cb_decoder_fun decode_cb;
    };

    //////////////////////////////////////////////////////////////////////////
    struct coded_cb_header
    {
      ui32 pass_length[2];
      ui32 num_passes;
      ui32 Kmax;
      ui32 missing_msbs;
      coded_lists *next_coded;

      static const int prefix_buf_size = 8;
      static const int suffix_buf_size = 16;
    };
    
  }
}

#endif // !OJPH_CODEBLOCK_H