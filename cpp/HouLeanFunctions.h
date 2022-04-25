#pragma once

#include <GA/GA_Types.h>
#include <UT/UT_VectorTypes.h>
#import <lean/lean.h>

#import "HouLeanContext.h"
#import "HouLeanModule.h"

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


extern "C" lean_object* houlean_addpoint(double x, double y, double z, lean_object* io) {

  auto geo = houLeanContext.outGeo;

  auto off = geo->appendPoint();
  geo->setPos3(off, x, y, z);

  return modules[currentId].io_unit(io);
}


template<class T>
static void CppClass_finalize(void * obj){
  delete static_cast<T*>(obj);
}

template<class T>
static void CppClass_foreach(void *, b_lean_obj_arg){
  // do nothing since `S` does not contain nested Lean objects
}

template<class T>
static lean_external_class *CppClass_class = nullptr;

template<class T>
static inline lean_object * CppClass_to_lean(T * t) {
    if (CppClass_class<T> == nullptr) {
        CppClass_class<T> = lean_register_external_class(CppClass_finalize<T>, CppClass_foreach<T>);
    }
    return lean_alloc_external(CppClass_class<T>, t);
}

template<class T>
static inline T const * to_CppClass(b_lean_obj_arg o) {
  return static_cast<T *>(lean_get_external_data(o));
}

extern "C" LEAN_EXPORT lean_object* houlean_rw_handle_f(b_lean_obj_arg attrib, lean_object* io) {
  
  const char * attr = lean_to_string(attrib)->m_data;

  auto * handle = new GA_RWHandleF(houLeanContext.outGeo, GA_ATTRIB_POINT, attr);

  if(handle->isValid()){
    return lean_io_result_mk_ok(CppClass_to_lean(handle));
  }else{
    lean_object * msg = lean_mk_string("Invalid point attribute!");
    return lean_mk_io_user_error(msg);
  }
}

extern "C" LEAN_EXPORT lean_object * houlean_rw_handle_f_get(b_lean_obj_arg o, uint64_t id, lean_object* io) {

  auto handle = to_CppClass<GA_RWHandleF>(o);

  // FIXME: id is used as offset but it is an index!
  return modules[currentId].io_float(handle->get(id), io);
}

extern "C" LEAN_EXPORT lean_object * houlean_rw_handle_f_set(b_lean_obj_arg o, uint64_t id, double val, lean_object* io) {

  auto handle = to_CppClass<GA_RWHandleF>(o);
  
  // FIXME: id is used as offset but it is an index!
  handle->set(id, val);

  return modules[currentId].io_unit(io);
}

extern "C" LEAN_EXPORT lean_object* houlean_rw_handle_v3(b_lean_obj_arg attrib, lean_object* io) {
  
  const char * attr = lean_to_string(attrib)->m_data;

  auto * handle = new GA_RWHandleV3(houLeanContext.outGeo, GA_ATTRIB_POINT, attr);

  if(handle->isValid()){
    return lean_io_result_mk_ok(CppClass_to_lean(handle));
  }else{
    lean_object * msg = lean_mk_string("Invalid point attribute!");
    return lean_mk_io_user_error(msg);
  }
}

extern "C" LEAN_EXPORT lean_object * houlean_rw_handle_v3_get(b_lean_obj_arg o, uint64_t id, lean_object* io) {

  auto handle = to_CppClass<GA_RWHandleV3>(o);
  // FIXME: id is used as offset but it is an index!
  auto v = handle->get(id);

  return modules[currentId].io_vec3(v.x(), v.y(), v.z(), io);
}

extern "C" LEAN_EXPORT lean_object * houlean_rw_handle_v3_set(b_lean_obj_arg o, uint64_t id, lean_object * v, lean_object* io) {

  auto handle = to_CppClass<GA_RWHandleV3>(o);

  UT_Vector3D u = {modules[currentId].vec3_x(v),
		   modules[currentId].vec3_y(v),
		   modules[currentId].vec3_z(v)};
  // FIXME: id is used as offset but it is an index!
  handle->set(id, u);

  return modules[currentId].io_unit(io);
}
