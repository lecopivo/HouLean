#pragma once

#include <lean/lean.h>

template<class T>
static void houlean_external_finalize(void * obj){
  delete static_cast<T*>(obj);
}

template<class T>
static void houlean_external_foreach(void *, b_lean_obj_arg){
  // do nothing since `S` does not contain nested Lean objects
}

template<class T>
static lean_external_class *houlean_external_class = nullptr;

template<class T>
static inline lean_object * houlean_external_to_lean(T * t) {
    if (houlean_external_class<T> == nullptr) {
      houlean_external_class<T> = lean_register_external_class(houlean_external_finalize<T>, houlean_external_foreach<T>);
    }
    return lean_alloc_external(houlean_external_class<T>, t);
}

template<class T>
static inline T const * houlean_lean_to_external(b_lean_obj_arg o) {
  return static_cast<T *>(lean_get_external_data(o));
}


