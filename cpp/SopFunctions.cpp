#include <iostream>

#include <GA/GA_Handle.h>
#include <GA/GA_Types.h>
#include <GU/GU_RayIntersect.h>

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
    std::stringstream ss;
    ss << "Invalid attribute " << attr << std::endl;
    return houlean_sop_mk_error(nullptr, lean_mk_string(ss.str().c_str()), context);
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

extern "C" lean_object *houlean_ro_handle_f(size_t geoId, b_lean_obj_arg _attr, uint8_t _owner, lean_object* context){

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

  auto ctx = houlean_lean_to_external<SopContext>(context);
  if (geoId >= ctx->ref_geo.size() || ctx->ref_geo[geoId] == nullptr){
    std::stringstream ss;
    ss << "Invalid input geometry " << geoId << std::endl;
    return houlean_sop_mk_error(nullptr, lean_mk_string(ss.str().c_str()), context);
  }
  auto geo = ctx->ref_geo[geoId];
  auto handle = new GA_ROHandleF(geo, owner, attr);

  // houlean_sop
  if(handle->isValid()){
    return houlean_sop_mk_value(nullptr, houlean_external_to_lean(handle), context);
  }else{
    std::stringstream ss;
    ss << "Invalid attribute " << attr << std::endl;
    return houlean_sop_mk_error(nullptr, lean_mk_string(ss.str().c_str()), context);
  }
}

extern "C" double houlean_ro_handle_f_get(b_lean_obj_arg _handle, size_t idx){

  // auto ctx    = houlean_lean_to_external<SopContext>(context);
  auto handle = houlean_lean_to_external<GA_ROHandleF>(_handle);
  const GA_Detail & geo = handle->getAttribute()->getDetail();

  return handle->get(geo.pointOffset(idx));
}


extern "C" double houlean_ro_handle_f_surface_get(b_lean_obj_arg _handle, size_t primIdx, double u, double v, double w){

  auto handle = houlean_lean_to_external<GA_ROHandleF>(_handle);
  
  auto owner  = handle->getAttribute()->getOwner();

  const GA_Detail & geo = handle->getAttribute()->getDetail();
  
  // todo: check GAisValid(primIdx)
  auto prim = reinterpret_cast<const GEO_Primitive*>(geo.getPrimitiveByIndex(primIdx));
  
  UT_Array<GA_Offset> offsetArray;
  UT_FloatArray weightArray;
  prim->computeInteriorPointWeights(offsetArray, weightArray, u, v, w);
   
  float result = 0;
  for (exint i = 0; i < offsetArray.size(); ++i){
    GA_Offset offset = offsetArray(i);
    if (owner == GA_ATTRIB_POINT){
       offset = geo.vertexPoint(offset);
    }
    result += weightArray(i) * handle->get(offset);
  }

  return result;
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
    std::stringstream ss;
    ss << "Invalid attribute " << attr << std::endl;
    return houlean_sop_mk_error(nullptr, lean_mk_string(ss.str().c_str()), context);
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

extern "C" lean_object *houlean_ro_handle_v3(size_t geoId, b_lean_obj_arg _attr, uint8_t _owner, lean_object* context){

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

  auto ctx = houlean_lean_to_external<SopContext>(context);
  if (geoId >= ctx->ref_geo.size() || ctx->ref_geo[geoId] == nullptr){
    std::stringstream ss;
    ss << "Invalid input geometry " << geoId << std::endl;
    return houlean_sop_mk_error(nullptr, lean_mk_string(ss.str().c_str()), context);
  }
  auto geo = ctx->ref_geo[geoId];
  auto handle = new GA_ROHandleV3(geo, owner, attr);

  // houlean_sop
  if(handle->isValid()){
    return houlean_sop_mk_value(nullptr, houlean_external_to_lean(handle), context);
  }else{
    std::stringstream ss;
    ss << "Invalid attribute " << attr << std::endl;
    return houlean_sop_mk_error(nullptr, lean_mk_string(ss.str().c_str()), context);
  }
}

extern "C" lean_object* houlean_ro_handle_v3_get(b_lean_obj_arg _handle, size_t idx){

  auto handle = houlean_lean_to_external<GA_ROHandleV3>(_handle);
  const GA_Detail & geo = handle->getAttribute()->getDetail();

  auto vec = handle->get(geo.pointOffset(idx));

  return houlean_vec3_mk(vec.x(), vec.y(), vec.z());
}

extern "C" lean_object* houlean_ro_handle_v3_surface_get(b_lean_obj_arg _handle, size_t primIdx, double u, double v, double w){

  auto handle = houlean_lean_to_external<GA_ROHandleV3>(_handle);
  
  auto owner  = handle->getAttribute()->getOwner();

  const GA_Detail & geo = handle->getAttribute()->getDetail();
  
  // todo: check GAisValid(primIdx)
  auto prim = reinterpret_cast<const GEO_Primitive*>(geo.getPrimitiveByIndex(primIdx));
  
  UT_Array<GA_Offset> offsetArray;
  UT_FloatArray weightArray;
  prim->computeInteriorPointWeights(offsetArray, weightArray, u, v, w);
  
  UT_Vector3F result = {0,0,0};
  for (exint i = 0; i < offsetArray.size(); ++i){
    GA_Offset offset = offsetArray(i);
    if (owner == GA_ATTRIB_POINT){
       offset = geo.vertexPoint(offset);
    }
    result += weightArray(i) * handle->get(offset);
  }

  return houlean_vec3_mk(result.x(), result.y(), result.z());
}


extern "C" lean_object* houlean_sop_get_gu_ray_intersect(size_t geoId, lean_object* context){

  auto ctx = houlean_lean_to_external<SopContext>(context);

  if (geoId >= ctx->ref_geo.size() || ctx->ref_geo[geoId] == nullptr){
    std::stringstream ss;
    ss << "Invalid input geometry " << geoId << std::endl;
    return houlean_sop_mk_error(nullptr, lean_mk_string(ss.str().c_str()), context);
  }

  auto geo = ctx->ref_geo[geoId];

  auto ri = new GU_RayIntersect(geo);
 
  if(ri != nullptr){
    return houlean_sop_mk_value(nullptr, houlean_external_to_lean(ri), context);
  }else{
    std::stringstream ss;
    ss << "faild building GURayIntersect";
    return houlean_sop_mk_error(nullptr, lean_mk_string(ss.str().c_str()), context);
  }
}



extern "C" double houlean_sop_gu_ray_intersect_dist(b_lean_obj_arg _ri, b_lean_obj_arg vec){

  auto ri = houlean_lean_to_external<GU_RayIntersect>(_ri);

  float x = houlean_vec3_x(vec);
  float y = houlean_vec3_y(vec);
  float z = houlean_vec3_z(vec);

  GU_MinInfo minInfo; //(1E18F, 1e-14F,10);
  ri->minimumPoint({x,y,z}, minInfo);

  return sqrt(minInfo.d);
}


extern "C" lean_object* houlean_sop_gu_ray_intersect_closest_point(b_lean_obj_arg _ri, b_lean_obj_arg vec){

  auto ri = houlean_lean_to_external<GU_RayIntersect>(_ri);

  float x = houlean_vec3_x(vec);
  float y = houlean_vec3_y(vec);
  float z = houlean_vec3_z(vec);
  
  GU_MinInfo minInfo; //(1E18F, 1e-14F,10);
  ri->minimumPoint({x,y,z}, minInfo);

  UT_Vector4 pos = {0,0,0,0};
  minInfo.prim->evaluateInteriorPoint(pos,minInfo.u1,minInfo.v1);

  pos.dehomogenize();
  return houlean_vec3_mk(pos.x(), pos.y(), pos.z());
}


extern "C" lean_object* houlean_sop_gu_ray_intersect_closest_surface_point(b_lean_obj_arg _ri, b_lean_obj_arg vec){

  auto ri = houlean_lean_to_external<GU_RayIntersect>(_ri);

  float x = houlean_vec3_x(vec);
  float y = houlean_vec3_y(vec);
  float z = houlean_vec3_z(vec);
  
  GU_MinInfo minInfo;
  ri->minimumPoint({x,y,z}, minInfo);

  size_t primIdx = minInfo.prim->getMapIndex();

  lean_object * p = lean_alloc_ctor(0, 0, sizeof(size_t)*1 + sizeof(double)*3);
  lean_ctor_set_usize(p, 0, primIdx);
  lean_ctor_set_float(p, sizeof(void*)*1, (double)minInfo.u1);
  lean_ctor_set_float(p, sizeof(void*)*1 + sizeof(double), (double)minInfo.v1);
  lean_ctor_set_float(p, sizeof(void*)*1 + sizeof(double)*2, (double)minInfo.w1);
  return p;
}
