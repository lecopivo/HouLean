
import HouLean.Sop

namespace Hou

  @[extern "houlean_time"]
  opaque time : Sop Float

  @[extern "houlean_npoints"]
  opaque npoints (geo : USize) : Sop USize

  structure SurfacePoint where
    primIdxUSize : USize
    u : Float
    v : Float
    w : Float
    deriving Inhabited
  
  def SurfacePoint.primIdx (p : SurfacePoint) : Nat  := p.primIdxUSize.toNat
  def SurfacePoint.uvw     (p : SurfacePoint) : Vec3 := ⟨p.u,p.v,p.w⟩

  section Attributes

     opaque RWHandleF.nonempty : NonemptyType
     def RWHandleF : Type := RWHandleF.nonempty.type
     instance : Nonempty RWHandleF := RWHandleF.nonempty.property

     opaque RWHandleV3.nonempty : NonemptyType
     def RWHandleV3 : Type := RWHandleV3.nonempty.type
     instance : Nonempty RWHandleV3 := RWHandleV3.nonempty.property

     opaque ROHandleF.nonempty : NonemptyType
     def ROHandleF : Type := ROHandleF.nonempty.type
     instance : Nonempty ROHandleF := ROHandleF.nonempty.property

     opaque ROHandleV3.nonempty : NonemptyType
     def ROHandleV3 : Type := ROHandleV3.nonempty.type
     instance : Nonempty ROHandleV3 := ROHandleV3.nonempty.property

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

     @[extern "houlean_ro_handle_f"]
     opaque getROAttrF (geo : USize) (attrib : @& String) (owner : @& AttributeOwner) : Sop ROHandleF
     @[extern "houlean_ro_handle_f_get"]
     opaque ROHandleF.get (self : @& ROHandleF) (idx : @& USize) : Float

     @[extern "houlean_ro_handle_f_surface_get"]
     private opaque ROHandleF.surfaceGet' (self : @& ROHandleF) (primIdx : USize) (u v w : Float)  : Float
     def ROHandleF.surfaceGet (self : @& ROHandleF) (p : SurfacePoint) : Float :=
       self.surfaceGet' p.primIdxUSize p.u p.v p.w

     instance : GetElem RWHandleF USize (Sop Float) (λ _ _ => True) := ⟨λ handle idx _ => handle.get idx⟩

     @[extern "houlean_rw_handle_v3"]
     opaque getAttrV3 (attrib : @& String) (owner : @& AttributeOwner) : Sop RWHandleV3
     @[extern "houlean_rw_handle_v3_get"]
     opaque RWHandleV3.get (self : @& RWHandleV3) (idx : @& USize) : Sop Vec3
     @[extern "houlean_rw_handle_v3_set"]
     opaque RWHandleV3.set (handle : @& RWHandleV3) (id : @& USize) (val : @& Vec3) : Sop Unit

     @[extern "houlean_ro_handle_v3"]
     opaque getROAttrV3 (geo : USize) (attrib : @& String) (owner : @& AttributeOwner) : Sop ROHandleV3
     @[extern "houlean_ro_handle_v3_get"]
     opaque ROHandleV3.get (self : @& ROHandleV3) (idx : @& USize) : Vec3

     @[extern "houlean_ro_handle_v3_surface_get"]
     private opaque ROHandleV3.surfaceGet' (self : @& ROHandleV3) (primIdx : USize) (u v w : Float) : Vec3
     def ROHandleV3.surfaceGet (self : @& ROHandleV3) (p : SurfacePoint) : Vec3 :=
       self.surfaceGet' p.primIdxUSize p.u p.v p.w

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

     @[extern "houlean_sop_gu_ray_intersect_closest_surface_point"]
     opaque GURayIntersect.closestSurfacePoint (ri : @& GURayIntersect) (x : @& Vec3) : SurfacePoint


  end DistanceQueries


  

  section Derivatives
  open SciLean

  variable 
    (R) [IsROrC R] 
    {X} [Vec R X] 
    {Y} [Vec R Y]

  @[fun_trans]
  opaque sopDeriv (R) [IsROrC R] {X} [Vec R X] {Y} [Vec R Y] (f : X → Sop Y) (x dx : X) : Sop Y 
  @[fun_prop]
  opaque SopDifferentiable (R) [IsROrC R] {X} [Vec R X] {Y} [Vec R Y] (f : X → Sop Y) : Prop

  -- in principle we could provide this but we do not want to
  @[instance]
  opaque isntROHandleFVec : Vec R ROHandleF := sorry

  axiom getROAttrF.SopDifferentiable_rule {geo attr owner} : 
    SopDifferentiable R (fun _ : Unit => getROAttrF geo attr owner)

  axiom getROAttrF.sopDeriv_rule {geo attr owner} : 
    sopDeriv R (fun _ : Unit => getROAttrF geo attr owner)
    =
    (fun _ _ => getROAttrF geo ("d"++attr) owner)

  end Derivatives

end Hou
