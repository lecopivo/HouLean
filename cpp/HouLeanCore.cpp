#include <iostream> 
#include <mutex>
#include <set>
#include <string>

#include <dlfcn.h>
#include <stddef.h>

#include <GU/GU_Detail.h>
#include <OP/OP_AutoLockInputs.h>
#include <OP/OP_Director.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_Include.h>
#include <UT/UT_DSOVersion.h>

#include <lean/lean.h>

#include <functional>

#include "HouLeanCore.h"
#include "utils/scope_guard.hpp"

#include "HouLeanFunctions.h"

void newSopOperator(OP_OperatorTable *table) {

  table->addOperator(
      new OP_Operator("houlean_core", "HouLean Core",
                      SOP_HouLeanCore::myConstructor,
                      SOP_HouLeanCore::myTemplateList, 1));
}

static PRM_Name prm_names[] = {
    PRM_Name("callback_library", "Callback Library"),
    PRM_Name("compile_time", "Compile Time"),
};

PRM_Template SOP_HouLeanCore::myTemplateList[] = {
    PRM_Template(PRM_FILE, 1, &prm_names[0]),
    PRM_Template(PRM_FLT, 1, &prm_names[1], PRMzeroDefaults),
    PRM_Template(),
};

OP_Node *SOP_HouLeanCore::myConstructor(OP_Network * net,
                                               const char * name,
                                               OP_Operator *op) {
  return new SOP_HouLeanCore(net, name, op);
}

SOP_HouLeanCore::SOP_HouLeanCore(OP_Network * net,
                                               const char * name,
                                               OP_Operator *op)
    : SOP_Node(net, name, op) {
  // Right now I'm assuming that anything can change
  // In fugure, I might bump data IDs base on which attribute handles were
  // created with `getattrib` function

  // mySopFlags.setManagesDataIDs(true);
}

SOP_HouLeanCore::~SOP_HouLeanCore() {}

OP_ERROR
SOP_HouLeanCore::cookMySop(OP_Context &context) {
  // We must lock our inputs before we try to access their geometry.
  // OP_AutoLockInputs will automatically unlock our inputs when we return.
  // NOTE: Don't call unlockInputs yourself when using this!
  OP_AutoLockInputs input_lock_guard(this);
  if (input_lock_guard.lock(context) >= UT_ERROR_ABORT)
    return error();

  // Duplicate incoming geometry
  duplicateSource(0, context);

  // Lock Lean context
  std::lock_guard contextGuard{contextMutex};

  currentId = this->getUniqueId();

  houLeanContext.outGeo = gdp;
  for(int i=0;i<4;i++){
    houLeanContext.inGeo[i] = inputGeo(i);
  }		 

  flags().setTimeDep(true);
  fpreal time = context.getTime();

  houLeanContext.time = time;

  std::cout << "Node unique id: " << this->getUniqueId() << std::endl;
  
  // Load external library
  UT_String callback_library;
  evalString(callback_library, "callback_library", 0, time);
  double compile_time = evalFloat("compile_time", 0, time);

  modules.try_emplace(currentId, LeanModule{});

  if(modules[currentId].realoadModule(callback_library, compile_time)){
    modules[currentId].callMain();
  }

  if (error() >= UT_ERROR_ABORT)
    return error();

  return error();
}
