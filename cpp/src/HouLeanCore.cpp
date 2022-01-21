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


std::mutex contextMutex;

int currentId;

struct HouLeanContext{

  double time = 0.0;

  GU_Detail * outGeo;
  const GU_Detail* inGeo[4];

} houLeanContext;

void closeAllModules();

struct LeanModule{
  double compile_time = 0.0f;
  
  void * module_handle = nullptr;
  
  lean_object* (*initialize_Main)(lean_object*);
  lean_object* (*_lean_main)(lean_object*);
  void (*lean_initialize_runtime_module)();

  void (*lean_inc)(lean_object*);
  void (*lean_dec)(lean_object*);

  lean_object* (*io_float)(double, lean_object*);
  lean_object* (*io_uint64)(uint64_t, lean_object*);
  lean_object* (*io_vec3)(double, double, double, lean_object*);
  lean_object* (*io_unit)(lean_object*);

  double (*vec3_x)(lean_object*);
  double (*vec3_y)(lean_object*);
  double (*vec3_z)(lean_object*);

  // std::function<void(lean_object*)> lean_dec_ref_cold;

  ~LeanModule(){
    if(module_handle != nullptr){
      dlclose(module_handle);
    }
  }

  int realoadModule(const char* modulePath, double compile_time) {
    if(module_handle == nullptr || compile_time > this->compile_time){
      std::cout << "Reloading Lean Module!" << std::endl;
      // close all existing libraries
      closeAllModules();

      std::cout << "Current version: " << this->compile_time << std::endl;
      std::cout << "New version: " << compile_time << std::endl;

      module_handle = dlopen(modulePath, RTLD_LOCAL | RTLD_NOW);

      if (!module_handle) {
	std::cout << "Error: Library failed to load!" << std::endl;
	std::cout << "Error Message:" << std::endl << dlerror() << std::endl;
	this->compile_time = 0.0;
	return 0;
      }

      lean_initialize_runtime_module = (void (*)())dlsym(module_handle, "lean_initialize_runtime_module");
      initialize_Main = (lean_object* (*)(lean_object*))dlsym(module_handle, "initialize_Main");
      _lean_main = (lean_object* (*)(lean_object*))dlsym(module_handle, "_lean_main");

      // These must be loaded from the main libleanshared.so
      // lean_inc = (void (*)(lean_object*))dlsym(module_handle, "lean_inc");
      // lean_dec = (void (*)(lean_object*))dlsym(module_handle, "lean_dec");

      io_float = (lean_object* (*)(double, lean_object*))dlsym(module_handle, "io_float");
      io_uint64 = (lean_object* (*)(uint64_t, lean_object*))dlsym(module_handle, "io_uint64");
      io_vec3 = (lean_object* (*)(double, double, double, lean_object*))dlsym(module_handle, "io_vec3");
      io_unit = (lean_object* (*)(lean_object*))dlsym(module_handle, "io_unit");

      vec3_x = (double (*)(lean_object*))dlsym(module_handle, "vec3_x");
      vec3_y = (double (*)(lean_object*))dlsym(module_handle, "vec3_y");
      vec3_z = (double (*)(lean_object*))dlsym(module_handle, "vec3_z");

      if (!lean_initialize_runtime_module ||
	  !initialize_Main ||
	  !_lean_main ||
	  // !lean_inc || !lean_dec ||
	  !io_float || !io_uint64 || !io_vec3 || !io_unit ||
	  !vec3_x || !vec3_y || !vec3_z){

	std::cout << "Error Message:" << std::endl << dlerror() << std::endl;
	module_handle = nullptr;
	this->compile_time = 0.0;
	return 0;
      }

      std::cout << "Initializing module" << std::endl;

      lean_object* res;
      lean_initialize_runtime_module();

      std::cout << "Initializing main" << std::endl;
      
      res = initialize_Main(lean_io_mk_world());

      // this causes crash upon recompilation and realod of lean library
      /* lean_dec_ref(res); */

      this->compile_time = compile_time;
    }

    return 1;  
  }

  void callMain(){
    if(module_handle && _lean_main){

      std::cout << "Running main" << std::endl;

      lean_object* res;
      res = _lean_main(lean_io_mk_world());
      
      // this causes crash upon recompilation and realod of lean library
      /* lean_dec_ref(res);  */
    }
  }

};

// use OP_Node->getUniqueId() as a key
std::unordered_map<int, LeanModule> modules;

void closeAllModules(){
  
  for(auto& [id, m] : modules){
    
    if(m.module_handle){
      
      dlclose(m.module_handle);
      m.module_handle = nullptr;
    }
  }
}

extern "C" uint64_t houlean_foo(uint64_t n){
  std::cout << "Calling foo with: " << n << std::endl;
  return 2*n;
}

extern "C" lean_object* houlean_npoints(uint64_t geo, lean_object* io) {
  uint64_t n = 0;
  if(geo < 4 && houLeanContext.inGeo[geo]){
    n = houLeanContext.inGeo[geo]->getNumPoints();
  }
  return modules[currentId].io_uint64(n, io);
}

extern "C" lean_object* houlean_time(lean_object* io) { return modules[currentId].io_float(houLeanContext.time, io); }

extern "C" lean_object* houlean_getpoint_v3(uint64_t geoId, lean_object * attrib, uint64_t ptnum, lean_object* io) {

  const char * attr = lean_to_string(attrib)->m_data;

  if(geoId < 4 && houLeanContext.inGeo[geoId]){
    auto geo = houLeanContext.inGeo[geoId];
    GA_ROHandleV3D handle(geo, GA_ATTRIB_POINT, attr);
    if(handle.isValid()){
      auto off = geo->pointOffset(ptnum);
      auto v = handle.get(off);
      return modules[currentId].io_vec3(v.x(), v.y(), v.z(), io);
    }
  }
  
  return modules[currentId].io_vec3(0.0, 0.0, 0.0, io);
}

extern "C" lean_object* houlean_setpoint_v3(lean_object * attrib, uint64_t ptnum, double x, double y, double z, lean_object* io) {

  const char * attr = lean_to_string(attrib)->m_data;

  auto geo = houLeanContext.outGeo;
  GA_RWHandleV3D handle(geo, GA_ATTRIB_POINT, attr);
  if(handle.isValid()){
    auto off = geo->pointOffset(ptnum);
    handle.set(off, {x, y ,z});
  }
  
  return modules[currentId].io_unit(io);
}

extern "C" lean_object* houlean_getpoint_r(uint64_t geoId, lean_object * attrib, uint64_t ptnum, lean_object* io) {

  const char * attr = lean_to_string(attrib)->m_data;

  if(geoId < 4 && houLeanContext.inGeo[geoId]){
    auto geo = houLeanContext.inGeo[geoId];
    GA_ROHandleD handle(geo, GA_ATTRIB_POINT, attr);
    if(handle.isValid()){
      auto off = geo->pointOffset(ptnum);
      auto val = handle.get(off);
      return modules[currentId].io_float(val, io);
    }
  }
  
  return modules[currentId].io_float(0.0, io);
}

extern "C" lean_object* houlean_setpoint_r(lean_object * attrib, uint64_t ptnum, double x, lean_object* io) {

  const char * attr = lean_to_string(attrib)->m_data;

  auto geo = houLeanContext.outGeo;
  GA_RWHandleD handle(geo, GA_ATTRIB_POINT, attr);
  if(handle.isValid()){
    auto off = geo->pointOffset(ptnum);
    handle.set(off, x);
  }
  
  return modules[currentId].io_unit(io);
}

extern "C" lean_object* houlean_getdetail_r(uint64_t geoId, lean_object * attrib, lean_object* io) {

  const char * attr = lean_to_string(attrib)->m_data;

  if(geoId < 4 && houLeanContext.inGeo[geoId]){
    auto geo = houLeanContext.inGeo[geoId];
    GA_ROHandleD handle(geo, GA_ATTRIB_DETAIL, attr);

    if(handle.isValid())
      return modules[currentId].io_float(handle.get(0), io);
  }

  return modules[currentId].io_float(0.0, io);
}

extern "C" lean_object* houlean_setdetail_r(lean_object * attrib, double x, lean_object* io) {

  const char * attr = lean_to_string(attrib)->m_data;

  auto geo = houLeanContext.outGeo;
  GA_RWHandleD handle(geo, GA_ATTRIB_DETAIL, attr);
  if(handle.isValid()){
    handle.set(0, x);
  }
  
  return modules[currentId].io_unit(io);
}


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
