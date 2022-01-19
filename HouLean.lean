import SciLean.Basic

namespace Hou

  @[inline, extern "houlean_npoints"]
  constant npoints (geo : UInt64) : IO UInt64

  @[inline, extern "houlean_time"]
  constant time : IO Float

  @[inline, extern "houlean_foo"]
  constant foo (n : UInt64) : UInt64

  -- @[inline, extern "houlean_getattrib"]
  -- constant getattrib (geo : UInt64) (attrib : String) (ptnum : UInt64) (dim : UInt64) : IO Float

  @[inline, extern "houlean_getpoint_v3"]
  constant getPointV3 (geo : UInt64) (attrib : String) (ptnum : UInt64) : IO (ℝ^(3 : Nat))

  @[inline, extern "houlean_setpoint_v3"]
  constant setPointV3_impl (attrib : String) (ptnum : UInt64) (x : Float) (y : Float) (z : Float) : IO Unit

  constant setPointV3 (attrib : String) (ptnum : UInt64) (vec : ℝ^(3 : Nat)) : IO Unit := 
    setPointV3_impl attrib ptnum (vec[0]) (vec[1]) (vec[2])

  @[inline, extern "houlean_getpoint_r"]
  constant getPointR (geo : UInt64) (attrib : String) (ptnum : UInt64) : IO ℝ

  @[inline, extern "houlean_setpoint_r"]
  constant setPointR (attrib : String) (ptnum : UInt64) (x : Float) : IO ℝ

  @[export simple]
  def simple (n : UInt64) : IO UInt64 := n

  @[export io_unit]
  def io_unit : IO Unit := pure ()

  @[export io_float]
  def io_float (x : Float) : IO Float := pure x

  @[export io_uint64]
  def io_uint64 (n : UInt64) : IO UInt64 := pure n

  @[export io_vec3]
  def io_vec3 (x y z : ℝ) : IO (ℝ^(3 : Nat)) := pure (^[x,y,z])

  @[export vec3_x]
  def vec3_x (u : @& ℝ^(3 : Nat)) : Float := u.get 0

  @[export vec3_y]
  def vec3_y (u : @& ℝ^(3 : Nat)) : Float := u.get 1

  @[export vec3_z]
  def vec3_z (u : @& ℝ^(3 : Nat)) : Float := u.get 2

end Hou
