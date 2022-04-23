import SciLean.Basic

open SciLean

namespace Hou

  @[inline, extern "houlean_time"]
  constant time : IO Float

  @[inline, extern "houlean_npoints"]
  constant npoints (geo : UInt64) : IO UInt64

  --- Point Attribute

  @[inline, extern "houlean_getpoint_r"]
  constant getPointR (geo : UInt64) (attrib : String) (ptnum : UInt64) : IO ℝ

  @[inline, extern "houlean_setpoint_r"]
  constant setPointR (attrib : String) (ptnum : UInt64) (x : Float) : IO ℝ

  @[inline, extern "houlean_getpoint_v3"]
  constant getPointV3 (geo : UInt64) (attrib : String) (ptnum : UInt64) : IO (ℝ^(3 : Nat))

  @[inline, extern "houlean_setpoint_v3"]
  constant setPointV3.impl (attrib : String) (ptnum : UInt64) (x : Float) (y : Float) (z : Float) : IO Unit

  def setPointV3 (attrib : String) (ptnum : UInt64) (vec : ℝ^(3 : Nat)) : IO Unit := 
    setPointV3.impl attrib ptnum (vec[0].val) (vec[1].val) (vec[2].val)

  -- def getAllPointR (geo : UInt64) (attrib : String) : IO (Array ℝ) := do
  --   let N := (← npoints geo).toNat
  --   let mut arr : Array ℝ := Array.mkEmpty N
  --   pure arr

  --- Detail Attribute

  @[inline, extern "houlean_getdetail_r"]
  constant getDetailR (geo : UInt64) (attrib : String) : IO ℝ

  @[inline, extern "houlean_setdetail_r"]
  constant setDetailtR (attrib : String) (x : Float) : IO ℝ

  -- Utility functions

  @[export io_unit]
  def io_unit : IO Unit := pure ()

  @[export io_float]
  def io_float (x : Float) : IO Float := pure x

  @[export io_uint64]
  def io_uint64 (n : UInt64) : IO UInt64 := pure n

  @[export io_vec3]
  def io_vec3 (x y z : ℝ) : IO (ℝ^(3 : Nat)) := pure (^[x,y,z])

  @[export vec3_x]
  def vec3_x (u : @& ℝ^(3 : Nat)) : Float := (u.get 0).val

  @[export vec3_y]
  def vec3_y (u : @& ℝ^(3 : Nat)) : Float := (u.get 1).val

  @[export vec3_z]
  def vec3_z (u : @& ℝ^(3 : Nat)) : Float := (u.get 2).val

end Hou
