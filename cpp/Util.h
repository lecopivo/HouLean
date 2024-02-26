#include <iostream>

#include <OP/OP_Director.h>
#include <SOP/SOP_Node.h>

#include <lean/lean.h>

/** Get Houdini node
   @arg node - consumed
   @arg houContext - consumed if error is set and nullptr is returned
   @arg error - set if node does not exits
 */
OP_Node *get_OP_Node(lean_obj_arg node, lean_object *houContext,
                     lean_obj_res &error);

/** Get Houdini SOP node
   @arg node - consumed
   @arg houContext - consumed if error is set and nullptr is returned
   @arg error - set if node does not exits or not a SOP node
 */
SOP_Node * get_SOP_Node(lean_obj_arg node, lean_object * houContext,
                        lean_obj_res & error);


/** Path of a houdini node
   @arg node - borrowed
 */
std::string hou_node_path(b_lean_obj_arg node);
