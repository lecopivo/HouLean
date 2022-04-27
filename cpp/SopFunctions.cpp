#include <iostream>

#include <GA/GA_Handle.h>
#include <GA/GA_Types.h>

#include <lean/lean.h>

#include "External.h"
#include "SopContext.h"
#include "SopCApi.h"

extern "C" lean_object* mk_sop_context(void* sopContext){
  return houlean_external_to_lean<SopContext>((SopContext*)sopContext);
}

extern "C" lean_object* houlean_time(lean_object* context){
  auto ctx = houlean_lean_to_external<SopContext>(context);
  return houlean_sop_mk_float(ctx->time, context);
}

extern "C" lean_object* houlean_npoints(size_t geo, lean_object* context){
  auto ctx = houlean_lean_to_external<SopContext>(context);
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

extern "C" lean_object *houlean_rw_handle_f(b_lean_obj_arg _attr, uint8_t _owner, lean_object* context){

  auto attr = lean_string_cstr(_attr);
  auto owner = GA_ATTRIB_INVALID;
  if(houlean_attr_owner_is_detail(_owner))
    owner = GA_ATTRIB_DETAIL;
  if(houlean_attr_owner_is_point(_owner))
    owner = GA_ATTRIB_POINT;
  if(houlean_attr_owner_is_vertex(_owner))
    owner = GA_ATTRIB_VERTEX;
  if(houlean_attr_owner_is_primitive(_owner))
    owner = GA_ATTRIB_PRIMITIVE;

  auto ctx    = houlean_lean_to_external<SopContext>(context);
  auto handle = new GA_RWHandleF(ctx->geo, owner, attr);

  // houlean_sop
  if(handle->isValid()){
    return houlean_sop_mk_value(nullptr, houlean_external_to_lean(handle), context);
  }else{
    std::cout << "Invalid attribute " << attr << std::endl;
    return houlean_sop_mk_error(nullptr, lean_mk_string("Invalid attribute"), context);
  }
}

extern "C" lean_object* houlean_rw_handle_f_get(b_lean_obj_arg _handle, size_t idx, lean_object* context){

  auto ctx    = houlean_lean_to_external<SopContext>(context);
  auto handle = houlean_lean_to_external<GA_RWHandleF>(_handle);

  return houlean_sop_mk_float(handle->get(ctx->geo->pointOffset(idx)), context);
}

extern "C" lean_object* houlean_rw_handle_f_set(b_lean_obj_arg _handle, size_t idx, double val, lean_object* context){

  auto ctx    = houlean_lean_to_external<SopContext>(context);
  auto handle = houlean_lean_to_external<GA_RWHandleF>(_handle);

  handle->set(ctx->geo->pointOffset(idx), val);

  return houlean_sop_mk_unit(context);
}

extern "C" lean_object *houlean_rw_handle_v3(b_lean_obj_arg _attr, uint8_t _owner, lean_object* context){

  auto attr = lean_string_cstr(_attr);
  auto owner = GA_ATTRIB_INVALID;
  if(houlean_attr_owner_is_detail(_owner))
    owner = GA_ATTRIB_DETAIL;
  if(houlean_attr_owner_is_point(_owner))
    owner = GA_ATTRIB_POINT;
  if(houlean_attr_owner_is_vertex(_owner))
    owner = GA_ATTRIB_VERTEX;
  if(houlean_attr_owner_is_primitive(_owner))
    owner = GA_ATTRIB_PRIMITIVE;

  auto ctx    = houlean_lean_to_external<SopContext>(context);
  auto handle = new GA_RWHandleV3(ctx->geo, owner, attr);

  // houlean_sop
  if(handle->isValid()){
    return houlean_sop_mk_value(nullptr, houlean_external_to_lean(handle), context);
  }else{
    std::cout << "Invalid attribute " << attr << std::endl;
    return houlean_sop_mk_error(nullptr, lean_mk_string("Invalid attribute"), context);
  }
}

extern "C" lean_object* houlean_rw_handle_v3_get(b_lean_obj_arg _handle, size_t idx, lean_object* context){

  auto ctx    = houlean_lean_to_external<SopContext>(context);
  auto handle = houlean_lean_to_external<GA_RWHandleV3>(_handle);

  auto vec = handle->get(ctx->geo->pointOffset(idx));

  return houlean_sop_mk_vec3(vec.x(), vec.y(), vec.z(), context);
}

extern "C" lean_object* houlean_rw_handle_v3_set(b_lean_obj_arg _handle, size_t idx, b_lean_obj_arg vec, lean_object* context){

  auto ctx    = houlean_lean_to_external<SopContext>(context);
  auto handle = houlean_lean_to_external<GA_RWHandleV3>(_handle);

  float x = houlean_vec3_x(vec);
  float y = houlean_vec3_y(vec);
  float z = houlean_vec3_z(vec);
  handle->set(ctx->geo->pointOffset(idx), {x,y,z});

  return houlean_sop_mk_unit(context);
}
