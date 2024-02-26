#pragma once

#include <lean/lean.h>

extern "C" lean_object* houlean_sop_mk_unit(lean_object*);
extern "C" lean_object* houlean_sop_mk_size_t(size_t, lean_object*);
extern "C" lean_object* houlean_sop_mk_float(double, lean_object*);
extern "C" lean_object* houlean_sop_mk_vec3(double, double, double, lean_object*);
extern "C" lean_object* houlean_sop_mk_value(lean_object*, lean_object*, lean_object*);
extern "C" lean_object* houlean_sop_mk_error(lean_object*, lean_object*, lean_object*);
extern "C" uint8_t houlean_sop_result_is_ok(lean_object* type, lean_obj_arg result);
extern "C" lean_object* houlean_sop_result_error_msg(lean_object* type, lean_obj_arg result);

extern "C" uint8_t houlean_attr_owner_is_point(uint8_t);
extern "C" uint8_t houlean_attr_owner_is_vertex(uint8_t);
extern "C" uint8_t houlean_attr_owner_is_primitive(uint8_t);
extern "C" uint8_t houlean_attr_owner_is_detail(uint8_t);

extern "C" lean_object* houlean_vec3_mk(double x_1, double x_2, double x_3);
inline double houlean_vec3_x(lean_object* x_1){
  double x_2; 
  x_2 = lean_ctor_get_float(x_1, 0);
  return x_2;
}
inline double houlean_vec3_y(lean_object* x_1){
  double x_2; 
  x_2 = lean_ctor_get_float(x_1, 8);
  return x_2;
}  
inline double houlean_vec3_z(lean_object* x_1){
  double x_2; 
  x_2 = lean_ctor_get_float(x_1, 16);
  return x_2;
}
