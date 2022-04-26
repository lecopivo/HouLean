#pragma once
#include <OP/OP_Operator.h>
#include <iostream>
#include <set>

#include <dlfcn.h>
#include <stddef.h>

#import <lean/lean.h>
#include <unordered_map>

#include <UT/UT_EnvControl.h>

#include <filesystem>

// TODO: split to *.h and *.cpp file
void *lean_lib_handle = nullptr;
void *houlean_lib_handle = nullptr;
void *houlean_core_lib_handle = nullptr;

// loads if not already loaded
int loadHouLean(){

  const std::filesystem::path dir = UT_EnvControl::getString(ENV_HOUDINI_USER_PREF_DIR);

  auto leanshared_path = dir;
  leanshared_path.append("lib").append("libleanshared.so");
  if(!lean_lib_handle){
    lean_lib_handle = dlopen(leanshared_path.c_str(),
			     RTLD_GLOBAL | RTLD_NOW);
    if(!lean_lib_handle){
      std::cerr << "Failed loading libleanshared.so: " << dlerror() << std::endl;
      return 1;
    }else
      std::cout << "Loaded: libleanshared.so!" << std::endl;

  }

  // auto houlean_core_path = dir;
  // houlean_core_path.append("dso").append("libHouLeanCore.so");
  // if(!houlean_lib_handle){
  //   houlean_core_lib_handle = dlopen(houlean_core_path.c_str(),
  // 				RTLD_GLOBAL | RTLD_NOW);
  //   if(!houlean_core_lib_handle){
  //     std::cerr << dlerror() << std::endl;
  //     return 1;
  //   } else
  //     std::cout << "Loaded: libHouLeanCore.so!" << std::endl;
  // }

  // auto houlean_path = dir;
  // houlean_path.append("lib").append("libHouLean.so");
  // std::cout << "HouLean lib path: " <<  houlean_path << std::endl;
  // if(!houlean_lib_handle){
  //   houlean_lib_handle = dlopen(houlean_path.c_str(),
  // 				RTLD_GLOBAL | RTLD_NOW);
  //   if(!houlean_lib_handle){
  //     std::cerr << dlerror() << std::endl;
  //     return 1;
  //   }else{
  //     std::cout << "Loaded: libHouLean.so!" << std::endl;
  //   }
  // }

  return 0;
}

int closeHouLean(){
  
  // if(houlean_lib_handle){
  //   int r = dlclose(houlean_lib_handle);
  //   if(r){
  //     std::cerr << dlerror() << std::endl;
  //     return r;
  //   }
  // }

  // if(houlean_core_lib_handle){
  //   int r = dlclose(houlean_core_lib_handle);
  //   if(r){
  //     std::cerr << dlerror() << std::endl;
  //     return r;
  //   }
  // }
  
  if(lean_lib_handle){
    int r = dlclose(lean_lib_handle);
    lean_lib_handle = nullptr;
    if(r){
      std::cerr << "Failed closing libleanshared.so: " << dlerror() << std::endl;
      return r;
    }else{
      lean_lib_handle = nullptr;
      std::cout << "Succesfully closed libleanshared.so!" << std::endl;
    }
  }
  
  return 0;
}

// cloases and loads HouLean and Lean 
int reloadHouLean(){
  int r = closeHouLean();
  if(r)
    return r;
  return loadHouLean();
}

void closeAllModules();

struct LeanModule{
  double compile_time = 0.0f;
  
  void * module_handle = nullptr;
  
  lean_object* (*initialize_Main)(lean_object*);
  lean_object* (*_lean_main)(lean_object*);
  void (*lean_initialize_runtime_module)();

  // void (*lean_inc)(lean_object*);
  // void (*lean_dec)(lean_object*);

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

  int realoadModule(const char *module_path, double compile_time) {
    if(module_handle == nullptr || compile_time > this->compile_time){
      std::cout << "Reloading Lean Module!" << std::endl;
      // close all existing libraries
      closeAllModules();

      reloadHouLean();

      std::cout << "Current version: " << this->compile_time << std::endl;
      std::cout << "New version: " << compile_time << std::endl;

      module_handle = dlopen(module_path, RTLD_GLOBAL | RTLD_NOW);

      if (!module_handle) {
	std::cout << "Error: Library failed to load!" << std::endl;
	std::cout << "Error Message:" << std::endl << dlerror() << std::endl;
	this->compile_time = 0.0;
	return 0;
      }

      lean_initialize_runtime_module = (void (*)())dlsym(lean_lib_handle, "lean_initialize_runtime_module");
      initialize_Main = (lean_object* (*)(lean_object*))dlsym(module_handle, "initialize_Main");
      _lean_main = (lean_object* (*)(lean_object*))dlsym(module_handle, "_lean_main");

      // These must be loaded from the main libleanshared.so
      // lean_inc = (void (*)(lean_object*))dlsym(module_handle, "lean_inc");
      // lean_dec = (void (*)(lean_object*))dlsym(module_handle, "lean_dec");

      // io_float = (lean_object* (*)(double, lean_object*))dlsym(module_handle, "io_float");
      // io_uint64 = (lean_object* (*)(uint64_t, lean_object*))dlsym(module_handle, "io_uint64");
      // io_vec3 = (lean_object* (*)(double, double, double, lean_object*))dlsym(module_handle, "io_vec3");
      // io_unit = (lean_object* (*)(lean_object*))dlsym(module_handle, "io_unit");

      // vec3_x = (double (*)(lean_object*))dlsym(module_handle, "vec3_x");
      // vec3_y = (double (*)(lean_object*))dlsym(module_handle, "vec3_y");
      // vec3_z = (double (*)(lean_object*))dlsym(module_handle, "vec3_z");

      // std::cout << "lean_initialize_runtime_module " << lean_initialize_runtime_module << std::endl;
      // std::cout << "initialize_Main " << initialize_Main << std::endl;
      // std::cout << "_lean_main " << _lean_main << std::endl;

      if (!lean_initialize_runtime_module ||
	  !initialize_Main ||
	  !_lean_main){
	  // !lean_inc || !lean_dec ||
	  // !io_float || !io_uint64 || !io_unit ||  !io_vec3 ||
	  // !vec3_x || !vec3_y || !vec3_z){

	std::cout << "Error Message:" << std::endl << dlerror() << std::endl;
	module_handle = nullptr;
	this->compile_time = 0.0;
	return 0;
      }

      std::cout << "Initializing module" << std::endl;

      // lean_object* res;
      lean_initialize_runtime_module();

      std::cout << "Initializing main" << std::endl;
      
      // res = 
	initialize_Main(lean_io_mk_world());

      // this causes crash upon recompilation and realod of lean library
      // lean_dec_ref(res);

      this->compile_time = compile_time;
    }

    return 1;
  }

  void callMain(){
    if(module_handle && _lean_main){

      std::cout << "Running main" << std::endl;

      // lean_object* res;
      // res = 
	_lean_main(lean_io_mk_world());
      
      // this causes crash upon recompilation and realod of lean library
      // lean_dec_ref(res);
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

  // dlclose(houlean_lib_handle);
  // dlclose(lean_lib_handle);
  // houlean_lib_handle = nullptr;
  // lean_lib_handle = nullptr;
}
