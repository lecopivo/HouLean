#pragma once
#include <OP/OP_Operator.h>
#include <iostream>
#include <set>

#include <dlfcn.h>
#include <stddef.h>

#include <lean/lean.h>
#include <unordered_map>

#include <UT/UT_EnvControl.h>

#include <filesystem>

// #include "SopCApi.h"
#include "SopContext.h"

// TODO: split to *.h and *.cpp file
void *lean_lib_handle = nullptr;

// loads if not already loaded
int loadLean(){

  const std::filesystem::path dir = UT_EnvControl::getString(ENV_HOUDINI_USER_PREF_DIR);

  auto leanshared_path = dir;
  leanshared_path.append("lib").append("libleanshared.so");
  if(!lean_lib_handle){
    lean_lib_handle = dlopen(leanshared_path.c_str(),
			     RTLD_GLOBAL | RTLD_NOW);
    if(!lean_lib_handle){
      std::cerr << "Failed to load libleanshared.so: " << dlerror() << std::endl;
      return 1;
    }else{
      // std::cout << "Loaded libleanshared.so" << std::endl;
    }
  }

  return 0;
}

int closeLean(){
    
  if(lean_lib_handle){
    int r = dlclose(lean_lib_handle);
    lean_lib_handle = nullptr;
    if(r){
      std::cerr << "Failed to close libleanshared: " << dlerror() << std::endl;
      return r;
    }else{
      // std::cout << "Closed libleanshared.so" << std::endl;
    }
  }
  
  return 0;
}

// cloases and loads HouLean and Lean 
int reloadLean(){
  closeLean();
  loadLean();
  return 0;
}

void closeAllModules();

struct LeanModule{
  double compile_time = 0.0f;
  
  void * module_handle = nullptr;
  
  lean_object* (*initialize_Main)(lean_object*);
  lean_object* (*_lean_main)(lean_object*);
  void         (*lean_initialize_runtime_module)();
  lean_object* (*mk_sop_context)(void*);
  uint8_t      (*houlean_sop_result_is_ok)(lean_object*, lean_object*);
  lean_object* (*houlean_sop_result_error_msg)(lean_object*, lean_object*);

  ~LeanModule(){
    if(module_handle != nullptr){
      dlclose(module_handle);
    }
  }

    int realoadModule(const char *module_path, const char *module_name, double compile_time, std::string& err) {
    if(module_handle == nullptr || compile_time != this->compile_time){
      // std::cout << "Reloading Lean Module!" << std::endl;
      // close all existing libraries
      closeAllModules();

      reloadLean();

      // std::cout << "Current version: " << this->compile_time << std::endl;
      // std::cout << "New version: " << compile_time << std::endl;

      module_handle = dlopen(module_path, RTLD_LOCAL | RTLD_NOW);

      if (!module_handle) {
        std::stringstream ss;
        ss << "Error: Library failed to load!" << std::endl;
        ss << "Error Message:" << std::endl << dlerror() << std::endl;
        err = ss.str();
	this->compile_time = 0.0;
	return 0;
      }

      lean_initialize_runtime_module = (void (*)())dlsym(lean_lib_handle, "lean_initialize_runtime_module");
      initialize_Main = (lean_object* (*)(lean_object*))dlsym(module_handle, "initialize_Main");
      _lean_main = (lean_object* (*)(lean_object*))dlsym(module_handle, "l_run");
      mk_sop_context = (lean_object* (*)(void*))dlsym(module_handle, "mk_sop_context");
      houlean_sop_result_is_ok = (uint8_t (*)(lean_object*, lean_object*))dlsym(module_handle, "houlean_sop_result_is_ok");
      houlean_sop_result_error_msg = (lean_object* (*)(lean_object*, lean_object*))dlsym(module_handle, "houlean_sop_result_error_msg");
      
      if (!initialize_Main) {

	  std::string initMainName = "initialize_Wrangles_";
	  initMainName.append(module_name).append("_Main");
	  
	  initialize_Main = (lean_object* (*)(lean_object*))dlsym(module_handle, initMainName.c_str());
      }

      if (!lean_initialize_runtime_module ||
	  !initialize_Main ||
	  !_lean_main ||
	  !mk_sop_context ||
          !houlean_sop_result_is_ok ||
          !houlean_sop_result_error_msg){

        std::stringstream ss;
        ss << "Loading module functions failed! module: " << module_path << std::endl;
        ss << "Error Message: " << dlerror() << std::endl;
        err = ss.str();
	module_handle = nullptr;
	this->compile_time = 0.0;
	return 0;
      }

      // std::cout << "Initializing module" << std::endl;

      // lean_object* res;
      lean_initialize_runtime_module();

      // std::cout << "Initializing main" << std::endl;
      
      // res =
      initialize_Main(lean_io_mk_world());

      // this causes crash upon recompilation and realod of lean library
      // lean_dec_ref(res);

      this->compile_time = compile_time;
    }

    return 1;
  }

    int callMain(double time, GU_Detail * geo, std::vector<GU_Detail const*> inputs, std::string& err){
    if(module_handle && _lean_main){

      // std::cout << "Running main" << std::endl;

      // lean_object* res;
      // res =
      auto sopContext = new SopContext;
      sopContext->time = time;
      sopContext->geo = geo;
      sopContext->ref_geo = inputs;
      
      auto result = _lean_main(mk_sop_context((void*)sopContext));

      lean_inc(result);
      if (!houlean_sop_result_is_ok(nullptr, result)){
        auto msg = houlean_sop_result_error_msg(nullptr,result);
        char const* m = lean_string_cstr(msg);
        err = m;
        lean_dec(msg);
        return 0;
      }

      return 1;
      // this causes crash upon recompilation and realod of lean library
      // lean_dec_ref(res);
    } else {

      err = "lean code not correctly initialize";
      return 0;
    }
  }

};

std::mutex moduleMutex;
// use OP_Node->getUniqueId() as a key
std::unordered_map<int, LeanModule> modules;

void closeAllModules(){
  
  for(auto& [id, m] : modules){
    
    if(m.module_handle){
      
      dlclose(m.module_handle);
      m.module_handle = nullptr;
    }
  }

  // dlclose(houlean_lib_handle);
  // dlclose(lean_lib_handle);
  // houlean_lib_handle = nullptr;
  // lean_lib_handle = nullptr;
}
