import Lean 

structure Hou.Node where
  path : String
deriving Inhabited, DecidableEq

structure Hou.NodeInput where
  node : Node
  idx : Nat
deriving Inhabited, DecidableEq

structure Hou.NodeOutput where
  node : Node
  idx : Nat
deriving Inhabited, DecidableEq

structure Hou.Parm where
  node : Node
  name : String
deriving Inhabited, DecidableEq

inductive Hou.Error where
  | nodeDoesNotExists (node : Node) (msg : String := "")
  | parmDoesNotExists (parm : Parm) (msg : String := "")
  | error (msg : String)

def Hou.Error.toString (e : Hou.Error) : String :=
  match e with
  | nodeDoesNotExists node msg => s!"Node '{node.path}' does not exists! " ++ msg
  | parmDoesNotExists parm msg => s!"Parameter '{parm.node.path}/{parm.name}' does not exists! " ++ msg
  | error msg => msg

def Hou.RealWorld : Type := Unit

def EHou (ε : Type) : Type → Type := EStateM ε Hou.RealWorld

instance : Monad (EHou ε) := inferInstanceAs (Monad (EStateM ε Hou.RealWorld))
instance : MonadFinally (EHou ε) := inferInstanceAs (MonadFinally (EStateM ε Hou.RealWorld))
instance : MonadExceptOf ε (EHou ε) := inferInstanceAs (MonadExceptOf ε (EStateM ε Hou.RealWorld))
instance : OrElse (EHou ε α) := ⟨MonadExcept.orElse⟩
instance [Inhabited ε] : Inhabited (EHou ε α) := inferInstanceAs (Inhabited (EStateM ε Hou.RealWorld α))

abbrev Hou : Type → Type := EHou Hou.Error

namespace Hou


  -- @[export houlean_hou_unit]
  -- def unit : Hou Unit := pure ()

  -- @[export houlean_hou_throw_node_does_not_exists]
  -- def throwNodeDoesNotExists (α : Type) (nodePath : String) : Hou α := throw (.nodeDoesNotExists nodePath)
  
  -- @[export houlean_hou_throw_parm_does_not_exists]
  -- def throwParmDoesNotExists (α : Type) (nodePath : String) (parmName : String) : Hou α := throw (.parmDoesNotExists nodePath parmName)
  
  @[export houlean_hou_mk_none]
  def mkNone {α : Type} : Hou (Option α) := return none

  @[export houlean_hou_mk_some]
  def mkSome {α : Type} (a : α) : Hou (Option α) := return some a

  @[export houlean_hou_get_result_error_msg]
  def getResultErrorMsg {α} (r : EStateM.Result Error RealWorld α) : String := 
      match r with
      | .ok _ _ => ""
      | .error e _ => e.toString 

  @[export houlean_hou_throw_msg_error]
  def throwMsgError {α} (msg : String) : Hou α := throw (.error msg)

  @[extern "houlean_hou_print"]
  opaque pprint (msg : String) : Hou Unit

  -- Number of inputs to a node. 

  --   This is one bigger then then the highest input index.
  -- @[extern "houlean_hou_node_num_inputs"]
  -- opaque nodeNumInputs (nodePath : String) : Hou Nat

  namespace Node

    @[export houlean_hou_node_mk]
    def mk' (path : String) : Node := ⟨path⟩

    @[export houlean_hou_node_path]
    def path' (node : Node) : String := node.path

    @[export houlean_hou_node_throw_does_not_exist]
    def throwDoesNotExist {α} (node : Node) : Hou α := throw (.nodeDoesNotExists node)

    @[extern "houlean_hou_node_num_inputs"]
    opaque numInputs (node : Node) : Hou Nat 

    @[extern "houlean_hou_node_min_inputs"]
    opaque minInputs (node : Node) : Hou Nat 

    @[extern "houlean_hou_node_max_inputs"]
    opaque maxInputs (node : Node) : Hou Nat 

    @[extern "houlean_hou_node_num_outputs"]
    opaque numOutputs (node : Node) : Hou Nat 

    def input  (node : Node) (idx : Nat) : Hou NodeInput := do
      if idx < (← node.maxInputs) 
      then return ⟨node, idx⟩
      else throw (.error "Hou.Node.input: Invalid index!")

    def output (node : Node) (idx : Nat) : Hou NodeOutput := do
      if idx < (← node.numOutputs) 
      then return ⟨node, idx⟩
      else throw (.error "Hou.Node.output: Invalid index!")

    /-- Get node given relative path to a node -/
    @[extern "houlean_hou_node_node"]
    opaque node (node : Node) (relPath : String) : Hou Node 

    @[extern "houlean_hou_node_num_children"]
    opaque numChildren (node : Node) : Hou Nat 

    @[extern "houlean_hou_node_child"]
    opaque child (node : Node) (idx : Nat) : Hou Node

    def children (node : Node) : Hou (Array Node) := do
      let mut c : Array Node := #[]
      for i in [0: ← node.numChildren] do
        c := c.push (← node.child i)
      return c

    /-- Parent of a node -/
    def parent (node : Node) : Hou Node := node.node ".."
  
  end Node

  namespace NodeInput

    @[export houlean_hou_node_input_mk]
    def mk' (path : String) (idx : USize) : NodeInput := ⟨⟨path⟩, idx.toNat⟩

    @[export houlean_hou_node_input_node]
    def node' (input : NodeInput) : Node := input.node

    @[export houlean_hou_node_input_path]
    def path (input : NodeInput) : String := input.node.path

    @[export houlean_hou_node_input_idx]
    def idx' (input : NodeInput) : USize := input.idx.toUSize

    @[extern "houlean_hou_node_input_output"]
    opaque output (input : NodeInput) : Hou (Option NodeOutput) 

    @[extern "houlean_hou_node_input_connect"]
    opaque connect (input : NodeInput) (output : NodeOutput) : Hou Unit

  end NodeInput

  namespace NodeOutput

    @[export houlean_hou_node_output_mk]
    def mk' (path : String) (idx : USize) : NodeOutput := ⟨⟨path⟩, idx.toNat⟩

    @[export houlean_hou_node_output_node]
    def node' (output : NodeOutput) : Node := output.node

    @[export houlean_hou_node_output_path]
    def path (output : NodeOutput) : String := output.node.path

    @[export houlean_hou_node_output_idx]
    def idx' (output : NodeOutput) : USize := output.idx.toUSize

    -- Don't know how to implement these
    @[extern "houlean_hou_node_output_inputs"]
    opaque inputs (output : NodeOutput) : Hou (Array NodeInput)

    def connect (output : NodeOutput) (input : NodeInput) : Hou Unit := 
      input.connect output

  end NodeOutput
  

  --   @[extern "houlean_hou_node_get_input"]
  --   opaque getInput (node : Node) (idx : Nat) : Hou Node

  --   /-- Connects `inIdx` input of node `inNode` with `outIdx` output of node `outNode` -/
  --   @[extern "houlean_hou_node_set_input"]
  --   opaque setInput (inNode : Node) (inIdx : Nat) (outNode : Node) (outIdx : Nat := 0) : Hou Unit

  --   -- c todo
  --   @[extern "houlean_hou_node_input"]
  --   opaque getOutput (node : Node) (idx : Nat) : Hou Node

  --   def setOutput (outNode : Node) (outIdx : Nat) (inNode : Node) (inIdx : Nat) : Hou Unit :=
  --     inNode.setInput inIdx outNode outIdx



  -- end Node
    

end Hou


