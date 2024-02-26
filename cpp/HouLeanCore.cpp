#include <iostream> 
#include <mutex>
#include <set>
#include <string>

#include <dlfcn.h>
#include <stddef.h>

#include <GU/GU_Detail.h>
#include <OP/OP_AutoLockInputs.h>
#include <OP/OP_Director.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_Include.h>
#include <UT/UT_DSOVersion.h>

#include <CMD/CMD_Manager.h>
#include <CMD/CMD_Args.h>


#include <lean/lean.h>

#include <functional>

#include "HouLeanCore.h"
#include "utils/scope_guard.hpp"

// #include "HouLeanContext.h"
#include "HouLeanModule.h"

void newSopOperator(OP_OperatorTable *table) {

  table->addOperator(
      new OP_Operator("houlean_core", "HouLean Core",
                      SOP_HouLeanCore::myConstructor,
                      SOP_HouLeanCore::myTemplateList, 1));
}

static PRM_Name prm_names[] = {
    PRM_Name("callback_library", "Callback Library"),
    PRM_Name("library_name", "Library Name"),
    PRM_Name("compile_time", "Compile Time"),
};

PRM_Template SOP_HouLeanCore::myTemplateList[] = {
    PRM_Template(PRM_FILE, 1, &prm_names[0]),
    PRM_Template(PRM_STRING, 1, &prm_names[1]),
    PRM_Template(PRM_FLT, 1, &prm_names[2], PRMzeroDefaults),
    PRM_Template(),
};

OP_Node *SOP_HouLeanCore::myConstructor(OP_Network * net,
                                               const char * name,
                                               OP_Operator *op) {
  return new SOP_HouLeanCore(net, name, op);
}

SOP_HouLeanCore::SOP_HouLeanCore(OP_Network * net,
                                               const char * name,
                                               OP_Operator *op)
    : SOP_Node(net, name, op) {
  // Right now I'm asvsuming that anything can change
  // In fugure, I might bump data IDs base on which attribute handles were
  // created with `getattrib` function

  // mySopFlags.setManagesDataIDs(true);
}

SOP_HouLeanCore::~SOP_HouLeanCore() {}

OP_ERROR
SOP_HouLeanCore::cookMySop(OP_Context &context) {
  // We must lock our inputs before we try to access their geometry.
  // OP_AutoLockInputs will automatically unlock our inputs when we return.
  // NOTE: Don't call unlockInputs yourself when using this!
  OP_AutoLockInputs input_lock_guard(this);
  if (input_lock_guard.lock(context) >= UT_ERROR_ABORT)
    return error();

  // Duplicate incoming geometry
  duplicateSource(0, context);

  // Lock Lean context
  // std::lock_guard contextGuard{contextMutex};

  int currentId = this->getUniqueId();

  // houLeanContext.outGeo = gdp;
  // for(int i=0;i<4;i++){
  //   houLeanContext.inGeo[i] = inputGeo(i);
  // }

  flags().setTimeDep(true);
  fpreal time = context.getTime();

  // houLeanContext.time = time;

  // std::cout << "User preference directory is: " <<   << std::endl;

  // std::cout << "Node unique id: " << this->getUniqueId() << std::endl;
  
  // Load external library
  UT_String callback_library, library_name;
  evalString(callback_library, "callback_library", 0, time);
  evalString(library_name, "library_name", 0, time);
  double compile_time = evalFloat("compile_time", 0, time);

  LeanModule * module;
  {
    std::lock_guard contextGuard{moduleMutex};
    modules.try_emplace(currentId, LeanModule{});
    module = &modules[currentId];
  }

  if(module->realoadModule(callback_library, library_name, compile_time)){
    module->callMain(time, gdp);
  }

  if (error() >= UT_ERROR_ABORT)
    return error();

  return error();
}


static void
run_lean_command( CMD_Args &args )
{
    // the 's' option left pads with some spaces,
    // just for an example
    if(args.found('l') && args.found('n'))
    {
        UT_String lib_path = args.argp( 'l' );
        UT_String lib_dir, lib_file, lib_name;
        lib_path.splitPath(lib_dir, lib_file);
        lib_file.substr(lib_name, 3, lib_file.length() - 6);

        const char* node_path = args.argp( 'n' );
        
        std::cout << "Specified dynamic library: " << lib_path << "\n";
        std::cout << "Library name: " << lib_name << "\n";
        std::cout << "Node path: " << node_path << "\n";

        closeAllModules();
        reloadLean();

        void * module_handle = dlopen(lib_path, RTLD_LOCAL | RTLD_NOW);

        if (!module_handle) {
            std::cerr << "Failed loading library: " << lib_path << std::endl;
            std::cerr << dlerror() << std::endl;
        } else {

            auto lean_initialize_runtime_module = (void (*)())dlsym(lean_lib_handle, "lean_initialize_runtime_module");
            auto main = (lean_object* (*)(lean_object*, lean_object*))dlsym(module_handle, "run");

            UT_String init_name = lib_name;
            init_name.prepend("initialize_Wrangles_");
            init_name.append("_Main");
            auto initialize_Main = (lean_object* (*)(uint8_t, lean_object*))dlsym(module_handle, init_name);

            auto houlean_hou_get_result_error_msg = (lean_object* (*)(lean_object*, lean_object*))dlsym(module_handle, "houlean_hou_get_result_error_msg");
            auto houlean_hou_node_mk = (lean_object* (*)(lean_object*))dlsym(module_handle, "houlean_hou_node_mk");

            if (!lean_initialize_runtime_module) {
                std::cerr << "initialize_runtime_module failed to load!" << std::endl;
                std::cerr << dlerror() << std::endl;
                dlclose(module_handle);
                return;
            }

            if (!main) {
                std::cerr << "Main failed to load!\n";
                std::cerr << dlerror() << std::endl;
                dlclose(module_handle);
                return;
            }

            if (!initialize_Main) {
                std::cerr << "initialize_main failed to load!\n";
                std::cerr << dlerror() << std::endl;
                dlclose(module_handle);
                return;
            }

            if (!houlean_hou_get_result_error_msg) {
                std::cerr << "get_result_error_msg failed to load!\n";
                std::cerr << dlerror() << std::endl;
                dlclose(module_handle);
                return;
            }

            if (!houlean_hou_node_mk) {
                std::cerr << "houlean_hou_get_result_error_msg failed to load!\n";
                std::cerr << dlerror() << std::endl;
                dlclose(module_handle);
                return;
            }
            
            if (lean_initialize_runtime_module &&
                main && initialize_Main &&
                houlean_hou_get_result_error_msg &&
                houlean_hou_node_mk) {

                lean_initialize_runtime_module();
                initialize_Main(1 /* builtin */, lean_io_mk_world());
                lean_object * node = houlean_hou_node_mk(lean_mk_string(node_path));
                auto result = main(node, lean_box(0) /* houlean_hou_mk_world() */);

                if (lean_ptr_tag(result) != 0 /* houlean_hou_result_is_ok */) {
                    auto msg = houlean_hou_get_result_error_msg(lean_box(0), result);
                    std::cerr << "Error: " << lean_string_cstr(msg) << std::endl;
                    lean_dec(msg);
                }

                // maybe clean up ?? :)
            }

            dlclose(module_handle);
        }
        // leancommand -l "/home/tskrivan/Documents/HouLean/build/lib/libCommandDev.so"


    } else {
        args.error("You have to specify the module name and the dynamic library that contains the lean command!");
        return;
    }
}
/*
 * this function gets called once during Houdini initialization
 */
void
CMDextendLibrary( CMD_Manager *cman )
{
    // install the date command into the command manager
    //    name = "date", options_string = "s:",
    //    callback = cmd_date
    cman->installCommand("leancommand", "l:n:", run_lean_command);
}

