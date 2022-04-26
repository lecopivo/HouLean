#include <lean/lean.h>

extern "C" lean_object* houlean_time(lean_object*){
  return lean_io_result_mk_ok(lean_box_float(42));
}
