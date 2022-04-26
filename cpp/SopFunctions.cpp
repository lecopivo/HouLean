#include "CppClass.h"

#include "SopContext.h"
#include "SopCApi.h"
#include "lean/lean.h"

#include <iostream>

extern "C" lean_object* mk_sop_context(void* sopContext){
  return CppClass_to_lean<SopContext>((SopContext*)sopContext);
}

extern "C" lean_object* houlean_time(lean_object* context){
  auto ctx = to_cppClass<SopContext>(context);
  return houlean_sop_mk_float(ctx->time, context);
}

extern "C" lean_object* houlean_npoints(size_t geo, lean_object* context){
  auto ctx = to_cppClass<SopContext>(context);
  if(geo < ctx->ref_geo.size()){
    return houlean_sop_mk_size_t(ctx->ref_geo[geo]->getNumPoints(), context);
  }else{
    return houlean_sop_mk_size_t(0, context);
  }
}

extern "C" lean_object* houlean_print(b_lean_obj_arg msg, lean_object* context){
  std::cout << lean_string_cstr(msg);
  return houlean_sop_mk_unit(context);
}



