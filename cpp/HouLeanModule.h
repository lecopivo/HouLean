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
std::vector<void *> lib_handles = {};

std::vector<std::string> libraries_to_load =
  {"libleanshared.so", "libLake.so", "libStd.so", "libQq.so", "libAesop.so", "libCli.so", "libProofWidgets.so", "libImportGraph.so","libMathlib.so", "libLeanColls.so", "libSciLean.so"};

// loads if not already loaded
int loadLean(){

  const std::filesystem::path dir = UT_EnvControl::getString(ENV_HOUDINI_USER_PREF_DIR);

  lib_handles.resize(0);

  for (auto const& libName : libraries_to_load){
    void * lean_lib_handle = nullptr;
    auto leanshared_path = dir;
    leanshared_path.append("lib").append(libName);
    if(!lean_lib_handle){
      lean_lib_handle = dlopen(leanshared_path.c_str(), RTLD_GLOBAL | RTLD_NOW);
      if(!lean_lib_handle){
        std::cerr << "Failed to load " << libName << ": " << dlerror() << std::endl;
        return 1;
      }else{
        // std::cout << "Loaded: " << libName << std::endl;
        lib_handles.push_back(lean_lib_handle);
      }
    }
  }

  return 0;
}

int closeLean(){
  const std::filesystem::path dir = UT_EnvControl::getString(ENV_HOUDINI_USER_PREF_DIR);

  for(int i=0;i<lib_handles.size();i++){
    int j = lib_handles.size() - i - 1;
    auto lean_lib_handle = lib_handles[j];
    
    if(lean_lib_handle){
      int r = dlclose(lean_lib_handle);
      if(r){
        std::cerr << "Failed to close " << libraries_to_load[j] <<": " << dlerror() << std::endl;
        return r;
      }else{
        auto leanshared_path = dir;
        leanshared_path.append("lib").append(libraries_to_load[j]);
        // std::cout << "Closed " << libraries_to_load[j] << std::endl;
      }
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
  std::string m_module_path;
  
  lean_object* (*initialize_Main)(uint8_t, lean_object*);
  lean_object* (*_lean_main)(lean_object*);
  void         (*lean_initialize_runtime_module)();
  void         (*lean_io_mark_end_initialization)();
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

      m_module_path = module_path;
      module_handle = dlopen(module_path, RTLD_LOCAL | RTLD_NOW);

      // std::cout << "Loaded: " << module_path << std::endl;

      if (!module_handle) {
        std::stringstream ss;
        ss << "Error: Library failed to load!" << std::endl;
        ss << "Error Message:" << std::endl << dlerror() << std::endl;
        err = ss.str();
	this->compile_time = 0.0;
	return 0;
      }

      lean_initialize_runtime_module = (void (*)())dlsym(lib_handles[0], "lean_initialize_runtime_module");
      lean_io_mark_end_initialization = (void (*)())dlsym(lib_handles[0], "lean_io_mark_end_initialization");
      initialize_Main = (lean_object* (*)(uint8_t, lean_object*))dlsym(module_handle, "initialize_Main");
      _lean_main = (lean_object* (*)(lean_object*))dlsym(module_handle, "l_run");
      mk_sop_context = (lean_object* (*)(void*))dlsym(module_handle, "mk_sop_context");
      houlean_sop_result_is_ok = (uint8_t (*)(lean_object*, lean_object*))dlsym(module_handle, "houlean_sop_result_is_ok");
      houlean_sop_result_error_msg = (lean_object* (*)(lean_object*, lean_object*))dlsym(module_handle, "houlean_sop_result_error_msg");
      
      if (!initialize_Main) {

	  std::string initMainName = "initialize_Wrangles_";
	  initMainName.append(module_name).append("_Main");
	  
	  initialize_Main = (lean_object* (*)(uint8_t,lean_object*))dlsym(module_handle, initMainName.c_str());
      }

      if (!lean_initialize_runtime_module ||
          !lean_io_mark_end_initialization ||
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
      uint8_t buildin = 1;
      lean_object * res = initialize_Main(buildin, lean_io_mk_world());
      if (lean_io_result_is_ok(res)){

          // std::cout << "Initialization of main succesfull!" << std::endl;
          // lean_dec_ref(res);
      }else{
          err = "Initialization of main failed!";
          std::cerr << "Initialization of main failed!" << std::endl;
          return 0;
      }
      lean_io_mark_end_initialization();

      // this causes crash upon recompilation and realod of lean library
      // lean_dec_ref(res);

      this->compile_time = compile_time;
    }

    return 1;
  }

    int callMain(double time, GU_Detail * geo, std::vector<GU_Detail const*> inputs, std::string& err){
    if(module_handle && _lean_main){
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
    } else {
      err = "lean code not correctly initialized";
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
