#include <iostream>

#include <OP/OP_Director.h>
#include <SOP/SOP_Node.h>

#include <GA/GA_Handle.h>
#include <GA/GA_Types.h>

#include <lean/lean.h>

#include "External.h"
// #include "SopContext.h"
#include "HouCApi.h"
#include "Util.h"


extern "C" lean_object* houlean_hou_print(b_lean_obj_arg msg, lean_object* houContext){
  std::cout << lean_string_cstr(msg);
  return houlean_hou_unit(houContext);
}


//------------------------------------------------------------------------------
// Hou.Node
//------------------------------------------------------------------------------

// Hou.Node.numInputs
extern "C" lean_object* houlean_hou_node_num_inputs(lean_object* node, lean_object* houContext){
    
    OP_Node * opnode = OPgetDirector()->findNode(lean_string_cstr(houlean_hou_node_path(node)));

    if (!opnode) {
        return houlean_hou_node_throw_does_not_exist(lean_box(0), node, houContext);
    }
    
    lean_dec(node);
    return houlean_hou_return(lean_box(opnode->nInputs()), houContext);
}

// Hou.Node.minInputs
extern "C" lean_object* houlean_hou_node_min_inputs(lean_object* node, lean_object* houContext){

    OP_Node * opnode = OPgetDirector()->findNode(lean_string_cstr(houlean_hou_node_path(node)));

    if (!opnode) {
        return houlean_hou_node_throw_does_not_exist(lean_box(0), node, houContext);
    }
    
    lean_dec(node);
    return houlean_hou_return(lean_box(opnode->minInputs()), houContext);
}

// Hou.Node.maxInputs
extern "C" lean_object* houlean_hou_node_max_inputs(lean_object* node, lean_object* houContext){

    OP_Node * opnode = OPgetDirector()->findNode(lean_string_cstr(houlean_hou_node_path(node)));

    if (!opnode) {
        return houlean_hou_node_throw_does_not_exist(lean_box(0), node, houContext);
    }
    
    lean_dec(node);
    return houlean_hou_return(lean_box(opnode->maxInputs()), houContext);
}

// Hou.Node.numOutputs
extern "C" lean_object* houlean_hou_node_num_outputs(lean_object* node, lean_object* houContext){

    OP_Node * opnode = OPgetDirector()->findNode(lean_string_cstr(houlean_hou_node_path(node)));

    if (!opnode) {
        return houlean_hou_node_throw_does_not_exist(lean_box(0), node, houContext);
    }
    
    lean_dec(node);
    return houlean_hou_return(lean_box(opnode->maxOutputs()), houContext);
}

// Hou.Node.node
extern "C" lean_object* houlean_hou_node_node(lean_obj_arg node, lean_obj_arg rel_path, lean_object* houContext){

    OP_Node * opnode = OPgetDirector()->findNode(lean_string_cstr(houlean_hou_node_path(node)));
    
    if (!opnode) {
        lean_dec(rel_path);
        return houlean_hou_node_throw_does_not_exist(lean_box(0), node, houContext);
    }

    UT_String relPath = lean_string_cstr(rel_path);
    
    auto relNode = opnode->findNode(relPath);

    if (!relNode) {
        lean_object * rel_node = houlean_hou_node_mk(lean_string_append(houlean_hou_node_path(node), rel_path));
        lean_dec(rel_path);                                             
        return houlean_hou_node_throw_does_not_exist(lean_box(0), rel_node, houContext);
    }
    
    lean_object * rel_node = houlean_hou_node_mk(lean_mk_string(relNode->getFullPath().c_str()));

    lean_dec(node);
    lean_dec(rel_path);
    
    return houlean_hou_return(rel_node, houContext);
}

// Hou.Node.numChildren
extern "C" lean_object* houlean_hou_node_num_children(lean_object* node, lean_object* houContext){

    lean_object * error;

    OP_Node * parent = get_OP_Node(node, houContext, error);

    if (error) {
        return error;
    }
    
    return houlean_hou_return(lean_box(parent->getNchildren()), houContext);
}

// Hou.Node.child
extern "C" lean_obj_res houlean_hou_node_child(lean_obj_arg node, lean_obj_arg idx, lean_obj_arg houContext){

    lean_object * error;

    size_t i = lean_usize_of_nat_mk(idx);

    OP_Node * parent = get_OP_Node(node, houContext, error);

    if (error) {
        return error;
    }

    OP_Node * child = parent->getChild(i);

    if(!child) {
        auto path = houlean_hou_node_path(node);
        std::stringstream msg;
        msg << "Hou.Node.child: Node '"
            << lean_string_cstr(path)
            << "' does not have "
            << i << "-th child!";
        lean_dec(path);
        return houlean_hou_throw_msg_error(lean_box(0), lean_mk_string(msg.str().c_str()), houContext);        
    }

    lean_object * child_node = houlean_hou_node_mk(lean_mk_string(child->getFullPath()));        

    lean_dec(node);
    return houlean_hou_return(child_node, houContext);
}

//------------------------------------------------------------------------------
// Hou.NodeInput
//------------------------------------------------------------------------------

// Hou.NodeInput.output
extern "C" lean_object* houlean_hou_node_input_output(lean_obj_arg input, lean_obj_arg houContext){
    lean_object * error;    

    lean_inc(input);
    size_t input_idx  = houlean_hou_node_input_idx(input);    
    auto node = houlean_hou_node_input_node(input);

    OP_Node * input_node = get_OP_Node(node, houContext, error);

    if (error) {
        return error;
    }

    OP_Node * output_node = input_node->getInput(input_idx);

    if (!output_node) {
        return houlean_hou_mk_none(lean_box(0), houContext);
    }
    
    size_t output_idx = output_node->whichOutputIs(input_node, input_idx);

    auto output = houlean_hou_node_input_mk(lean_mk_string(output_node->getFullPath()),
                                            output_idx);

    return houlean_hou_mk_some(lean_box(0), output, houContext);
}

// Hou.NodeInput.connect
extern "C" lean_object* houlean_hou_node_input_connect(lean_obj_arg input, lean_obj_arg output, lean_obj_arg houContext){

    lean_object * error;    

    lean_inc(input);    
    size_t input_idx  = houlean_hou_node_input_idx(input);    
    auto input_node = houlean_hou_node_input_node(input);
    
    lean_inc(output);
    size_t output_idx  = houlean_hou_node_output_idx(output);    
    auto output_node = houlean_hou_node_output_node(output);

    
    OP_Node * input_opnode = get_OP_Node(input_node, houContext, error);

    if (error) {
        lean_dec(output_node);
        return error;
    }

    OP_Node * output_opnode = get_OP_Node(output_node, houContext, error);

    if (error) {
        return error;
    }

    auto err = input_opnode->setInput(input_idx, output_opnode, output_idx);

    if (err) {
        std::stringstream msg;
        msg << "Unable to make connection from output:"
            << output_idx << " of node "
            << output_opnode->getFullPath()
            << " and input:"
            << input_idx << " of node "
            << input_opnode->getFullPath();
        
        return houlean_hou_throw_msg_error(lean_box(0), lean_mk_string(msg.str().c_str()), houContext);        
    }

    return houlean_hou_unit(houContext);
}

//------------------------------------------------------------------------------
// Hou.NodeOutput
//------------------------------------------------------------------------------

// Hou.NodeOutput.inputs
extern "C" lean_obj_res houlean_hou_node_output_inputs(lean_obj_arg output, lean_obj_arg houContext){

    lean_object * error;

    lean_inc(output);
    size_t output_idx  = houlean_hou_node_output_idx(output);    
    auto output_node = houlean_hou_node_output_node(output);
    
    OP_Node * output_opnode = get_OP_Node(output_node, houContext, error);

    if (error) {
        return error;
    }

    // Is there beter HDK function then calling `getOutputnodes` and
    // then iterate over inputs with `whichOutputs` to get the input index?
    UT_Array<OP_Node *> input_opnodes;
    output_opnode->getOutputNodes(input_opnodes, false, true, output_idx);

    input_opnodes.sort([](OP_Node * a, OP_Node * b) { return a->getFullPath() < b->getFullPath(); });
    input_opnodes.sortedRemoveDuplicates();

    size_t n = input_opnodes.size();

    lean_object * inputs = lean_mk_empty_array_with_capacity(lean_box(n));

    for(int i; i<n; i++){

        OP_Node * input_opnode = input_opnodes[i];

        auto path = input_opnode->getFullPath();

        for(size_t input_idx=0;
            input_idx<input_opnode->maxInputs();
            input_idx++){

            if (output_idx
                ==
                output_opnode->whichOutputIs(input_opnode, input_idx)) {
                
                lean_object * input = houlean_hou_node_input_mk(lean_mk_string(path),
                                                                input_idx);

                inputs = lean_array_push(inputs, input);
            }
        }
    }
    
    return houlean_hou_return(inputs, houContext);
}
