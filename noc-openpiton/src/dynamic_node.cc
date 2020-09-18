#include "dynamic_node.h"

D_NODE_OUT::D_NODE_OUT() : model("D_NODE_OUT") { 
  auto ROUTE_A = BvConst(0b000, 3);
  auto ROUTE_B = BvConst(0b001, 3);
  auto ROUTE_C = BvConst(0b010, 3);
  auto ROUTE_D = BvConst(0b011, 3);
  auto ROUTE_X = BvConst(0b100, 3);

  f_in_out_route.push_back({ROUTE_X, ROUTE_A, ROUTE_C, ROUTE_D, ROUTE_C});
  f_in_out_route.push_back({ROUTE_A, ROUTE_X, ROUTE_D, ROUTE_B, ROUTE_A});
  f_in_out_route.push_back({ROUTE_D, ROUTE_B, ROUTE_X, ROUTE_A, ROUTE_D});
  f_in_out_route.push_back({ROUTE_B, ROUTE_C, ROUTE_A, ROUTE_X, ROUTE_B});
  f_in_out_route.push_back({ROUTE_C, ROUTE_D, ROUTE_B, ROUTE_C, ROUTE_X});

  auto yummyIn_N = model.NewBoolInput("yummyIn_N");
  auto yummyIn_S = model.NewBoolInput("yummyIn_S");
  auto yummyIn_W = model.NewBoolInput("yummyIn_W");
  auto yummyIn_E = model.NewBoolInput("yummyIn_E");
  auto yummyIn_P = model.NewBoolInput("yummyIn_P");

  auto myLocX = model.NewBvState("myLocX", XY_WIDTH);
  auto myLocY = model.NewBvState("myLocY", XY_WIDTH);
  auto myChipID = model.NewBvState("myChipID", CHIP_ID_WIDTH);

  auto dataOut_N = model.NewBvState("dataOut_N", DATA_WIDTH);
  auto dataOut_S = model.NewBvState("dataOut_S", DATA_WIDTH);
  auto dataOut_W = model.NewBvState("dataOut_W", DATA_WIDTH);
  auto dataOut_E = model.NewBvState("dataOut_E", DATA_WIDTH);
  auto dataOut_P = model.NewBvState("dataOut_P", DATA_WIDTH);
  dataOut.push_back(dataOut_N);
  dataOut.push_back(dataOut_S);
  dataOut.push_back(dataOut_E);
  dataOut.push_back(dataOut_W);
  dataOut.push_back(dataOut_P);

  auto validOut_N = model.NewBoolState("validOut_N");
  auto validOut_S = model.NewBoolState("validOut_S");
  auto validOut_W = model.NewBoolState("validOut_W");
  auto validOut_E = model.NewBoolState("validOut_E");
  auto validOut_P = model.NewBoolState("validOut_P");
  validOut.push_back(validOut_N);
  validOut.push_back(validOut_S);
  validOut.push_back(validOut_E);
  validOut.push_back(validOut_W);
  validOut.push_back(validOut_P);

  auto current_route_N = model.NewBvState("current_route_N", 3);
  auto current_route_S = model.NewBvState("current_route_S", 3);
  auto current_route_W = model.NewBvState("current_route_W", 3);
  auto current_route_E = model.NewBvState("current_route_E", 3);
  auto current_route_P = model.NewBvState("current_route_P", 3);
  current_route.push_back(current_route_N);
  current_route.push_back(current_route_S);
  current_route.push_back(current_route_E);
  current_route.push_back(current_route_W);
  current_route.push_back(current_route_P);
 
  auto head_ptr_N = model.NewBvState("head_ptr_N", 2);
  auto head_ptr_S = model.NewBvState("head_ptr_S", 2);
  auto head_ptr_W = model.NewBvState("head_ptr_W", 2);
  auto head_ptr_E = model.NewBvState("head_ptr_E", 2);
  auto head_ptr_P = model.NewBvState("head_ptr_P", 2);
  head_ptr.push_back(head_ptr_N);  
  head_ptr.push_back(head_ptr_S);  
  head_ptr.push_back(head_ptr_E);  
  head_ptr.push_back(head_ptr_W);  
  head_ptr.push_back(head_ptr_P);  

  auto thanks_N = model.NewBoolState("thanks_N");
  auto thanks_S = model.NewBoolState("thanks_S");
  auto thanks_E = model.NewBoolState("thanks_E");
  auto thanks_W = model.NewBoolState("thanks_W");
  auto thanks_P = model.NewBoolState("thanks_P");

  thanks.push_back(thanks_N);
  thanks.push_back(thanks_S);
  thanks.push_back(thanks_E);
  thanks.push_back(thanks_W);
  thanks.push_back(thanks_P);

  for (int i = 0; i < 5; i++) {
    buf.push_back(std::vector<ExprRef>());
  }
 
  for (int i = 0; i < 4; i++) {
    buf[0].push_back(model.NewBvState("north_buf_" + std::to_string(i), DATA_WIDTH));
    buf[1].push_back(model.NewBvState("south_buf_" + std::to_string(i), DATA_WIDTH));
    buf[2].push_back(model.NewBvState("east_buf_" + std::to_string(i), DATA_WIDTH));
    buf[3].push_back(model.NewBvState("west_buf_" + std::to_string(i), DATA_WIDTH));
  }
  for (int i = 0; i < 4; i++)
    buf[4].push_back(model.NewBvState("pip_buf_" + std::to_string(i), DATA_WIDTH));

  std::vector<std::string> route_name = {"N", "S", "E", "W" ,"P"};
  std::vector<int> route_v = {0, 0, 0, 0, 0};
  for (int i = 0; i < 32; i++) { 
    std::string instr_name = "";
    for (int j = 0; j < 5; j++) {
      if (i & (1 << j))
        instr_name = instr_name + route_name[j]; 
    }
    if (i != 0)
      instr_name = instr_name + "_yummy_";
    for (int j = 0; j < 5; j++) {
      if (i & (1 << j))
        continue;
      else
        instr_name = instr_name + route_name[j];
    }
    instr_name += "_no_yummy";
    std::cout << instr_name << std::endl;
    for (int j = 0; j < 5; j++) 
      route_v[j] = (i >> j) & 1;
    auto instr = model.NewInstr(instr_name); 
    instr.SetDecode((yummyIn_N == BoolConst(route_v[0])) & (yummyIn_S == BoolConst(route_v[1])) & (yummyIn_E == BoolConst(route_v[2])) & (yummyIn_W == BoolConst(route_v[3])) & (yummyIn_P == BoolConst(route_v[4])));
    for (int j = 0; j < 5; j++) {
      auto thanks_expr = BoolConst(0);
      auto head_ptr_cond = BoolConst(0);
      for (int out_v = 0; out_v < 5; out_v++) {
        if (route_v[out_v]) {
          auto r_v = f_in_out_route[j][out_v];
          thanks_expr = Ite(current_route[out_v] == r_v, BoolConst(1), thanks_expr);
          head_ptr_cond = head_ptr_cond | (current_route[out_v] == r_v);
        }
      }
      instr.SetUpdate(thanks[j], thanks_expr);
      instr.SetUpdate(head_ptr[j], Ite(head_ptr_cond, head_ptr[j] + 1, head_ptr[j]));
    }

    for (int out_v = 0; out_v < 5; out_v++) {
      if (route_v[out_v]) {
        auto data_out_expr = dataOut[out_v];
        for (int j = 0; j < 5; j++) {
          auto data_out_tmp = buf[j][0];
          for (int k = 1; k < 4; k++) {
            data_out_tmp = Ite(head_ptr[j] == k, buf[j][k], data_out_tmp);  
          }
          auto route_v = f_in_out_route[j][out_v];
          data_out_expr = Ite(current_route[out_v] == route_v, data_out_tmp, data_out_expr); 
        }
        instr.SetUpdate(dataOut[out_v], data_out_expr);
      } 
    }  
  }

}

void D_NODE_IN::update_buf(std::vector<ExprRef>& buf, ExprRef& validIn, ExprRef& tail_ptr, ExprRef& dataIn, InstrRef& instr, int len) {
  for (int i = 0; i < len; i++) {
    instr.SetUpdate(buf[i], Ite(validIn, Ite(tail_ptr == i, dataIn, buf[i]), buf[i]));
  } 
  instr.SetUpdate(tail_ptr, Ite(validIn, tail_ptr + 1, tail_ptr));
}

ExprRef D_NODE_IN::select_entry(std::vector<ExprRef>& buf, ExprRef& head_ptr, int len) {
  auto res = buf[0];
  for (int i = 1; i < len; i++) {
    res = Ite(head_ptr == i, buf[i], res);
  }
  return res;
}

void D_NODE_IN::compute_new_route(std::vector<ExprRef>& dataIn, ExprRef& myLocX, ExprRef& myLocY) {
  for (int i = 0; i < 5; i++) {
    new_route_in_out.push_back(std::vector<ExprRef>());
    auto more_x = myLocX < (Extract(dataIn[i], DATA_WIDTH - CHIP_ID_WIDTH - 1, DATA_WIDTH - CHIP_ID_WIDTH - XY_WIDTH));
    auto more_y = myLocY < (Extract(dataIn[i], DATA_WIDTH - CHIP_ID_WIDTH- XY_WIDTH -1, DATA_WIDTH - CHIP_ID_WIDTH - 2*XY_WIDTH));
    auto less_x = myLocX > (Extract(dataIn[i], DATA_WIDTH - CHIP_ID_WIDTH-1, DATA_WIDTH - CHIP_ID_WIDTH - XY_WIDTH));
    auto less_y = myLocY > (Extract(dataIn[i], DATA_WIDTH - CHIP_ID_WIDTH- XY_WIDTH -1, DATA_WIDTH - CHIP_ID_WIDTH - 2*XY_WIDTH));
    auto done_x = myLocX == (Extract(dataIn[i], DATA_WIDTH - CHIP_ID_WIDTH -1, DATA_WIDTH - CHIP_ID_WIDTH - XY_WIDTH));
    auto done_y = myLocY == (Extract(dataIn[i], DATA_WIDTH - CHIP_ID_WIDTH - XY_WIDTH -1, DATA_WIDTH - CHIP_ID_WIDTH - 2*XY_WIDTH));
    auto final_bits = (Extract(dataIn[i], DATA_WIDTH - CHIP_ID_WIDTH - 2 * XY_WIDTH-2, DATA_WIDTH - CHIP_ID_WIDTH - 2 * XY_WIDTH - 4));
    auto to_north = (done_x & less_y) | ((final_bits == FINAL_NORTH) & done_x & done_y);
    new_route_in_out[i].push_back(to_north);
    new_route_to_north.push_back(to_north); 
    auto to_south = (done_x & more_y) | ((final_bits == FINAL_SOUTH) & done_x & done_y); 
    new_route_in_out[i].push_back(to_south);
    new_route_to_south.push_back(to_south); 
    auto to_east = more_x | ((final_bits == FINAL_EAST) & done_x & done_y); 
    new_route_in_out[i].push_back(to_east);
    new_route_to_east.push_back(to_east); 
    auto to_west = less_x | ((final_bits == FINAL_WEST) & done_x & done_y); 
    new_route_in_out[i].push_back(to_west);
    new_route_to_west.push_back(to_west); 
    auto to_proc = ((final_bits == FINAL_NONE) & done_x & done_y);
    new_route_in_out[i].push_back(to_proc);
    new_route_to_proc.push_back(to_proc); 
    auto new_route_taken_to_N = to_north;
    auto new_route_taken_to_W = to_west;
    auto new_route_taken_to_S = to_south;
    auto new_route_taken_to_E = to_east;
    auto new_route_taken_to_P = to_proc;
    auto tmp_N = BoolConst(0);
    auto tmp_S = BoolConst(0);
    auto tmp_E = BoolConst(0);
    auto tmp_W = BoolConst(0);
    auto tmp_P = BoolConst(0);
    for (int j = 0; j < 5; j++) {
      if (i == j)
        continue;
      tmp_N = tmp_N | ((current_route[j] == f_in_out_route[j][0]) & (header[j]));
      tmp_S = tmp_S | ((current_route[j] == f_in_out_route[j][1]) & (header[j]));
      tmp_E = tmp_E | ((current_route[j] == f_in_out_route[j][2]) & (header[j]));
      tmp_W = tmp_W | ((current_route[j] == f_in_out_route[j][3]) & (header[j]));
      tmp_P = tmp_P | ((current_route[j] == f_in_out_route[j][4]) & (header[j])); 
    }
    new_route_taken_to_N = new_route_taken_to_N & (tmp_N);
    new_route_taken_to_S = new_route_taken_to_N & (tmp_S);
    new_route_taken_to_E = new_route_taken_to_N & (tmp_E);
    new_route_taken_to_W = new_route_taken_to_N & (tmp_W);
    new_route_taken_to_P = new_route_taken_to_N & (tmp_P);
    auto route_taken = (new_route_taken_to_N | new_route_taken_to_W | new_route_taken_to_E | new_route_taken_to_S | new_route_taken_to_P);
    new_route_taken.push_back(route_taken);
    auto payload_length = Extract(dataIn[i], DATA_WIDTH - CHIP_ID_WIDTH - 2*XY_WIDTH - 5, DATA_WIDTH - CHIP_ID_WIDTH - 2*XY_WIDTH - 4 -PAYLOAD_LEN);  
    new_payload_length.push_back(payload_length);
  }
}
// 375



D_NODE_IN::D_NODE_IN() : model("D_NODE_IN") {
  auto ROUTE_A = BvConst(0b000, 3);
  auto ROUTE_B = BvConst(0b001, 3);
  auto ROUTE_C = BvConst(0b010, 3);
  auto ROUTE_D = BvConst(0b011, 3);
  auto ROUTE_X = BvConst(0b100, 3);
  f_in_out_route.push_back({ROUTE_X, ROUTE_A, ROUTE_C, ROUTE_D, ROUTE_C});
  f_in_out_route.push_back({ROUTE_A, ROUTE_X, ROUTE_D, ROUTE_B, ROUTE_A});
  f_in_out_route.push_back({ROUTE_D, ROUTE_B, ROUTE_X, ROUTE_A, ROUTE_D});
  f_in_out_route.push_back({ROUTE_B, ROUTE_C, ROUTE_A, ROUTE_X, ROUTE_B});
  f_in_out_route.push_back({ROUTE_C, ROUTE_D, ROUTE_B, ROUTE_C, ROUTE_X});
  rr_out_route_in.push_back({1, 3, 4, 2, 0}); 
  rr_out_route_in.push_back({0, 2, 3, 4, 1}); 
  rr_out_route_in.push_back({3, 4, 0, 1, 2}); 
  rr_out_route_in.push_back({2, 1, 4, 0, 3}); 
  rr_out_route_in.push_back({1, 3, 0, 2, 4}); 

  auto dataIn_N = model.NewBvInput("dataIn_N", DATA_WIDTH);
  auto dataIn_S = model.NewBvInput("dataIn_S", DATA_WIDTH);
  auto dataIn_W = model.NewBvInput("dataIn_W", DATA_WIDTH);
  auto dataIn_E = model.NewBvInput("dataIn_E", DATA_WIDTH);
  auto dataIn_P = model.NewBvInput("dataIn_P", DATA_WIDTH);
  std::vector<ExprRef> dataIn;
  dataIn.push_back(dataIn_N);
  dataIn.push_back(dataIn_S);
  dataIn.push_back(dataIn_E);
  dataIn.push_back(dataIn_W);
  dataIn.push_back(dataIn_P);

  auto validIn_N = model.NewBoolInput("validIn_N");
  auto validIn_S = model.NewBoolInput("validIn_S");
  auto validIn_W = model.NewBoolInput("validIn_W");
  auto validIn_E = model.NewBoolInput("validIn_E");
  auto validIn_P = model.NewBoolInput("validIn_P");

  auto myLocX = model.NewBvState("myLocX", XY_WIDTH);
  auto myLocY = model.NewBvState("myLocY", XY_WIDTH);
  auto myChipID = model.NewBvState("myChipID", CHIP_ID_WIDTH);

  auto yummyOut_N = model.NewBoolState("yummyOut_N");
  auto yummyOut_S = model.NewBoolState("yummyOut_S");
  auto yummyOut_W = model.NewBoolState("yummyOut_W");
  auto yummyOut_E = model.NewBoolState("yummyOut_E");
  auto yummyOut_P = model.NewBoolState("yummyOut_P");
  yummyOut.push_back(yummyOut_N);
  yummyOut.push_back(yummyOut_S);
  yummyOut.push_back(yummyOut_W);
  yummyOut.push_back(yummyOut_E);
  yummyOut.push_back(yummyOut_P);

  auto header_N = model.NewBoolState("header_N");
  auto header_S = model.NewBoolState("header_S");
  auto header_W = model.NewBoolState("header_W");
  auto header_E = model.NewBoolState("header_E");
  auto header_P = model.NewBoolState("header_P");
  header.push_back(header_N);
  header.push_back(header_S);
  header.push_back(header_E);
  header.push_back(header_W);
  header.push_back(header_P);

  auto payload_length_N = model.NewBvState("payload_length_N", PAYLOAD_LEN);
  auto payload_length_S = model.NewBvState("payload_length_S", PAYLOAD_LEN);
  auto payload_length_W = model.NewBvState("payload_length_W", PAYLOAD_LEN);
  auto payload_length_E = model.NewBvState("payload_length_E", PAYLOAD_LEN);
  auto payload_length_P = model.NewBvState("payload_length_P", PAYLOAD_LEN);
  std::vector<ExprRef> payload_length;
  payload_length.push_back(payload_length_N);
  payload_length.push_back(payload_length_S);
  payload_length.push_back(payload_length_E);
  payload_length.push_back(payload_length_W);
  payload_length.push_back(payload_length_P);

  auto current_route_N = model.NewBvState("current_route_N", 3);
  auto current_route_S = model.NewBvState("current_route_S", 3);
  auto current_route_W = model.NewBvState("current_route_W", 3);
  auto current_route_E = model.NewBvState("current_route_E", 3);
  auto current_route_P = model.NewBvState("current_route_P", 3);
  current_route.push_back(current_route_N);
  current_route.push_back(current_route_S);
  current_route.push_back(current_route_E);
  current_route.push_back(current_route_W);
  current_route.push_back(current_route_P);
  
  auto tail_ptr_N = model.NewBvState("tail_ptr_N", 2);
  auto tail_ptr_S = model.NewBvState("tail_ptr_S", 2);
  auto tail_ptr_W = model.NewBvState("tail_ptr_W", 2);
  auto tail_ptr_E = model.NewBvState("tail_ptr_E", 2);
  auto tail_ptr_P = model.NewBvState("tail_ptr_P", 2);

  auto thanks_N = model.NewBoolState("thanks_N");
  auto thanks_S = model.NewBoolState("thanks_S");
  auto thanks_E = model.NewBoolState("thanks_E");
  auto thanks_W = model.NewBoolState("thanks_W");
  auto thanks_P = model.NewBoolState("thanks_P");

  tail_ptr.push_back(tail_ptr_N);
  tail_ptr.push_back(tail_ptr_S);
  tail_ptr.push_back(tail_ptr_E);
  tail_ptr.push_back(tail_ptr_W);
  tail_ptr.push_back(tail_ptr_P);

  thanks.push_back(thanks_N);
  thanks.push_back(thanks_S);
  thanks.push_back(thanks_E);
  thanks.push_back(thanks_W);
  thanks.push_back(thanks_P);

  for (int i = 0; i < 5; i++) {
    buf.push_back(std::vector<ExprRef>());
  }
 
  for (int i = 0; i < 4; i++) {
    buf[0].push_back(model.NewBvState("north_buf_" + std::to_string(i), DATA_WIDTH));
    buf[1].push_back(model.NewBvState("south_buf_" + std::to_string(i), DATA_WIDTH));
    buf[2].push_back(model.NewBvState("east_buf_" + std::to_string(i), DATA_WIDTH));
    buf[3].push_back(model.NewBvState("west_buf_" + std::to_string(i), DATA_WIDTH));
  }
  for (int i = 0; i < 4; i++)
    buf[4].push_back(model.NewBvState("pip_buf_" + std::to_string(i), DATA_WIDTH));


  // Path Order: N:0 -> S:1 -> E:2 -> W:3 -> P:4
  
  compute_new_route(dataIn, myLocX, myLocY);
/* 
  {
    auto instr = model.NewInstr("N_valid_SWEP_no_valid");
    instr.SetDecode((validIn_N == BoolConst(1)) & (validIn_S == BoolConst(0)) & (validIn_E == BoolConst(0)) & (validIn_W == BoolConst(0)) & (validIn_P == BoolConst(0)));
    // instr.SetUpdate(yummyOut_N, Ite(new_route_N_taken & header_N, BoolConst(0), Ite(thanks_N, BoolConst(1), BoolConst(0))));
    // instr.SetUpdate(yummyOut_S, BoolConst(0));
    // instr.SetUpdate(yummyOut_W, BoolConst(0));
    // instr.SetUpdate(yummyOut_E, BoolConst(0));
    // instr.SetUpdate(yummyOut_P, BoolConst(0));
    instr.SetUpdate(current_route[0], Ite((new_route_taken[0] == BoolConst(0)) & header[0] & new_route_to_north[0], f_in_out_route[0][0], current_route[0]));
    instr.SetUpdate(current_route[2], Ite((new_route_taken[0] == BoolConst(0)) & header[0] & new_route_to_east[0], f_in_out_route[0][2], current_route[2]));
    instr.SetUpdate(current_route[1], Ite((new_route_taken[0] == BoolConst(0)) & header[0] & new_route_to_south[0], f_in_out_route[0][1], current_route[1]));
    instr.SetUpdate(current_route[3], Ite((new_route_taken[0] == BoolConst(0)) & header[0] & new_route_to_west[0], f_in_out_route[0][3], current_route[3]));
    instr.SetUpdate(current_route[4], Ite((new_route_taken[0] == BoolConst(0)) & header[0] & new_route_to_proc[0], f_in_out_route[0][4], current_route[4]));



    instr.SetUpdate(payload_length[0], Ite(header[0], new_payload_length[0], Ite(thanks_N, payload_length[0] - 1, payload_length[0])));
    instr.SetUpdate(payload_length[1], Ite(thanks_S, payload_length[1] - 1, payload_length[1]));
    instr.SetUpdate(payload_length[3], Ite(thanks_W, payload_length[3] - 1, payload_length[3]));
    instr.SetUpdate(payload_length[2], Ite(thanks_E, payload_length[2] - 1, payload_length[2]));
    instr.SetUpdate(payload_length[4], Ite(thanks_P, payload_length[4] - 1, payload_length[4]));
    instr.SetUpdate(tail_ptr_N, tail_ptr_N + 1);

    auto const_1 = BoolConst(1);
    update_buf(north_buf, const_1, tail_ptr_N, dataIn_N, instr, 4);
    // Maybe need update head_ptr
    instr.SetUpdate(header[0], Ite(header[0], Ite((new_payload_length[0] == 0), header[0], BoolConst(0)), Ite((payload_length[0] == 1) & (thanks_N), BoolConst(1), BoolConst(0))));
    instr.SetUpdate(header[1], header[1]);
    instr.SetUpdate(header[2], header[2]);
    instr.SetUpdate(header[3], header[3]);
    instr.SetUpdate(header[4], header[4]);
  }
*/

  std::vector<std::string> route_name = {"N", "S", "E", "W" ,"P"};
  std::vector<int> route_v = {0, 0, 0, 0, 0};

  for (int i = 0; i < 32; i++) { 
    std::string instr_name = "";
    for (int j = 0; j < 5; j++) {
      if (i & (1 << j))
        instr_name = instr_name + route_name[j]; 
    }
    if (i != 0)
      instr_name = instr_name + "_valid_";
    for (int j = 0; j < 5; j++) {
      if (i & (1 << j))
        continue;
      else
        instr_name = instr_name + route_name[j];
    }
    instr_name += "_no_valid";
    std::cout << instr_name << std::endl;
    for (int j = 0; j < 5; j++) 
      route_v[j] = (i >> j) & 1;
    auto instr = model.NewInstr(instr_name); 

    instr.SetDecode((validIn_N == BoolConst(route_v[0])) & (validIn_S == BoolConst(route_v[1])) & (validIn_E == BoolConst(route_v[2])) & (validIn_W == BoolConst(route_v[3])) & (validIn_P == BoolConst(route_v[4])));

    std::vector<ExprRef> new_current_route(current_route.begin(), current_route.end());
    for (int j = 0; j < 5; j++) {
      for (int k = 4; k >= 0; k--) {
        auto in_v = rr_out_route_in[j][k];
        if (route_v[in_v]) {
          new_current_route[j] = Ite((new_route_taken[in_v] == BoolConst(0)) & header[in_v] & new_route_in_out[in_v][j], f_in_out_route[in_v][j], new_current_route[j]);
        }
      }
      instr.SetUpdate(current_route[j], new_current_route[j]);
    }

    for (int j = 0; j < 5; j++) {
      if (route_v[j]) {
        instr.SetUpdate(payload_length[j], Ite(header[j], new_payload_length[j], Ite(thanks[j], payload_length[j] - 1, payload_length[j])));
        instr.SetUpdate(tail_ptr[j], tail_ptr[j] + 1);
        auto const_1 = BoolConst(1);
        update_buf(buf[j], const_1, tail_ptr[j], dataIn[j], instr, 4);
        instr.SetUpdate(header[j], Ite(header[j], Ite((new_payload_length[j] == 0), header[j], BoolConst(0)), Ite((payload_length[j] == 1) & (thanks[j]), BoolConst(1), BoolConst(0))));
        instr.SetUpdate(yummyOut_N, Ite(new_route_taken[j] & header[j], BoolConst(0), Ite(thanks[j], BoolConst(1), BoolConst(0))));

      } else {
        instr.SetUpdate(payload_length[j], Ite(thanks[j], payload_length[j] - 1, payload_length[j]));
        instr.SetUpdate(header[j], header[j]);
      }
    }
  }
}
// 80
