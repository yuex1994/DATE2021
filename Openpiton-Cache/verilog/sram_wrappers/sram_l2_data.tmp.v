
/*
Copyright (c) 2015 Princeton University
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Princeton University nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY PRINCETON UNIVERSITY "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL PRINCETON UNIVERSITY BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// 02/06/2015 14:58:59
// This file is auto-generated
// Author: Tri Nguyen

`include "l2.tmp.h"
// /home/huaixil/ele575/openpiton/piton/verif/env/manycore/devices.xml

`include "define.tmp.h"
`ifdef DEFAULT_NETTYPE_NONE
`default_nettype none
`endif
module sram_l2_data
(
input wire MEMCLK,
input wire RESET_N,
input wire CE,
input wire [`L2_DATA_ARRAY_HEIGHT_LOG2-1:0] A,
input wire RDWEN,
input wire [`L2_DATA_ARRAY_WIDTH-1:0] BW,
input wire [`L2_DATA_ARRAY_WIDTH-1:0] DIN,
output wire [`L2_DATA_ARRAY_WIDTH-1:0] DOUT,
input wire [`BIST_OP_WIDTH-1:0] BIST_COMMAND,
input wire [`SRAM_WRAPPER_BUS_WIDTH-1:0] BIST_DIN,
output reg [`SRAM_WRAPPER_BUS_WIDTH-1:0] BIST_DOUT,
input wire [`BIST_ID_WIDTH-1:0] SRAMID
);

// `ifdef SYNTHESIZABLE_BRAM
// wire [`L2_DATA_ARRAY_WIDTH-1:0] DOUT_bram;
// assign DOUT = DOUT_bram;

// bram_1rw_wrapper #(
//    .NAME          (""             ),
//    .DEPTH         (`L2_DATA_ARRAY_HEIGHT),
//    .ADDR_WIDTH    (`L2_DATA_ARRAY_HEIGHT_LOG2),
//    .BITMASK_WIDTH (`L2_DATA_ARRAY_WIDTH),
//    .DATA_WIDTH    (`L2_DATA_ARRAY_WIDTH)
// )   sram_l2_data (
//    .MEMCLK        (MEMCLK     ),
//    .RESET_N        (RESET_N     ),
//    .CE            (CE         ),
//    .A             (A          ),
//    .RDWEN         (RDWEN      ),
//    .BW            (BW         ),
//    .DIN           (DIN        ),
//    .DOUT          (DOUT_bram       )
// );
      
// `else

// reg [`L2_DATA_ARRAY_WIDTH-1:0] cache [`L2_DATA_ARRAY_HEIGHT-1:0];
reg [`L2_DATA_ARRAY_WIDTH-1:0] cache [3:0];

// integer i;
// initial
// begin
//    for (i = 0; i < `L2_DATA_ARRAY_HEIGHT; i = i + 1)
//    begin
//       cache[i] = 0;
//    end
// end



   reg [`L2_DATA_ARRAY_WIDTH-1:0] dout_f;

   assign DOUT = dout_f;

   always @ (posedge MEMCLK)
   begin
      if(!RESET_N) begin  // ILA: add initial states
         cache[0] <= 0;
         cache[1] <= 0;
         cache[2] <= 0;
         cache[3] <= 0;
         end
      else if (CE)
      begin
         if (RDWEN == 1'b0)
            cache[A] <= (DIN & BW) | (cache[A] & ~BW);
         else
            dout_f <= cache[A];
      end
   end

// `endif 

 endmodule

