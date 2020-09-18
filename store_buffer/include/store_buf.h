#ifndef STORE_BUF_H__
#define STORE_BUF_H__

#include <ilang/ilang++.h>
#include <vector>

#define STBUF_ENT_SEL 4
#define SPECTAG_LEN 4
#define STBUF_ENT_NUM 16
#define ADDR_LEN 16
#define DATA_LEN 32
using namespace ilang;

class STORE_BUF {
public:
  STORE_BUF();
  Ila model;

  void in_port_input_state();
  void spec_port_input_state();
  void load_port();

  void add_shared_state();
  void spec_port_inst_0(std::vector<ExprRef>& decode, std::vector<ExprRef>& u_state, std::vector<ExprRef>& u_func);
  void spec_port_inst_1(std::vector<ExprRef>& decode, std::vector<ExprRef>& u_state, std::vector<ExprRef>& u_func);
  //void spec_port_inst_0();
  void in_port_inst_0(std::vector<ExprRef>& decode, std::vector<ExprRef>& u_state, std::vector<ExprRef>& u_func);
  void in_port_inst_1(std::vector<ExprRef>& decode, std::vector<ExprRef>& u_state, std::vector<ExprRef>& u_func);
  void shared_inst();

  void update_bit(std::vector<ExprRef>& result, std::vector<ExprRef>& source, ExprRef& cond, ExprRef value);
  void gen_valid_cls(std::vector<ExprRef>& valid_and_valid_cls, std::vector<ExprRef>& valid_nand_valid_cls, std::vector<ExprRef>& spec_bit, std::vector<ExprRef>& spectag);

  ExprRef gen_hitvec();
  ExprRef index_vector(std::vector<ExprRef>& v, ExprRef& index);
  ExprRef rotate_left(ExprRef& op1, ExprRef& op2);
  std::vector<ExprRef> search_end_bv(ExprRef& in);
  std::vector<ExprRef> search_end(std::vector<ExprRef>& in);
  std::vector<ExprRef> search_begin(std::vector<ExprRef>& in);
  /*
  ExprRef finptr;
  ExprRef comptr;
  ExprRef retptr;
  ExprRef stfin;
  ExprRef prmiss;
  ExprRef spectag_fix;
  */
  std::vector<ExprRef> valid;
  std::vector<ExprRef> completed;
  std::vector<ExprRef> specbit;
  std::vector<ExprRef> spectag; 
  std::vector<ExprRef> data;
  std::vector<ExprRef> addr;
};


void integrate_instr(InstrRef& instr, void (STORE_BUF::*func_1)(std::vector<ExprRef>&, std::vector<ExprRef>&, std::vector<ExprRef>&), void (STORE_BUF::*func_2)(std::vector<ExprRef>&, std::vector<ExprRef>&, std::vector<ExprRef>&), STORE_BUF& ptr, std::vector<ExprRef>& shared_valid, std::vector<ExprRef>& shared_completed);

#endif
