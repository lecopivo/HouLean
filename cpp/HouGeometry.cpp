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


//------------------------------------------------------------------------------
// Hou.Node
//------------------------------------------------------------------------------


// Hou.Node.getGeometry
extern "C" lean_object* houlean_hou_node_get_geometry(lean_object* node, lean_object* houContext) {

    std::string path = hou_node_path(node);

    lean_object * error = nullptr;
    SOP_Node * sopnode = get_SOP_Node(node, houContext, error);

    if (!sopnode ) {
        return error;
    }

    OP_Context ctx = OP_Context::CurrentEvalTime;
    const GU_Detail * geo = sopnode->getCookedGeo(ctx);

    if (!geo) {
        std::stringstream msg;
        msg << "Failed cooking geometry of the node '"
            << path
            << "'!";
        return houlean_hou_throw_msg_error(lean_box(0), lean_mk_string(msg.str().c_str()), houContext);
    }

    GU_Detail * new_geo = new GU_Detail{*geo};

}
