-- import SciLean.Basic

-- open SciLean

namespace Hou

  structure Vec3 where
    x : Float
    y : Float
    z : Float

  instance : ToString Vec3 := ⟨λ v => s!"[{v.x}, {v.y}, {v.z}]"⟩

  @[inline, extern "houlean_time"]
  constant time : IO Float

  @[inline, extern "houlean_npoints"]
  constant npoints (geo : UInt64) : IO UInt64

  --- Point Attribute

  @[inline, extern "houlean_getpoint_r"]
  constant getPointR (geo : UInt64) (attrib : String) (ptnum : UInt64) : IO Float

  @[inline, extern "houlean_setpoint_r"]
  constant setPointR (attrib : String) (ptnum : UInt64) (x : Float) : IO Float

  @[inline, extern "houlean_getpoint_v3"]
  constant getPointV3 (geo : UInt64) (attrib : String) (ptnum : UInt64) : IO Vec3

  @[inline, extern "houlean_setpoint_v3"]
  constant impl.setPointV3 (attrib : String) (ptnum : UInt64) (x : Float) (y : Float) (z : Float) : IO Unit
  def setPointV3 (attrib : String) (ptnum : UInt64) (vec : Vec3) : IO Unit := 
    impl.setPointV3 attrib ptnum vec.x vec.y vec.z

  @[inline, extern "houlean_addpoint"]
  constant impl.addpoint (x y z : Float) : IO Unit 
  def addpoint (vec : Vec3) : IO Unit :=
    impl.addpoint vec.x vec.y vec.z

  -- def getAllPointR (geo : UInt64) (attrib : String) : IO (Array ℝ) := do
  --   let N := (← npoints geo).toNat
  --   let mut arr : Array ℝ := Array.mkEmpty N
  --   pure arr

  --- Detail Attribute

  @[inline, extern "houlean_getdetail_r"]
  constant getDetailR (geo : UInt64) (attrib : String) : IO Float

  @[inline, extern "houlean_setdetail_r"]
  constant setDetailtR (attrib : String) (x : Float) : IO Float

  constant SPointed : NonemptyType
  def RWHandleF : Type := SPointed.type
  def RWHandleV3 : Type := SPointed.type
  instance : Nonempty RWHandleF := SPointed.property
  instance : Nonempty RWHandleV3 := SPointed.property

  @[extern "houlean_rw_handle_f"]
  constant getAttrF (attrib : @& String) : IO RWHandleF
  @[extern "houlean_rw_handle_f_get"]
  constant RWHandleF.getOp (self : RWHandleF) (idx : UInt64) : IO Float
  @[extern "houlean_rw_handle_f_set"]
  constant RWHandleF.set (handle : RWHandleF) (id : UInt64) (val : Float) : IO Unit

  @[extern "houlean_rw_handle_v3"]
  constant getAttrV3 (attrib : @& String) : IO RWHandleV3
  @[extern "houlean_rw_handle_v3_get"]
  constant RWHandleV3.getOp (self : RWHandleV3) (idx : UInt64) : IO Vec3
  @[extern "houlean_rw_handle_v3_set"]
  constant RWHandleV3.set (handle : RWHandleV3) (id : UInt64) (val : Vec3) : IO Unit

  -- Utility functions
  @[export io_unit]
  def io_unit : IO Unit := pure ()

  @[export io_float]
  def io_float (x : Float) : IO Float := pure x

  @[export io_uint64]
  def io_uint64 (n : UInt64) : IO UInt64 := pure n

  @[export io_vec3]
  def io_vec3 (x y z : Float) : IO (Vec3) := pure ⟨x,y,z⟩

  @[export vec3_x]
  def vec3_x (u : @& Vec3) : Float := u.x

  @[export vec3_y]
  def vec3_y (u : @& Vec3) : Float := u.y

  @[export vec3_z]
  def vec3_z (u : @& Vec3) : Float := u.z

end Hou
