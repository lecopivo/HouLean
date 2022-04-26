
namespace Hou

  structure Vec3 where
    x : Float
    y : Float
    z : Float

  instance : ToString Vec3 := ⟨λ v => s!"[{v.x}, {v.y}, {v.z}]"⟩

end Hou
