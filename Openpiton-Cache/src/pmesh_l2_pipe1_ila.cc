/// \file the ila example of OpenPiton L2 Cache Coherence ILA
///  Huaixi Lu (huaixil@princeton.edu)
///

#include "pmesh_l2_ila.h"


/*
#define NOC_MSG_WIDTH 8
#define MESI_WIDTH 2
#define DATA_WIDTH  64
#define TAG_WIDTH 26
#define FETCH_STATE_WIDTH 3
#define MSG_CACHE_TYPE_WIDTH  1
#define L2_OWNER_BITS         6
*/

PMESH_L2_PIPE1_ILA::PMESH_L2_PIPE1_ILA()
    : // construct the model
      model("PMESH_L2_PIPE1_ILA"),

      // PIPE1
      msg1_type       (model.NewBvInput("msg1_type", NOC_MSG_WIDTH)),
      msg1_data       (model.NewBvInput("msg1_data", DATA_WIDTH)), // only used in WB?
      msg1_source     (model.NewBvInput("msg1_source", L2_OWNER_BITS)),
      msg1_tag        (model.NewBvInput("msg1_tag", TAG_WIDTH)),
      // msg1_cache_type (model.NewbvInput("msg1_cache_type", MSG_CACHE_TYPE_WIDTH)),

      // Output states: l2 --> noc2 requests
      // not specifying these updates:   
      msg2_type       (model.NewBvState("msg2_type", NOC_MSG_WIDTH) ),

      // architecture states
      cache_vd        (model.NewBvState("cache_vd_pipe1", 2)),
      cache_state     (model.NewBvState("cache_state_pipe1", MESI_WIDTH)),

      cache_tag       (model.NewBvState("cache_tag_pipe1", TAG_WIDTH)),
      cache_data      (model.NewBvState("cache_data_pipe1",  DATA_WIDTH)),
      cache_owner     (model.NewBvState("cache_owner_pipe1", L2_OWNER_BITS)),
      share_list      (model.NewBvState("share_list", DIR_WIDTH)),
      // cache_type   (model.NewBvState("cache_type", MSG_CACHE_TYPE_WIDTH)),

      // internal states
      cur_msg_state   (model.NewBvState("cur_msg_state", 2)),
      cur_msg_type    (model.NewBvState("cur_msg_type", NOC_MSG_WIDTH)),
      cur_msg_source  (model.NewBvState("cur_msg_source", L2_OWNER_BITS)),
      cur_msg_tag     (model.NewBvState("cur_msg_tag" , TAG_WIDTH))
// -----------------------------------------------------------------------------------------------

{
  // ------------------------------ CONSTANTS ---------------------------------- //

  //auto MSG_CACHE_TYPE_DATA         =BvConst(0, MSG_CACHE_TYPE_WIDTH);
  //auto MSG_CACHE_TYPE_INS          =BvConst(1, MSG_CACHE_TYPE_WIDTH);
  auto STATE_INVAL   = BvConst(0,2);
  auto STATE_WAIT    = BvConst(1,2);
  auto STATE_PENDING = BvConst(2,2);

  // MSG TYPES
  auto L2_MSG_WIDTH  = NOC_MSG_WIDTH;
  // Requests from L15 to L2
/*
  // auto MSG_TYPE_RESERVED           =BvConst(0, L2_MSG_WIDTH);

  // auto MSG_TYPE_PREFETCH_REQ       =BvConst(1, L2_MSG_WIDTH);
*/
  auto MSG_TYPE_STORE_REQ          =BvConst(2, L2_MSG_WIDTH);
/*
  // auto MSG_TYPE_BLK_STORE_REQ      =BvConst(3, L2_MSG_WIDTH);
  // auto MSG_TYPE_BLKINIT_STORE_REQ  =BvConst(4, L2_MSG_WIDTH);
  // auto MSG_TYPE_CAS_REQ            =BvConst(5, L2_MSG_WIDTH);
  // auto MSG_TYPE_CAS_P1_REQ         =BvConst(6, L2_MSG_WIDTH); 
*/
  auto MSG_TYPE_LOAD_REQ           =BvConst(31, L2_MSG_WIDTH);
/*
  // condition satisfied
  // auto MSG_TYPE_CAS_P2Y_REQ        =BvConst(7, L2_MSG_WIDTH);
  // condition not satisfied
  // auto MSG_TYPE_CAS_P2N_REQ        =BvConst(8, L2_MSG_WIDTH);
  // Both SWAP and LDSTUB are the same for L2
  // auto MSG_TYPE_SWAP_REQ           =BvConst(9, L2_MSG_WIDTH);
  // auto MSG_TYPE_SWAP_P1_REQ        =BvConst(10, L2_MSG_WIDTH);
  // auto MSG_TYPE_SWAP_P2_REQ        =BvConst(11, L2_MSG_WIDTH);
*/
  auto MSG_TYPE_WB_REQ             =BvConst(12, L2_MSG_WIDTH);
  auto MSG_TYPE_WBGUARD_REQ        =BvConst(13, L2_MSG_WIDTH);
/*
  // auto MSG_TYPE_NC_LOAD_REQ        =BvConst(14, L2_MSG_WIDTH);
  // auto MSG_TYPE_NC_STORE_REQ       =BvConst(15, L2_MSG_WIDTH);
  // auto MSG_TYPE_INTERRUPT_FWD      =BvConst(32, L2_MSG_WIDTH);

  //RISC-V AMO requests
  // auto MSG_TYPE_AMO_ADD_REQ         =BvConst(36, L2_MSG_WIDTH);
  // auto MSG_TYPE_AMO_AND_REQ         =BvConst(37, L2_MSG_WIDTH);
  // auto MSG_TYPE_AMO_OR_REQ          =BvConst(38, L2_MSG_WIDTH);
  // auto MSG_TYPE_AMO_XOR_REQ         =BvConst(39, L2_MSG_WIDTH);
  // auto MSG_TYPE_AMO_MAX_REQ         =BvConst(40, L2_MSG_WIDTH);
  // auto MSG_TYPE_AMO_MAXU_REQ        =BvConst(41, L2_MSG_WIDTH);
  // auto MSG_TYPE_AMO_MIN_REQ         =BvConst(42, L2_MSG_WIDTH);
  // auto MSG_TYPE_AMO_MINU_REQ        =BvConst(43, L2_MSG_WIDTH);
  // RISC-V AMO L2-internal phase 1
  // auto MSG_TYPE_AMO_ADD_P1_REQ      =BvConst(44, L2_MSG_WIDTH);
  // auto MSG_TYPE_AMO_AND_P1_REQ      =BvConst(45, L2_MSG_WIDTH);
  // auto MSG_TYPE_AMO_OR_P1_REQ       =BvConst(46, L2_MSG_WIDTH);
  // auto MSG_TYPE_AMO_XOR_P1_REQ      =BvConst(47, L2_MSG_WIDTH);
  // auto MSG_TYPE_AMO_MAX_P1_REQ      =BvConst(48, L2_MSG_WIDTH);
  // auto MSG_TYPE_AMO_MAXU_P1_REQ     =BvConst(49, L2_MSG_WIDTH);
  // auto MSG_TYPE_AMO_MIN_P1_REQ      =BvConst(50, L2_MSG_WIDTH);
  // auto MSG_TYPE_AMO_MINU_P1_REQ     =BvConst(51, L2_MSG_WIDTH);
  // RISC-V AMO L2-internal phase 2
  // auto MSG_TYPE_AMO_ADD_P2_REQ      =BvConst(52, L2_MSG_WIDTH);
  // auto MSG_TYPE_AMO_AND_P2_REQ      =BvConst(53, L2_MSG_WIDTH);
  // auto MSG_TYPE_AMO_OR_P2_REQ       =BvConst(54, L2_MSG_WIDTH);
  // auto MSG_TYPE_AMO_XOR_P2_REQ      =BvConst(55, L2_MSG_WIDTH);
  // auto MSG_TYPE_AMO_MAX_P2_REQ      =BvConst(56, L2_MSG_WIDTH);
  // auto MSG_TYPE_AMO_MAXU_P2_REQ     =BvConst(57, L2_MSG_WIDTH);
  // auto MSG_TYPE_AMO_MIN_P2_REQ      =BvConst(58, L2_MSG_WIDTH);
  // auto MSG_TYPE_AMO_MINU_P2_REQ     =BvConst(59, L2_MSG_WIDTH);
  // auto MSG_TYPE_LR_REQ              =BvConst(60, L2_MSG_WIDTH);
*/

  //Forward requests from L2 to L15
  auto MSG_TYPE_LOAD_FWD           =BvConst(16, L2_MSG_WIDTH);
  auto MSG_TYPE_STORE_FWD          =BvConst(17, L2_MSG_WIDTH);
  auto MSG_TYPE_INV_FWD            =BvConst(18, L2_MSG_WIDTH);
  //Memory requests from L2 to DRAM
  auto MSG_TYPE_LOAD_MEM           =BvConst(19, L2_MSG_WIDTH);
  auto MSG_TYPE_STORE_MEM          =BvConst(20, L2_MSG_WIDTH);
  //Forward acks from L15 to L2
  auto MSG_TYPE_LOAD_FWDACK        =BvConst(21, L2_MSG_WIDTH);
  auto MSG_TYPE_STORE_FWDACK       =BvConst(22, L2_MSG_WIDTH);
  auto MSG_TYPE_INV_FWDACK         =BvConst(23, L2_MSG_WIDTH);
  //Memory acks from memory to L2
  auto MSG_TYPE_LOAD_MEM_ACK       =BvConst(24, L2_MSG_WIDTH);
  auto MSG_TYPE_STORE_MEM_ACK      =BvConst(25, L2_MSG_WIDTH);
  // auto MSG_TYPE_NC_LOAD_MEM_ACK    =BvConst(26, L2_MSG_WIDTH);
  // auto MSG_TYPE_NC_STORE_MEM_ACK   =BvConst(27, L2_MSG_WIDTH);
  // Acks from L2 to L15
  // auto MSG_TYPE_NODATA_ACK         =BvConst(28, L2_MSG_WIDTH);
  auto MSG_TYPE_DATA_ACK           =BvConst(29, L2_MSG_WIDTH);
  /*
   // auto MSG_TYPE_ERROR              =BvConst(30, L2_MSG_WIDTH);
   // auto MSG_TYPE_INTERRUPT          =BvConst(33, L2_MSG_WIDTH);
   // Only exist within L2
   // auto MSG_TYPE_L2_LINE_FLUSH_REQ   =BvConst(34, L2_MSG_WIDTH);
   // auto MSG_TYPE_L2_DIS_FLUSH_REQ    =BvConst(35, L2_MSG_WIDTH);
  */

  auto L2_MESI_I   =   BvConst(0, MESI_WIDTH);
  auto L2_MESI_S   =   BvConst(1, MESI_WIDTH);
  auto L2_MESI_E   =   BvConst(2, MESI_WIDTH);
  auto L2_MESI_M   =   BvConst(3, MESI_WIDTH);

  auto L2_INVAL    =   BvConst(0, 2);
  auto L2_DIRTY    =   BvConst(3, 2);
  auto L2_CLEAN    =   BvConst(2, 2);
  auto L2_ERROR    =   BvConst(1, 2);

  // ---------------------------- Intial States -------------------------------- //
  model.AddInit(cur_msg_state == STATE_INVAL);
  model.AddInit(cache_vd == L2_INVAL);
  model.AddInit(cache_state == L2_MESI_I);
  model.AddInit(share_list == BvConst(0, DIR_WIDTH));

  // -------------------------- Cache Operation Condition --------------------------//

  auto request_from_owner = (Ite(cur_msg_state == STATE_INVAL, msg1_source, cur_msg_source) == cache_owner);
  auto tag_hit = (Ite(cur_msg_state == STATE_INVAL, msg1_tag, cur_msg_tag) == cache_tag) & (cache_vd == L2_CLEAN | cache_vd == L2_DIRTY);
  auto evict   = (Ite(cur_msg_state == STATE_INVAL, msg1_tag, cur_msg_tag) != cache_tag) & (cache_vd == L2_CLEAN | cache_vd == L2_DIRTY);
  auto empty   = (cache_vd == L2_INVAL);
  auto req_done = (tag_hit == b1) & ( (cache_state != L2_MESI_E) | (request_from_owner));


  // ------------------------------ INSTRUCTIONS ---------------------------------- //

  // L2 fetch function -- what corresponds to instructions on L2 NOC1/NOC3 interface
  model.SetFetch( lConcat({msg1_type, cur_msg_type, cur_msg_state}) );
  // Valid instruction: what means to have valid command (valid = 1)
  model.SetValid( BoolConst(true) );

  // add instructions

  // ****************** //
  // pipe1 for msg1     //
  // L1.5 to L2         //
  // ****************** //

  // LOAD_REQ
  {
    auto instr = model.NewInstr("LOAD_REQ");

    instr.SetDecode( ((msg1_type == MSG_TYPE_LOAD_REQ)    & (cur_msg_state == STATE_INVAL))
                  | ((cur_msg_type == MSG_TYPE_LOAD_REQ) & (cur_msg_state == STATE_PENDING)) );

    // mshr info
    instr.SetUpdate(cur_msg_type  , Ite(req_done | (cur_msg_state == STATE_PENDING), cur_msg_type  , msg1_type));
    instr.SetUpdate(cur_msg_tag   , Ite(req_done | (cur_msg_state == STATE_PENDING), cur_msg_tag   , msg1_tag));
    instr.SetUpdate(cur_msg_source, Ite(req_done | (cur_msg_state == STATE_PENDING), cur_msg_source, msg1_source));

    instr.SetUpdate(cur_msg_state, Ite(req_done, STATE_INVAL, STATE_WAIT) );
    
    // Only when data returns from memory(tag_hit), the cache state will be updated. I->E
    // For other read cases, the cache state won't change
    instr.SetUpdate(cache_state, Ite(tag_hit == b1, Ite(cache_state == L2_MESI_I, L2_MESI_E, cache_state), cache_state));
    instr.SetUpdate(cache_owner, Ite(req_done, 
                                  Ite(cache_state == L2_MESI_I, Ite(cur_msg_state == STATE_INVAL, msg1_source, cur_msg_source), cache_owner), 
                                  Ite(evict == b1 & cache_state == L2_MESI_S, unknown_range(0,63), cache_owner)) );
    // updating cache_vd, so that way_selection will be correct
    instr.SetUpdate(cache_vd, Ite(evict == b1, Ite(cache_state == L2_MESI_I, L2_INVAL, cache_vd), cache_vd));
    instr.SetUpdate(cache_tag, cache_tag);
    // Send out Coherence Request  
    instr.SetUpdate(share_list, Ite(req_done & cache_state == L2_MESI_S, 
                                 (share_list | BvConst(1,DIR_WIDTH) << Ite(cur_msg_state == STATE_INVAL, msg1_source, cur_msg_source).ZExt(DIR_WIDTH)), share_list));

    // Send out Coherence Request
    instr.SetUpdate(msg2_type, Ite(evict == b1, Ite(cache_state == L2_MESI_I, Ite(cache_vd == L2_DIRTY, 
                                                                              MSG_TYPE_STORE_MEM, 
                                                                              MSG_TYPE_LOAD_MEM), // shuld also STORE_MEM if data is dirty
                                                Ite(cache_state == L2_MESI_S, MSG_TYPE_INV_FWD, 
                                                                              MSG_TYPE_STORE_FWD)), // cache_state = E
                               Ite(empty == b1,                               MSG_TYPE_LOAD_MEM,
                               // tag_hit
                               Ite(cache_state == L2_MESI_E, Ite(request_from_owner, MSG_TYPE_DATA_ACK, 
                                                                                     MSG_TYPE_LOAD_FWD), 
                                                                                     MSG_TYPE_DATA_ACK))));
  }

  // STORE_REQ
  {
    auto instr = model.NewInstr("STORE_REQ");

    instr.SetDecode( ((msg1_type == MSG_TYPE_STORE_REQ)    & (cur_msg_state == STATE_INVAL))
                  | ((cur_msg_type == MSG_TYPE_STORE_REQ)  & (cur_msg_state == STATE_PENDING)) );

    auto req_done = (tag_hit == b1) & ( (cache_state != L2_MESI_E) | (request_from_owner)) & (cache_state != L2_MESI_S);

    // mshr info
    instr.SetUpdate(cur_msg_type  , Ite(req_done | (cur_msg_state == STATE_PENDING), cur_msg_type  , msg1_type));
    instr.SetUpdate(cur_msg_tag   , Ite(req_done | (cur_msg_state == STATE_PENDING), cur_msg_tag   , msg1_tag));
    instr.SetUpdate(cur_msg_source, Ite(req_done | (cur_msg_state == STATE_PENDING), cur_msg_source, msg1_source));
    instr.SetUpdate(cur_msg_state, Ite(req_done, STATE_INVAL, STATE_WAIT) );
    
    instr.SetUpdate(cache_state, Ite(tag_hit == b1, Ite(cache_state == L2_MESI_I, L2_MESI_E, cache_state), cache_state));
    instr.SetUpdate(cache_vd, Ite(evict == b1, Ite(cache_state == L2_MESI_I, L2_INVAL, cache_vd), cache_vd));
    instr.SetUpdate(cache_tag, cache_tag);
    instr.SetUpdate(cache_owner, Ite(req_done, 
                                  Ite(cache_state == L2_MESI_I, Ite(cur_msg_state == STATE_INVAL, msg1_source, cur_msg_source), cache_owner),
                                  Ite(cache_state == L2_MESI_S, unknown_range(0,63), cache_owner)) );

    // Sharer list
    instr.SetUpdate(share_list, share_list);

    instr.SetUpdate(msg2_type, Ite(evict == b1, Ite(cache_state == L2_MESI_I, Ite(cache_vd == L2_DIRTY, 
                                                                              MSG_TYPE_STORE_MEM, 
                                                                              MSG_TYPE_LOAD_MEM),
                                                Ite(cache_state == L2_MESI_S, MSG_TYPE_INV_FWD, 
                                                                              MSG_TYPE_STORE_FWD)), // cache_state = E
                               Ite(empty == b1,                               MSG_TYPE_LOAD_MEM,
                               // tag_hit
                               Ite(cache_state == L2_MESI_E, Ite(request_from_owner, MSG_TYPE_DATA_ACK, 
                                                                                     MSG_TYPE_STORE_FWD),
                               Ite(cache_state == L2_MESI_S, MSG_TYPE_INV_FWD, 
                                                             MSG_TYPE_DATA_ACK)))));
  }
}
