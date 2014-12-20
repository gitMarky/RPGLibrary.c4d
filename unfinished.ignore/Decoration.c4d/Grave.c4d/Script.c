/*-- Wegweiser --*/

#strict 2

local pTarget;

global func HideGraves()
{
	for(var pGrave in FindObjects(Find_ID(1GRV)))
	{
		pGrave->SetHide();
	}
}

func SetHide()
{
	if(GetAction() == "Occupied") return;
	SetAction("Free");
	SetObjectLayer(this);
}

func SetOccupied(pObj)
{
	SetAction("Occupied");
	pTarget = pObj;
	SetObjectLayer();
}

protected func ControlUp(object pPlayer)
  { [$CtrlRead$]
  var pGhost = Contents();
	if(pGhost && GetID(pGhost)==GHST)
		return pGhost->Activate();
  // Spieler vorbereiten (Action halten, weil Push)
	var sText = Format("Hier ruht in Frieden %s", GetName(pTarget));
	if(!pTarget) sText = "Das Grab ist leer.";
  var szPortrait = Format("Portrait:%i::%x::%s", GetID(), GetColorDw(), "1");
  CreateMenu(GetID(),pPlayer,this,0,Format("<c %x>%s:</c>", GetColorDw(), GetName()),0,C4MN_Style_Dialog);
  AddMenuItem(szPortrait,0,NONE,pPlayer,0,0,0,5);
  AddMenuItem(sText,0,NONE,pPlayer);
  AddMenuItem("Abbrechen", "Stop", MCMX, pPlayer, 0, pPlayer);
  return 1;
  }
  
func WaitingTimer()
{
	if(ObjectCount2(Find_ID(GetID()), Find_Action("Occupied")) >= Local(0))
	{
		var pGhost = CreateContents(GHST);
		pGhost->LocalN("idName") = Local(1);
		SetAction("Occupied");
		SetObjectLayer();
	}
}

public func Stop() { }
  
public func GetDlgMsgColor() { return 10778899; }
