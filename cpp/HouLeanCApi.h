#pragma once

#include <lean/lean.h>

extern "C" lean_object* houlean_sop_mk_size_t(size_t, lean_object*);
extern "C" lean_object* houlean_sop_mk_float(double, lean_object*);
extern "C" lean_object* houlean_sop_mk_unit(lean_object*);
extern "C" lean_object* houlean_sop_mk_vec3(double, double, double, lean_object*);


