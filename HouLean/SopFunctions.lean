import HouLean.Sop

namespace Hou

  @[extern "houlean_time"]
  constant time : Sop Float

  @[extern "houlean_npoints"]
  constant npoints (geo : USize) : Sop USize

end Hou
