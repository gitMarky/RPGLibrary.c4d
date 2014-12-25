/*-- Is a lock and a switch!
@title Lock
@author Sven2 (assumed), Marky (script)
@version 0.2.0
--*/

#strict 2

#include LF_L
#include LF_S

static const gLock_State_Locked = 0;
static const gLock_State_Unlocked = 1;

/* Control */

public func IsLocked()
{
	return GetActiveState() == gLock_State_Locked;
}

public func ControlDig(object controller)
{
	[$MessageTurnKey$]
	TurnKey(controller);
	return true;
}
  
public func ControlThrow(object controller)
{
	[$MessageGetPutKey$]
	
	GetOrPutKey(controller);
	return true;
}

protected func TurnKey(object controller)
{
	if (!Contents())
	{
		Message("$MessageNoKey$", controller);
		return;
	}

	if (IsLocked())
	{
		UnlockWithKey(Contents());
	}
	else
	{
		LockWithKey(Contents());
	}
}

protected func OnPutKey(object controller)
{
	UpdateAction();
}

protected func OnGetKey(object controller)
{
	UpdateAction();
}

/* Collection */

protected func RejectCollect(id def, object key)
{
	if (!key || !key->~IsKey()) return true;
	if (key->~GetKeyID() != key_id)
	{
		Message("$MessageWrongKey$", Contained(key));
		return true;
	}
	return false;
}

public func OnUnlockWithKey(object key)
{
	SetState(gLock_State_Unlocked);
	UpdateAction();
}

public func OnSetLocked()
{
	OnLockWithKey();
}

public func OnLockWithKey(object key)
{
	SetState(gLock_State_Locked);
	UpdateAction();
}

public func UpdateAction()
{
	if (Contents())
	{
		if (GetAction() != "Key") SetAction("Key");

		SetDir(GetActiveState());
	}
	else
	{
		if (GetAction() != "Empty") SetAction("Empty");
		
		SetDir(0);
	}
}
