
namespace Hou

  structure Vec3 where
    x : Float
    y : Float
    z : Float

  instance : ToString Vec3 := ⟨λ v => s!"[{v.x}, {v.y}, {v.z}]"⟩

  @[export houlean_vec3_x]
  def Vec3.capi.x (v : Vec3) : Float := v.x
  @[export houlean_vec3_y]
  def Vec3.capi.y (v : Vec3) : Float := v.y
  @[export houlean_vec3_z]
  def Vec3.capi.z (v : Vec3) : Float := v.z

end Hou
