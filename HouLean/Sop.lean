import HouLean.Data

namespace Hou

  opaque SopContext.nonempty : NonemptyType   
  def SopContext := SopContext.nonempty.type
  instance : Nonempty SopContext := SopContext.nonempty.property
  
  inductive SopError where
  | missingValue
  | user_error (msg : String)

  abbrev Sop := EStateM SopError SopContext

  instance {α} : Inhabited (Sop α) := ⟨λ σ => .error .missingValue σ⟩

  @[export houlean_sop_mk_unit]
  def Sop.capi.unit : Sop Unit := pure ()

  @[export houlean_sop_mk_float]
  def Sop.capi.float (x : Float) : Sop Float := pure x

  @[export houlean_sop_mk_size_t]
  def Sop.capi.usize (x : USize) : Sop USize := pure x

  @[export houlean_sop_mk_vec3]
  def Sop.capi.vec3 (x y z : Float) : Sop Vec3 := pure ⟨x,y,z⟩

  @[export houlean_sop_mk_value]
  def Sop.capi.value {α} (a : α) : Sop α := pure a

  @[export houlean_sop_mk_error]
  def Sop.capi.error {α} (msg : String) : Sop α := λ σ => .error (.user_error msg) σ

  @[export houlean_sop_result_is_ok]
  def Sop.capi.resultIsOk {α} (r : EStateM.Result SopError SopContext α) : Bool := 
    match r with
    | .ok .. => true
    | .error .. => false

  @[export houlean_sop_result_error_msg]
  def Sop.capi.resultErrorMsg {α} (r : EStateM.Result SopError SopContext α) : String := 
    match r with
    | .error (.user_error msg) _ => msg
    | .error (.missingValue) _ => "missing value"
    | _ => ""

  @[export houlean_sop_throw_error]
  def Sop.capi.throwErrror {α} (msg : String) : Sop α := fun s => .error (.user_error msg) s

  @[extern "houlean_print"]
  opaque print (msg : @& String) : Sop Unit

  def println (msg : String) : Sop Unit := print (msg ++ "\n")
  
end Hou
