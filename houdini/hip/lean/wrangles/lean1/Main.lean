import HouLean

def main : IO Unit := do
  let P ← Hou.getAttrV3 "P"
  let Cd ← Hou.getAttrV3 "Cd"

  let t ← Hou.time

  for i in [0:(← Hou.npoints 0).toNat] do
    let i : UInt64 := i.toUInt64
    let p ← P[i]
    let θ := Float.atan2 p.x p.z
    let r := Float.sqrt (p.x*p.x + p.z*p.z)
    let R := Float.sqrt (p.x*p.x + p.y*p.y + p.z*p.z)
    -- let N : Hou.Vec3 := ⟨p.x/R, p.y/R, p.z/R⟩
    let A := Float.cos (40 * r - t + 3 * θ)
    let scale := 0.03
    P.set i ⟨p.x + scale * A * p.x/R,
             p.y + scale * A * p.y/R,
             p.z + scale * A * p.z/R⟩
    Cd.set i ⟨(A+1)/2, 0, 0.2*(1-(A+1)/2)⟩                
