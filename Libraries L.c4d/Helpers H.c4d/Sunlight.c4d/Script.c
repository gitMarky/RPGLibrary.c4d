/*-- Sonnenlicht --*/

#strict 2

protected func Initialize()
{
  SetAction("Check");
}

public func Attach(pTarget)
{
  SetAction("Attach", pTarget);
}

global func AddPlrViewRange(iRange,pObj)
{
	if(!pObj) return;
	var light = CreateObject( _SUL,0,0,-1 );
	light->~Attach( pObj );
	SetPlrViewRange(iRange,light);
}

global func DelPlrViewRange(pObj)
{
	if(!pObj) return;
	var lights = FindObjects( Find_ID(_SUL), Find_Action("Attach"), Find_ActionTarget(pObj) );
	for( var light in lights ) RemoveObject(light);
}

public func AttachTargetLost() { RemoveObject(); }

global func Find_PlayerType(int iType)
{
  var i = GetPlayerCount(iType);
  var aFind = [C4FO_Or];
  while(i--)
    aFind[GetLength(aFind)] = Find_Owner(GetPlayerByIndex(i, iType));
  return aFind;
}

public func Activate()
{
  SetAction("Idle");
  SetPlrViewRange(150);
}

public func Check4Clonk()
{
  var clnk;
  if(FindObject2(Find_InRect(-200,-200,400,400),Find_PlayerType(C4PT_User),Find_OCF(OCF_CrewMember)))
  {
    SetAction("Idle");
    SetPlrViewRange(150);
  }
  return 1;
}

global func PlaceSunLight()
{
	for(var obj in FindObjects(Find_ID(_SUL)))
		obj->RemoveObject();
	var fSecondRow, y, x;
	var dist = 120;
	for(y=0; y<LandscapeHeight(); (y+=dist) & (fSecondRow=!fSecondRow))
	for(x=0+dist*fSecondRow/2; x<LandscapeWidth(); x+=dist)
	{
			if(GetMaterial(x,y)==-1 || GetMaterial(x,y) == Material("Vehicle")) CreateObject(_SUL,x,y,-1);
	}
}
