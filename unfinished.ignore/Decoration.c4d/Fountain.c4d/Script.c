/*-- Brunnen --*/

#strict

local y,helper,ziehen;

protected func Destruction() { RemoveHelper(); }

private func CreateHelper()
{
  RemoveHelper();
  helper= CreateObject(HLPR,0,+28);
  return(helper);
}

private func RemoveHelper()
{
  if(helper) RemoveObject(helper);
}

protected func ControlUp(pClonk)
{
  [$FillBucket$|WDBR]
  var line,bucket= FindBucket(pClonk);
  if(line= FindObject(PWRL,0,0,0,0,0,"Connect",helper))
    {
    SetPlrView(GetOwner(pClonk),GetActionTarget(0,line));
    ziehen= 1;
    EimerHochZiehen(GetActionTarget(0,line));
    return(0);
    }
  if(!bucket) return(Message("$NoBucket$",this()));
  if(!PathFree(GetX(),GetY()+38,GetX(),GetY()+46))  return(Message("$NoRope$",this()));
  Exit(bucket);
  SetPosition(GetX(),GetY() +43,bucket);
  SetPlrView(GetOwner(pClonk),bucket);
  ObjectSetAction(CreateObject(PWRL,-GetX() +GetX(bucket),-GetY() +GetY(bucket)-7,GetOwner()),"Connect",bucket,CreateHelper());
  return(1);
}

private func EimerHochZiehen(pBucket)
{
  if(!ziehen) return(0);
  if(!pBucket) { ziehen= 0; return(0); }
  if(!FindObject(0,0,0,0,0,OCF_CrewMember,"Push",this())){
      return(ziehen=0);
  }
  if(ObjectDistance(helper,pBucket)< 15)
    {
    var clonk,line;
    if(line= FindObject(PWRL,0,0,0,0,0,"Connect",helper)) RemoveObject(line);
    if(clonk= FindObject(0,0,0,0,0,OCF_CrewMember(),"Push",this())) Enter(clonk,pBucket);    
    else SetPosition(GetX(),GetY() +15,pBucket);
    ziehen= 0;
    return(1);
    }
  SetPosition(GetX(pBucket),GetY(pBucket)-2,pBucket);
  SetYDir(-15,pBucket);
  ScheduleCall(this(),"EimerHochZiehen",10,0,pBucket);
}

private func FindBucket(pContainer)
{
  if(FindContents(WDBR,pContainer))
    return(FindContents(WDBR,pContainer));
  return(FindContents(WDWB,pContainer));
}

protected func Grabbed(object pClonk, bool fGrab)
{
  if(pClonk->~IsCarryingCorps())
  {
    var pCorps;
    pCorps = pClonk->~GetCorps(1);
    pCorps->SetPosition(GetX(),GetY() +43);
    Sound("Splash1");
    return 1;
  }
}