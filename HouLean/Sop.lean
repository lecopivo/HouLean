import HouLean.Data

namespace Hou

  constant SopContext.nonempty : NonemptyType   
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

  @[extern "houlean_print"]
  constant print (msg : @& String) : Sop Unit

  def println (msg : String) : Sop Unit := print (msg ++ "\n")
  
end Hou
