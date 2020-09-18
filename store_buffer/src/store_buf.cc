#include "store_buf.h"

void STORE_BUF::load_port() {
  model.NewBvInput("ldaddr", ADDR_LEN);  
  auto outdata = model.NewBvState("outdata", DATA_LEN);
  auto outhit = model.NewBoolState("outhit");
  auto hitvec = gen_hitvec();
  auto ptr = BvConst(0, STBUF_ENT_SEL) - model.state("finptr"); 
  auto hitvec_rot = rotate_left(hitvec, ptr);
  auto findhit = search_end_bv(hitvec_rot); 
  {
    auto instr = model.NewInstr("ld_port_hit");
    instr.SetDecode(findhit[1]);
    auto index = findhit[0] + model.state("finptr");
    auto lddata = index_vector(data, index); 
    instr.SetUpdate(outdata, lddata);
    instr.SetUpdate(outhit, BoolConst(1));
  }
  {
    auto instr = model.NewInstr("ld_port_no_hit");
    instr.SetDecode(findhit[1] == BoolConst(0));
    instr.SetUpdate(outhit, BoolConst(0));
  }
}

void STORE_BUF::spec_port_input_state() {
  model.NewBoolInput("prmiss");
  model.NewBvInput("prtag", SPECTAG_LEN);
  model.NewBoolInput("prsuccess");
  model.NewBoolInput("stspecbit");
  model.NewBoolInput("stcom");
  model.NewBoolInput("memoccupy_ld");
  model.NewBvState("retptr", STBUF_ENT_SEL); 
  model.NewBvState("comptr", STBUF_ENT_SEL);
}

void STORE_BUF::spec_port_inst_0(std::vector<ExprRef>& decode, std::vector<ExprRef>& u_state, std::vector<ExprRef>& u_func) {
    decode.push_back(model.input("prmiss"));
    for (int i = 0; i < STBUF_ENT_NUM; i++) {
      u_state.push_back(specbit[i]);
      u_func.push_back(BoolConst(0));
    }
}

void STORE_BUF::spec_port_inst_1(std::vector<ExprRef>& decode, std::vector<ExprRef>& u_state, std::vector<ExprRef>& u_func) {
  auto retptr = model.state("retptr");
  auto valid_ret = index_vector(valid, retptr);
  auto completed_ret = index_vector(completed, retptr); 
  decode.push_back(model.input("prmiss") == BoolConst(0));
  u_state.push_back(model.state("retptr"));
  u_func.push_back(Ite(((model.input("stfin") & (model.state("finptr") == model.state("retptr"))) == BoolConst(0)) & ((model.input("stcom") & (model.state("comptr") == model.state("retptr"))) == BoolConst(0)) & valid_ret & completed_ret & (model.input("memoccupy_ld") == BoolConst(0)), model.state("retptr") + BvConst(1, STBUF_ENT_SEL), model.state("retptr")));
  u_state.push_back(model.state("comptr"));
  u_func.push_back(Ite(model.input("stcom") & ((model.input("stfin") & (model.state("finptr") == model.state("comptr"))) == BoolConst(0)), model.state("comptr") + 1, model.state("comptr")));
  for (int i = 0; i < STBUF_ENT_NUM; i++) {
    u_state.push_back(specbit[i]);
    auto spec_cls = Ite(model.input("prtag") == spectag[i], BoolConst(0), BoolConst(1));
    u_func.push_back(Ite(model.input("prsuccess"), (specbit[i] & spec_cls) | Ite((model.input("stfin") == BoolConst(0)) | (model.state("finptr") > BvConst(i, STBUF_ENT_SEL)), BoolConst(0), model.input("stspecbit")), Ite(model.input("stfin") & (model.state("finptr") == i), model.input("stspecbit"), specbit[i])));
  }
}

void STORE_BUF::in_port_input_state() {
  model.NewBoolInput("stfin");
  model.NewBvInput("stdata", DATA_LEN);
  model.NewBvInput("staddr", ADDR_LEN);
  model.NewBvInput("stspectag", SPECTAG_LEN);
  model.NewBvInput("spectagfix", SPECTAG_LEN);
  model.NewBvState("finptr", STBUF_ENT_SEL);
}

void STORE_BUF::shared_inst() {
  std::vector<std::vector<ExprRef>> shared_valid_update(4, std::vector<ExprRef>()), shared_completed_update(4, std::vector<ExprRef>()); 
  std::vector<ExprRef> valid_and_valid_cls, valid_nand_valid_cls;
  gen_valid_cls(valid_and_valid_cls, valid_nand_valid_cls, specbit, spectag); 
  auto retptr = model.state("retptr");
  auto stretire = index_vector(valid, retptr) & index_vector(completed, retptr) & (model.input("memoccupy_ld") == BoolConst(0));
  for (int i = 0; i < STBUF_ENT_NUM; i++) {
    shared_valid_update[0].push_back(Ite(model.state("finptr") == i, BoolConst(1), valid[i]));  
    shared_completed_update[0].push_back(Ite(model.state("finptr") == i, BoolConst(0), completed[i]));
    shared_valid_update[1].push_back(valid_and_valid_cls[i]);
    shared_completed_update[1].push_back(completed[i]);
    std::cout << "alive" << std::endl;
    shared_valid_update[2].push_back(Ite(model.state("finptr") == i, BoolConst(1), Ite(model.input("stcom") & (model.state("comptr") == i), valid[i], Ite(stretire & (model.state("retptr") == i), BoolConst(0), valid[i]))));
    shared_completed_update[2].push_back(Ite(model.state("finptr") == i, BoolConst(0), Ite(model.input("stcom") & (model.state("comptr") == i), BoolConst(1), Ite(stretire & (model.state("retptr") == i), BoolConst(0), completed[i]))));
    shared_valid_update[3].push_back(Ite(model.input("stcom") & (model.state("comptr") == i), valid[i], Ite(stretire & (model.state("retptr") == i), BoolConst(0), valid[i])));
    shared_completed_update[3].push_back(Ite(model.input("stcom") & (model.state("comptr") == i), BoolConst(1), Ite(stretire & (model.state("retptr") == i), BoolConst(0), completed[i])));
  }
  std::vector<void (STORE_BUF::*)(std::vector<ExprRef>&, std::vector<ExprRef>&, std::vector<ExprRef>&)> spec_func;
  std::vector<void (STORE_BUF::*)(std::vector<ExprRef>&, std::vector<ExprRef>&, std::vector<ExprRef>&)> in_func;
  spec_func.push_back(&STORE_BUF::spec_port_inst_0);
  spec_func.push_back(&STORE_BUF::spec_port_inst_1);
  in_func.push_back(&STORE_BUF::in_port_inst_0);
  in_func.push_back(&STORE_BUF::in_port_inst_1);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      auto instr = model.NewInstr("spec_func_" + std::to_string(i) + "_in_func_" + std::to_string(j));
      integrate_instr(instr, spec_func[i], in_func[j], *this, shared_valid_update[i*2+j], shared_completed_update[i*2+j]);
    }
  } 
}

void integrate_instr(InstrRef& instr, void (STORE_BUF::*func_1)(std::vector<ExprRef>&, std::vector<ExprRef>&, std::vector<ExprRef>&), void (STORE_BUF::*func_2)(std::vector<ExprRef>&, std::vector<ExprRef>&, std::vector<ExprRef>&), STORE_BUF& ptr, std::vector<ExprRef>& shared_valid, std::vector<ExprRef>& shared_completed) {
  std::vector<ExprRef> decode, u_state, u_func;
  (ptr.*func_1)(decode, u_state, u_func);
  (ptr.*func_2)(decode, u_state, u_func);
  auto decode_func = BoolConst(1);
  for (auto a: decode)
    decode_func = decode_func & ExprRef(a);
  instr.SetDecode(decode_func);
  for (int i = 0; i < u_state.size(); i++)
    instr.SetUpdate(ExprRef(u_state[i]), ExprRef(u_func[i]));
  for (int i = 0; i < STBUF_ENT_NUM; i++) {
    instr.SetUpdate(ptr.valid[i], shared_valid[i]);
    instr.SetUpdate(ptr.completed[i], shared_completed[i]);
  }
}
                           
void STORE_BUF::in_port_inst_0(std::vector<ExprRef>& decode, std::vector<ExprRef>& u_state, std::vector<ExprRef>& u_func) {
  decode.push_back(model.input("stfin"));
  for (int i = 0; i < STBUF_ENT_NUM; i++) {
    u_state.push_back(data[i]);
    u_func.push_back(Ite(model.state("finptr") == i, model.input("stdata"), data[i]));
    u_state.push_back(addr[i]);
    u_func.push_back(Ite(model.state("finptr") == i, model.input("staddr"), addr[i]));
    u_state.push_back(spectag[i]);
    u_func.push_back(Ite(model.state("finptr") == i, model.input("stspectag"), spectag[i])); 
  }
  u_state.push_back(model.state("finptr"));
  u_func.push_back(model.state("finptr") + BvConst(1, STBUF_ENT_SEL));
}

void STORE_BUF::in_port_inst_1(std::vector<ExprRef>& decode, std::vector<ExprRef>& u_state, std::vector<ExprRef>& u_func) {
    decode.push_back(model.input("stfin") == BoolConst(0));
    std::vector<ExprRef> valid_and_valid_cls, valid_nand_valid_cls; 
    gen_valid_cls(valid_and_valid_cls, valid_nand_valid_cls, specbit, spectag); 
    auto snb1 = search_begin(valid_and_valid_cls); 
    auto sne1 = search_end(valid_and_valid_cls);
    auto snb0 = search_begin(valid_nand_valid_cls);
    u_state.push_back(model.state("finptr"));
    u_func.push_back(Ite(model.input("prmiss"), Ite(sne1[1] & snb0[1], Ite((snb1[0] == 0) & (sne1[0] == (STBUF_ENT_NUM - 1)), snb0[0], sne1[0] + 1), model.state("retptr")), model.state("finptr"))); 
}

void STORE_BUF::add_shared_state() {
  for (int i = 0; i < STBUF_ENT_NUM; i++) {
    data.push_back(model.NewBvState("data_" + std::to_string(i), DATA_LEN));
    addr.push_back(model.NewBvState("addr_" + std::to_string(i), ADDR_LEN));
    specbit.push_back(model.NewBoolState("specbit_" + std::to_string(i)));
    valid.push_back(model.NewBoolState("valid_" + std::to_string(i)));
    completed.push_back(model.NewBoolState("completed_" + std::to_string(i)));
    spectag.push_back(model.NewBvState("spectag_" + std::to_string(i), SPECTAG_LEN)); 
  }
}

STORE_BUF::STORE_BUF() : model("store_buffer") { 
  in_port_input_state();
  spec_port_input_state();
  add_shared_state();
  load_port();
  shared_inst();
}

