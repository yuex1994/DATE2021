#include <dynamic_node.h>
#include <ilang/vtarget-out/vtarget_gen.h>

int main() {
  D_NODE_OUT dynamic_node_ila_model;
  std::vector<std::string> design_files = {
    "bus_compare_equal.v",
    "dynamic_input_control.v",
    "dynamic_input_route_request_calc.v",
    "dynamic_input_top_16.v",
    "dynamic_input_top_4.v",
    "dynamic_node_top.v",
    "dynamic_output_control.v",
    "dynamic_output_datapath.v",
    "dynamic_output_top.v",
    "flip_bus.v",
    "net_dff.v",
    "network_input_blk_multi_out.v",
    "one_of_eight.v",
    "one_of_five.v",
    "space_avail_top.v"};
  
  SetUnsignedComparison(true);
  
  VerilogVerificationTargetGenerator::vtg_config_t ret;
  ret.CosaGenTraceVcd = true;
  VerilogGeneratorBase::VlgGenConfig vlg_cfg;
  vlg_cfg.pass_node_name = true;
  std::string RootPath = "..";
  std::string VerilogPath = RootPath + "/verilog/";
  std::string RefrelPath = RootPath + "/refinement/";
  std::string OutputPath = RootPath + "/verification/";
  for (int i = 0; i < design_files.size(); i++) {
    design_files[i] = VerilogPath + design_files[i];
  } 

  VerilogVerificationTargetGenerator vg(
      {VerilogPath + "include/"},                                                    // no include
      design_files,
      "dynamic_node_top",
      RefrelPath + "var-map-outport.json",            // variable mapping
      RefrelPath + "inst-cond-outport.json",          // conditions of start/ready
      OutputPath,                                            // output path
      dynamic_node_ila_model.model.get(),                                           // model
      VerilogVerificationTargetGenerator::backend_selector::JASPERGOLD, // backend: COSA
      ret,  // target generator configuration
      vlg_cfg); // verilog generator configuration

  vg.GenerateTargets(); 


  D_NODE_IN dynamic_node_ila_model_in;
  VerilogVerificationTargetGenerator::vtg_config_t ret_in;
  ret_in.CosaGenTraceVcd = true;
  VerilogGeneratorBase::VlgGenConfig vlg_cfg_in;
  vlg_cfg_in.pass_node_name = true;
  VerilogVerificationTargetGenerator vg_in(
      {VerilogPath + "include/"},                                                    // no include
      design_files,
      "dynamic_node_top",
      RefrelPath + "var-map-inport.json",            // variable mapping
      RefrelPath + "inst-cond-inport.json",          // conditions of start/ready
      OutputPath,                                            // output path
      dynamic_node_ila_model_in.model.get(),                                           // model
      VerilogVerificationTargetGenerator::backend_selector::JASPERGOLD, // backend: COSA
      ret_in,  // target generator configuration
      vlg_cfg_in); // verilog generator configuration

  vg_in.GenerateTargets(); 

  return 0;
}
