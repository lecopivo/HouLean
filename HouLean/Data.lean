
namespace Hou

  structure Vec3 where
    x : Float
    y : Float
    z : Float
    deriving Inhabited

  instance : ToString Vec3 := ⟨λ v => s!"[{v.x}, {v.y}, {v.z}]"⟩

  @[export houlean_vec3_mk]
  def Vec3.capi.mk (x y z : Float) : Vec3 := ⟨x,y,z⟩

  --@[export houlean_vec3_x]
  def Vec3.capi.x (v : Vec3) : Float := v.x
  --@[export houlean_vec3_y]
  def Vec3.capi.y (v : Vec3) : Float := v.y
  --@[export houlean_vec3_z]
  def Vec3.capi.z (v : Vec3) : Float := v.z

  instance : Add Vec3 := ⟨fun u v => ⟨u.x+v.x, u.y+v.y, u.z+v.z⟩⟩
  instance : Sub Vec3 := ⟨fun u v => ⟨u.x-v.x, u.y-v.y, u.z-v.z⟩⟩
  instance : Neg Vec3 := ⟨fun u => ⟨-u.x, -u.y, -u.z⟩⟩
  instance : HMul Float Vec3 Vec3 := ⟨fun r u => ⟨r*u.x, r*u.y, r*u.z⟩⟩
  instance : HDiv Vec3 Float Vec3 := ⟨fun u r => ⟨u.x/r, u.y/r, u.z/r⟩⟩

end Hou
