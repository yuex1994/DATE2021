#include "store_buf.h"

ExprRef STORE_BUF::rotate_left(ExprRef& op1, ExprRef& op2) {
  auto res = op1;
  for (int i = 1; i < STBUF_ENT_NUM; i++)
    res = Ite(op2 == BvConst(i, STBUF_ENT_SEL), Concat(Extract(op1, STBUF_ENT_NUM - 1 - i, 0), Extract(op1, STBUF_ENT_NUM - 1, STBUF_ENT_NUM - i)), res);
  return res;
}

ExprRef STORE_BUF::index_vector(std::vector<ExprRef>& v, ExprRef& index) {
  auto res = v[0];
  for (int i = 1; i < STBUF_ENT_NUM; i++)
    res = Ite(index == BvConst(i, STBUF_ENT_SEL), v[i], res);
  return res;
}

std::vector<ExprRef> STORE_BUF::search_end(std::vector<ExprRef>& in) {
  std::vector<ExprRef> res;
  auto out = BvConst(0, STBUF_ENT_SEL);
  auto en = BoolConst(0);
  for (int i = 0; i < STBUF_ENT_NUM; i++) {
    out = Ite(in[i] == BoolConst(1), BvConst(i, STBUF_ENT_SEL), out);
    en = Ite(in[i] == BoolConst(1), BoolConst(1), en);
  }
  res.push_back(out);
  res.push_back(en);
  return res;
}

std::vector<ExprRef> STORE_BUF::search_end_bv(ExprRef& in) {
  std::vector<ExprRef> res;
  auto out = BvConst(0, STBUF_ENT_SEL);
  auto en = BoolConst(0);
  for (int i = 0; i < STBUF_ENT_NUM; i++) {
    out = Ite(Extract(in, i, i) == BvConst(1, 1), BvConst(i, STBUF_ENT_SEL), out);
    en = Ite(Extract(in, i, i) == BvConst(1, 1), BoolConst(1), en);
  }
  res.push_back(out);
  res.push_back(en);
  return res;
}

std::vector<ExprRef> STORE_BUF::search_begin(std::vector<ExprRef>& in) {
  std::vector<ExprRef> res;
  auto out = BvConst(0, STBUF_ENT_SEL);
  auto en = BoolConst(0);
  for (int i = STBUF_ENT_NUM - 1; i >= 0 ; i--) {
    out = Ite(in[i] == BoolConst(1), BvConst(i, STBUF_ENT_SEL), out);
    en = Ite(in[i] == BoolConst(1), BoolConst(1), en);
  }
  res.push_back(out);
  res.push_back(en);
  return res;
}

void STORE_BUF::gen_valid_cls(std::vector<ExprRef>& result_and, std::vector<ExprRef>& result_nand, std::vector<ExprRef>& spec_bit, std::vector<ExprRef>& spectag) {
  auto spectagfix = model.input("spectagfix");
  for (int i = 0; i < spec_bit.size(); i++) {
    result_and.push_back(valid[i] & Ite(spec_bit[i] & ((spectagfix & spectag[i]) != 0), BoolConst(0), BoolConst(1)));
    result_nand.push_back((valid[i] == BoolConst(0)) | (Ite(spec_bit[i] & ((spectagfix & spectag[i]) != 0), BoolConst(0), BoolConst(1)) == BoolConst(0)));
  }
}

ExprRef STORE_BUF::gen_hitvec() {
  auto ldaddr = model.input("ldaddr");
  ExprRef result = Ite(valid[0] & (addr[0] == ldaddr), BvConst(1, 1), BvConst(0, 1));
  for (int i = 1; i < addr.size(); i++)
    result = Concat(Ite(valid[i] & (addr[i] == ldaddr), BvConst(1, 1), BvConst(0, 1)), result);
  return result;
}


void STORE_BUF::update_bit(std::vector<ExprRef>& result, std::vector<ExprRef>& source, ExprRef& cond, ExprRef value) {
  for (int i = 0; i < source.size(); i++) {
    result.push_back(Ite(cond == i, value, source[i]));
  }
}
