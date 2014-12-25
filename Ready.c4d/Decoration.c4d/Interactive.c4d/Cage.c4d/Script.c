/*-- Holds prisoners captive, or other things.
@title Cage
@author Sven2 (assumed)
@version 0.2.0
--*/

#strict 2
#include LF_D
#include LF_L

protected func IsLocked()
{
	return GetAction() != "Open";
}

protected func ControlThrow(object controller)
{
	[$CtrlOpen$|Image=_KEY]
	
	if (key_id)
	{
		GetOrPutKey(controller);
	}
	else
	{
		OnUnlockWithKey();
	}
	return true;
}

private func OnPutKey(object controller, object key)
{
	if (IsLocked()) UnlockWithKey(key);
	return;
}

private func OnUnlockWithKey(object key)
{
	Sound("Connect", this);
	Sound("ChestOpening", this);
	SetAction("Open");
	SetSolidMask(0,14,46,4,0,14);
}