namespace Hou

  constant SopContext.nonempty : NonemptyType   
  def SopContext := SopContext.nonempty.type
  instance : Nonempty SopContext := SopContext.nonempty.property
  
  inductive SopError where
  | warrning (msg : String) 
  | error (msg : String)

  abbrev Sop := EStateM SopError SopContext

  @[inline, extern "houlean_time"]
  constant time : Sop Float

  @[inline, extern "houlean_npoints"]
  constant npoints (geo : USize) : Sop USize
  
end Hou
