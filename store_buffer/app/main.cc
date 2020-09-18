// main.cc

#include "store_buf.h"
#include <ilang/ila/instr_lvl_abs.h>
#include <iostream>
#include <ilang/vtarget-out/vtarget_gen.h>


int main() {
  STORE_BUF store_buf;
  std::vector<std::string> design_files = {"store_buf.v"};

  SetUnsignedComparison(true);
  
  std::ofstream fout("./ila.v"); 
  
  VerilogVerificationTargetGenerator::vtg_config_t ret; 
  //ret.ForceInstCheckReset = true;
  ret.CosaGenTraceVcd = true;
  VerilogGeneratorBase::VlgGenConfig vlg_cfg;
  vlg_cfg.pass_node_name = true;
  std::string RootPath = "..";
  std::string VerilogPath = RootPath + "/verilog/";
  std::string RefrelPath = RootPath + "/refinement/";
  std::string OutputPath = RootPath + "/verification/";
  VerilogVerificationTargetGenerator vg(
      {},                                                    // no include
      {VerilogPath + "store_buf.v"},
      "storebuf",                                             // top_module_name
      RefrelPath + "ref-rel-var-map.json",            // variable mapping
      RefrelPath + "ref-rel-inst-cond.json",          // conditions of start/ready
      OutputPath,                                            // output path
      store_buf.model.get(),                                           // model
      VerilogVerificationTargetGenerator::backend_selector::JASPERGOLD, // backend: COSA
      ret,  // target generator configuration
      vlg_cfg); // verilog generator configuration

  vg.GenerateTargets();


  return 0;
}
