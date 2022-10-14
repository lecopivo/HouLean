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
     opaque RWHandleF.getOp (self : @& RWHandleF) (idx : @& USize) : Sop Float
     @[extern "houlean_rw_handle_f_set"]
     opaque RWHandleF.set (handle : @& RWHandleF) (idx : @& USize) (val : @& Float) : Sop Unit

     @[extern "houlean_rw_handle_v3"]
     opaque getAttrV3 (attrib : @& String) (owner : @& AttributeOwner) : Sop RWHandleV3
     @[extern "houlean_rw_handle_v3_get"]
     opaque RWHandleV3.getOp (self : @& RWHandleV3) (idx : @& USize) : Sop Vec3
     @[extern "houlean_rw_handle_v3_set"]
     opaque RWHandleV3.set (handle : @& RWHandleV3) (id : @& USize) (val : @& Vec3) : Sop Unit


  end Attributes

end Hou
