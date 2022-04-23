#pragma once

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
