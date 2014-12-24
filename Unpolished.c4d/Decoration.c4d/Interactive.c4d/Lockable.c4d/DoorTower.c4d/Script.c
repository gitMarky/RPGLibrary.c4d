/*-- Lockable tower with door.
@author Randrian ({@link http://clonkforge.net/pr.php?pr=1415})
@version 0.2.0
--*/

#strict 2
#include LF_L // lockable

local unlocked;

public func OnSetLocked()
{
	SetAction("Locked");
}

/* Control */

protected func DoClose(object controller, bool is_AI)
{
	if (GetAction() == "DoorOpen")
	{
		SetAction("CloseDoor");
	}
	else if (GetAction() == "OpenDoor")
	{
		var phase = GetPhase();
		SetAction("CloseDoor");
		SetPhase(5 - phase);
	}
}

protected func DoOpen(object controller, bool is_AI)
{
	if (controller && GetAction() == "Locked")
	{
		if (is_AI)
		{
			var temp = unlocked;
		
			unlocked = true;
			SetAction("DoorClosed");
			unlocked = temp;
		}
		else
		{
			var key = Contents(0, controller);
			
			if (key) UnlockWithKey(key);
		}
	}
	
	if (GetAction() == "DoorClosed")
	{
		SetAction("OpenDoor");
	}
	else if (GetAction() == "CloseDoor")
	{
		var phase = GetPhase();
		SetAction("OpenDoor");
		SetPhase(5 - phase);
	}
}

protected func ControlLeft(object controller)
{
	[$MessageCloseDoor$|Image=GATL]
	
	if (GetDir() == 1)
		DoOpen(controller);
	else
		DoClose(controller);
}
 
protected func ControlRight(object controller)
{
	[$MessageOpenDoor$|Image=GATR]
	
	if (GetDir() == 1)
		DoClose(controller);
	else
		DoOpen(controller);
}
 
/* Kontext */ 
 
public func ContextDoorClose(object controller)
{
	[$MessageCloseDoor$|Image=GATL|Condition=IsOpen]
	SetCommand(controller, "Grab", this);
	AppendCommand(controller, "Call", this, 0, 0, 0, 0, "DoClose");
	return true;
}
 
public func ContextDoorOpen(object controller)
{
	[$MessageOpenDoor$|Image=GATR|Condition=IsClosed]
	SetCommand(controller, "Grab", this);
	AppendCommand(controller, "Call", this, 0, 0, 0, 0, "DoOpen");
	return true;
}

public func IsOpen()
{
	return GetAction() == "DoorOpen" || GetAction() == "OpenDoor";
}

public func IsClosed()
{
	return GetAction() == "DoorClosed" || GetAction() == "CloseDoor";
}
  
/* Initialisierung */

protected func Initialize()
{
	SetAction("DoorClosed");
}

public func SetDir(int dir)
{
	_inherited(dir, ...);
	CloseEntrance();
}

private func OpenEntrance()
{
	SetSolidMask(25, 0 + 74 * GetDir(), 15, 74, 5 * GetDir());
}

private func CloseEntrance()
{
	if (key_id && !unlocked && GetAction() == "DoorClosed")
		SetAction("Locked");
	SetSolidMask(40, 0 + 74 * GetDir(), 14, 74, 5 * GetDir());
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

private func OnUnlockWithKey(object key)
{
	if (key && swallow_key) RemoveObject(key);
	
	unlocked = true;
	
	SetAction("DoorClosed");
}