#ifndef DYNAMIC_NODE_H__
#define DYNAMIC_NODE_H__
#include <ilang/ilang++.h>

using namespace ilang;

class D_NODE_OUT {
  public: 
    D_NODE_OUT();
    void update_buf(std::vector<ExprRef>& buf, ExprRef& validIn, ExprRef& tail_ptr, ExprRef& dataIn, InstrRef& instr, int len);
    ExprRef select_entry(std::vector<ExprRef>& buf, ExprRef& head_ptr, int len); 
    Ila model; 
    std::vector<ExprRef> thanks;
    std::vector<std::vector<ExprRef>> f_in_out_route;
    std::vector<std::vector<ExprRef>> buf;
    std::vector<ExprRef> current_route;
    std::vector<ExprRef> head_ptr;
    std::vector<ExprRef> dataOut;
    std::vector<ExprRef> validOut;
};

class D_NODE_IN {
  public: 
    D_NODE_IN();
    void update_buf(std::vector<ExprRef>& buf, ExprRef& validIn, ExprRef& tail_ptr, ExprRef& dataIn, InstrRef& instr, int len);
    ExprRef select_entry(std::vector<ExprRef>& buf, ExprRef& head_ptr, int len); 
    void compute_new_route(std::vector<ExprRef>& dataIn, ExprRef& myLocX, ExprRef& myLocY); 
    Ila model; 
    std::vector<ExprRef> current_route;
    std::vector<ExprRef> header;
    std::vector<std::vector<ExprRef>> new_route_in_out;
    std::vector<ExprRef> new_route_to_north;
    std::vector<ExprRef> new_route_to_south;
    std::vector<ExprRef> new_route_to_east;
    std::vector<ExprRef> new_route_to_west;
    std::vector<ExprRef> new_route_to_proc;
    std::vector<ExprRef> new_route_taken;
    std::vector<ExprRef> new_payload_length;
    std::vector<ExprRef> yummyOut;
    std::vector<std::vector<ExprRef>> f_in_out_route;
    std::vector<std::vector<int>> rr_out_route_in;
    std::vector<ExprRef> tail_ptr;
    std::vector<std::vector<ExprRef>> buf;
    std::vector<ExprRef> thanks;
};

#define FINAL_NONE      0b000 
#define FINAL_WEST      0b010
#define FINAL_SOUTH     0b011
#define FINAL_EAST      0b100
#define FINAL_NORTH     0b101
#define START_NONE      0b000 
#define START_WEST      0b010
#define START_SOUTH     0b011
#define START_EAST      0b100
#define START_NORTH     0b101

#define    XY_WIDTH 8
#define    CHIP_ID_WIDTH 14
#define    PAYLOAD_LEN 8
#define    DATA_WIDTH 64
#define    HALF_DATA_WIDTH 32
#define    OFF_CHIP_NODE_X 0
#define    OFF_CHIP_NODE_Y 0
#define    MAX_FILE_SIZE 1024
#define    FINAL_BITS 4

#endif
