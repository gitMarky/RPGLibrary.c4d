/*-- Lockable tower with door.
@author Randrian ({@link http://clonkforge.net/pr.php?pr=1415})
@version 0.2.0
--*/

#strict 2

local iRefID;
local unLocked;

func SetLocked(int iID)
{
	iRefID = iID;
	SetAction("DoorLocked");
}

/* Torsteuerung */

protected func DoClose(pClonk, fKi)
{
  if(GetAction() == "DoorOpen")
		SetAction("CloseDoor");
	if(GetAction() == "OpenDoor")
	{
		var phase = GetPhase();
		SetAction("CloseDoor");
		SetPhase(5-phase);
	}
}

protected func DoOpen(pClonk, fKi)
{
	if(pClonk && GetAction() == "DoorLocked")
	{
		if(fKi)
		{
			unLocked = 1;
			SetAction("DoorClosed");
			unLocked = 0;
		}
		else
		{
			var pQuest = pClonk->~GetQuestStorage();
			var find = Find_Container(pClonk);
			if(pQuest) find = Find_Container(pQuest);
			for(var pObj in FindObjects(find, Find_Func("IsKey")))
					if(LocalN("iRefID", pObj) == iRefID)
					{
						unLocked = 1;
						SetAction("DoorClosed");
					}
			if(GetAction() == "DoorLocked")
				Message("Tür ist abgesperrt!", pClonk);
		}
	}
  if(GetAction() == "DoorClosed")
		SetAction("OpenDoor");
	if(GetAction() == "CloseDoor")
	{
		var phase = GetPhase();
		SetAction("OpenDoor");
		SetPhase(5-phase);
	}
}

protected func ControlLeft(pClonk)
{
  [Tür schließen|Image=GATL]
	if(GetDir() == 1)
		DoOpen(pClonk);
	else
		DoClose(pClonk);
}

func IsOpen() { return GetAction() == "DoorOpen" || GetAction() == "OpenDoor"; }
 
protected func ControlRight(pClonk)
{
  [Tür öffnen|Image=GATR]
	if(GetDir() == 1)
		DoClose(pClonk);
	else
		DoOpen(pClonk);
}

func IsClosed() { return GetAction() == "DoorClosed" || GetAction() == "CloseDoor"; }
 
/* Kontext */ 
 
public func ContextDoorClose(object pClonk)
{
  [Tür schließen|Image=GATL|Condition=IsOpen]
  SetCommand(pClonk, "Grab", this());
  AppendCommand(pClonk, "Call", this(), 0,0,0,0, "DoClose");  
  return(1); 
}
 
public func ContextDoorOpen(object pClonk)
{
  [Tür öffnen|Image=GATR|Condition=IsClosed]
  SetCommand(pClonk, "Grab", this());
  AppendCommand(pClonk, "Call", this(), 0,0,0,0, "DoOpen");
  return(1); 
}
  
/* Initialisierung */

protected func Initialize()
{
	SetAction("DoorClosed");
  // Fertig
  return(1);
}

func SetDir(int iDir)
{
	_inherited(iDir, ...);
	CloseEntrance();
}

private func OpenEntrance()
{
  SetSolidMask(25,0+74*GetDir(),15,74,5*GetDir());
}

private func CloseEntrance()
{
	if(iRefID && !unLocked && GetAction() == "DoorClosed") SetAction("DoorLocked");
  SetSolidMask(40,0+74*GetDir(),14,74,5*GetDir());
}

private func SoundOpenDoor()
{
	OpenEntrance();
  Sound("DoorOpen");
}

private func SoundCloseDoor()
{
  Sound("DoorClose");
}