#include "CppClass.h"

#include "SopContext.h"
#include "SopCApi.h"

extern "C" LEAN_EXPORT lean_object* houlean_time(lean_object* context){
  auto ctx = to_cppClass<SopContext>(context);
  return houlean_sop_mk_float(ctx->time, context);
}

extern "C" LEAN_EXPORT lean_object* houlean_npoints(size_t geo, lean_object* context){
  auto ctx = to_cppClass<SopContext>(context);
  if(geo < ctx->ref_geo.size()){
    return houlean_sop_mk_size_t(ctx->ref_geo[geo]->getNumPoints(), context);
  }else{
    return houlean_sop_mk_size_t(0, context);
  }
}



