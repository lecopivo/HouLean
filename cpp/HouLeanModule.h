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
      // std::cerr << "Failed to load libleanshared.so: " << dlerror() << std::endl;
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
      // std::cerr << "Failed to close libleanshared: " << dlerror() << std::endl;
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
  void (*lean_initialize_runtime_module)();
  lean_object* (*mk_sop_context)(void*);

  ~LeanModule(){
    if(module_handle != nullptr){
      dlclose(module_handle);
    }
  }

  int realoadModule(const char *module_path, double compile_time) {
    if(module_handle == nullptr || compile_time != this->compile_time){
      // std::cout << "Reloading Lean Module!" << std::endl;
      // close all existing libraries
      closeAllModules();

      reloadLean();

      // std::cout << "Current version: " << this->compile_time << std::endl;
      // std::cout << "New version: " << compile_time << std::endl;

      module_handle = dlopen(module_path, RTLD_LOCAL | RTLD_NOW);

      if (!module_handle) {
	// std::cout << "Error: Library failed to load!" << std::endl;
	// std::cout << "Error Message:" << std::endl << dlerror() << std::endl;
	this->compile_time = 0.0;
	return 0;
      }

      lean_initialize_runtime_module = (void (*)())dlsym(lean_lib_handle, "lean_initialize_runtime_module");
      initialize_Main = (lean_object* (*)(lean_object*))dlsym(module_handle, "initialize_Main");
      _lean_main = (lean_object* (*)(lean_object*))dlsym(module_handle, "l_run");
      mk_sop_context = (lean_object* (*)(void*))dlsym(module_handle, "mk_sop_context");

      if (!lean_initialize_runtime_module ||
	  !initialize_Main ||
	  !_lean_main ||
	  !mk_sop_context){

	// std::cout << "Error Message: " << dlerror() << std::endl;
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

  void callMain(double time, GU_Detail * geo){
    if(module_handle && _lean_main){

      // std::cout << "Running main" << std::endl;

      // lean_object* res;
      // res =
      auto sopContext = new SopContext;
      sopContext->time = time;
      sopContext->geo = geo;
      sopContext->ref_geo.push_back(geo);
      
      _lean_main(mk_sop_context((void*)sopContext));
      
      // this causes crash upon recompilation and realod of lean library
      // lean_dec_ref(res);
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
