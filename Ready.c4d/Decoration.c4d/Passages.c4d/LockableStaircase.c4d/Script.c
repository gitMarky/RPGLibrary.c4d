/*--Connects to castle part staircases.@title Lockable Staircase@version 0.2.0@author Marky, based on staircase--*/
#strict 2
#include _SCS // Staircase#include LF_L // lockablelocal unlocked;
/* Staircase */
protected func Initialize()
{
	// Not open
	SetEntrance(0);
	// Transfer zone
	UpdateTransferZone();	// Action	SetAction("DoorClosed");
}public func ActivateEntrance(object controller){	DoOpen(controller);	return true;}public func OnSetLocked(){	SetAction("Locked");}protected func DoClose(object controller, bool is_AI){	if (GetAction() == "DoorOpen")	{		SetAction("CloseDoor");	}	else if (GetAction() == "OpenDoor")	{		var phase = GetPhase();		SetAction("CloseDoor");		SetPhase(5 - phase);	}}protected func DoOpen(object controller, bool is_AI){	if (controller && GetAction() == "Locked")	{		if (is_AI)		{			var temp = unlocked;					unlocked = true;			SetAction("DoorClosed");			unlocked = temp;		}		else		{			var key = Contents(0, controller);						if (key) UnlockWithKey(key);		}	}		if (GetAction() == "DoorClosed")	{		SetAction("OpenDoor");	}	else if (GetAction() == "CloseDoor")	{		var phase = GetPhase();		SetAction("OpenDoor");		SetPhase(5 - phase);	}}public func IsOpen(){	return GetAction() == "DoorOpen" || GetAction() == "OpenDoor";}public func IsClosed(){	return GetAction() == "DoorClosed" || GetAction() == "CloseDoor";}private func OpenEntrance(){	SetEntrance(1);}private func CloseEntrance(){	if (key_id && !unlocked && GetAction() == "DoorClosed") SetAction("Locked");	SetEntrance(0);}private func SoundOpenDoor(){	Sound("DoorOpen");}private func SoundCloseDoor(){	Sound("DoorClose");}private func OnUnlockWithKey(object key){	if (key && swallow_key) RemoveObject(key);		unlocked = true;		SetAction("DoorClosed");}
