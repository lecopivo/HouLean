
import HouLean.Sop

namespace Hou

  @[extern "houlean_time"]
  opaque time : Sop Float

  @[extern "houlean_npoints"]
  opaque npoints (geo : USize) : Sop USize


  
  section Attributes

     opaque RWHandleF.nonempty : NonemptyType
     def RWHandleF : Type := RWHandleF.nonempty.type
     instance : Nonempty RWHandleF := RWHandleF.nonempty.property

     opaque RWHandleV3.nonempty : NonemptyType
     def RWHandleV3 : Type := RWHandleV3.nonempty.type
     instance : Nonempty RWHandleV3 := RWHandleV3.nonempty.property

     @[unbox]
     inductive AttributeOwner | point | vertex | primitive | detail

     @[export houlean_attr_owner_is_point]
     def AttributeOwner.isPoint : AttributeOwner → Bool | point => true | _ => false
     @[export houlean_attr_owner_is_vertex]
     def AttributeOwner.isVertex : AttributeOwner → Bool | vertex => true | _ => false
     @[export houlean_attr_owner_is_primitive]
     def AttributeOwner.isPrimitive : AttributeOwner → Bool | primitive => true | _ => false
     @[export houlean_attr_owner_is_detail]
     def AttributeOwner.isDetail : AttributeOwner → Bool | detail => true | _ => false

     @[extern "houlean_rw_handle_f"]
     opaque getAttrF (attrib : @& String) (owner : @& AttributeOwner) : Sop RWHandleF
     @[extern "houlean_rw_handle_f_get"]
     opaque RWHandleF.get (self : @& RWHandleF) (idx : @& USize) : Sop Float
     @[extern "houlean_rw_handle_f_set"]
     opaque RWHandleF.set (handle : @& RWHandleF) (idx : @& USize) (val : @& Float) : Sop Unit

     instance : GetElem RWHandleF USize (Sop Float) (λ _ _ => True) := ⟨λ handle idx _ => handle.get idx⟩

     @[extern "houlean_rw_handle_v3"]
     opaque getAttrV3 (attrib : @& String) (owner : @& AttributeOwner) : Sop RWHandleV3
     @[extern "houlean_rw_handle_v3_get"]
     opaque RWHandleV3.get (self : @& RWHandleV3) (idx : @& USize) : Sop Vec3
     @[extern "houlean_rw_handle_v3_set"]
     opaque RWHandleV3.set (handle : @& RWHandleV3) (id : @& USize) (val : @& Vec3) : Sop Unit

     instance : GetElem RWHandleV3 USize (Sop Vec3) (λ _ _ => True) := ⟨λ handle idx _ => handle.get idx⟩


  end Attributes



  section DistanceQueries

     opaque GURayIntersect.nonempty : NonemptyType
     def GURayIntersect : Type := GURayIntersect.nonempty.type
     instance : Nonempty GURayIntersect := GURayIntersect.nonempty.property

     @[extern "houlean_sop_get_gu_ray_intersect"]
     opaque getGURayIntersect' (geoId : USize) : Sop GURayIntersect 

     def getGURayIntersect (geoId : Nat) : Sop GURayIntersect :=
       getGURayIntersect' geoId.toUSize

     @[extern "houlean_sop_gu_ray_intersect_dist"]
     opaque GURayIntersect.dist (ri : @& GURayIntersect) (x : @& Vec3) : Float

     @[extern "houlean_sop_gu_ray_intersect_closest_point"]
     opaque GURayIntersect.closestPoint (ri : @& GURayIntersect) (x : @& Vec3) : Vec3

  end DistanceQueries

end Hou
