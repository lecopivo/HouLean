import HouLean

def main : IO Unit := do

  let time ← Hou.time

  for i in [0:(← Hou.npoints 0).toNat] do
    let i := i.toUInt64
    let mut p ← Hou.getPointV3 0 "P" i
    let R := Float.sqrt (p.x * p.x + p.z * p.z)
    p := {p with y := p.y + 0.1 * Float.sin (10 * R - 4 * time)}
    Hou.setPointV3 "P" i p
