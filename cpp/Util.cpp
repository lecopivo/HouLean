#include "Util.h"
#include "lean/lean.h"
#include "HouCApi.h"

std::string hou_node_path(b_lean_obj_arg node) {
    lean_inc(node);
    lean_object * path = houlean_hou_node_path(node);

    std::string path_str = lean_string_cstr(path);

    lean_dec(path);
    lean_dec(node);

    return path_str;
}

OP_Node * get_OP_Node(lean_obj_arg node, lean_object * houContext, lean_obj_res & error){

    std::string path = hou_node_path(node);

    OP_Node * opnode = OPgetDirector()->findNode(path.c_str());
    
    if (opnode) {
        error = nullptr;
        lean_dec(node);
        return opnode;
    } else {
        // node is used in error        
        error = houlean_hou_node_throw_does_not_exist(lean_box(0), node, houContext);
        return nullptr;
    }
}


SOP_Node * get_SOP_Node(lean_obj_arg node, lean_object * houContext, lean_obj_res & error){

    std::string path = hou_node_path(node);

    OP_Node * opnode = OPgetDirector()->findNode(path.c_str());
    
    if (opnode) {
        SOP_Node * sopnode = opnode->getSOPNode(".");

        if (sopnode) {
            error = nullptr;
            lean_dec(node);
            return sopnode;
        } else {
            std::stringstream msg;
            msg << "The node '"
                << path
                << "' is not SOP node!";
            error = houlean_hou_throw_msg_error(lean_box(0), lean_mk_string(msg.str().c_str()), houContext);
            lean_dec(node);
            return nullptr;
        }
    } else {
        // node is used in error        
        error = houlean_hou_node_throw_does_not_exist(lean_box(0), node, houContext);
        return nullptr;
    }
}


