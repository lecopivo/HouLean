#pragma once

#include <lean/lean.h>

extern "C" lean_object* houlean_sop_mk_unit(lean_object*);
extern "C" lean_object* houlean_sop_mk_size_t(size_t, lean_object*);
extern "C" lean_object* houlean_sop_mk_float(double, lean_object*);
extern "C" lean_object* houlean_sop_mk_vec3(double, double, double, lean_object*);
extern "C" lean_object* houlean_sop_mk_value(lean_object*, lean_object*, lean_object*);
extern "C" lean_object* houlean_sop_mk_error(lean_object*, lean_object*, lean_object*);

extern "C" uint8_t houlean_attr_owner_is_point(uint8_t);
extern "C" uint8_t houlean_attr_owner_is_vertex(uint8_t);
extern "C" uint8_t houlean_attr_owner_is_primitive(uint8_t);
extern "C" uint8_t houlean_attr_owner_is_detail(uint8_t);

extern "C" double houlean_vec3_x(lean_object*);
extern "C" double houlean_vec3_y(lean_object*);
extern "C" double houlean_vec3_z(lean_object*);
