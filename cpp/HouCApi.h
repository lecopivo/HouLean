#pragma once

#include <lean/lean.h>


// return/Pure.pure 
inline lean_obj_res houlean_hou_return(lean_obj_arg val, lean_obj_arg houContext){
  lean_object* result = lean_alloc_ctor(0, 2, 0);
  lean_ctor_set(result, 0, val);
  lean_ctor_set(result, 1, houContext);
  return result;
}

// Hou Unit
inline lean_obj_res houlean_hou_unit(lean_obj_arg houContext){
    return houlean_hou_return(lean_box(0), houContext);
}

// Hou (Option ...)
extern "C" lean_obj_res houlean_hou_mk_none(lean_object * type, lean_obj_arg houContext);
extern "C" lean_obj_res houlean_hou_mk_some(lean_object * type, lean_obj_arg val, lean_obj_arg houContext);

// Result
extern "C" lean_object* houlean_hou_get_result_error_msg(lean_object* type, b_lean_obj_arg result);

// Hou.Error
extern "C" lean_object* houlean_hou_throw_msg_error(lean_object* type, lean_obj_arg msg, lean_obj_arg houContext);

// Hou.Node
extern "C" lean_obj_res houlean_hou_node_mk(lean_obj_arg path);
extern "C" lean_obj_res houlean_hou_node_path(lean_obj_arg node);
extern "C" lean_obj_res houlean_hou_node_throw_does_not_exist(lean_object* type, lean_obj_arg node, lean_obj_arg houContext);

// Hou.NodeInput
extern "C" lean_obj_res houlean_hou_node_input_mk(lean_obj_arg path, size_t idx);
extern "C" lean_obj_res houlean_hou_node_input_node(lean_obj_arg input);
extern "C" lean_obj_res houlean_hou_node_input_path(lean_obj_arg input);
extern "C" size_t houlean_hou_node_input_idx(b_lean_obj_arg input);

// Hou.NodeOutput
extern "C" lean_obj_res houlean_hou_node_output_mk(lean_obj_arg path, size_t idx);
extern "C" lean_obj_res houlean_hou_node_output_node(lean_obj_arg output);
extern "C" lean_obj_res houlean_hou_node_output_path(lean_obj_arg output);
extern "C" size_t houlean_hou_node_output_idx(b_lean_obj_arg output);

