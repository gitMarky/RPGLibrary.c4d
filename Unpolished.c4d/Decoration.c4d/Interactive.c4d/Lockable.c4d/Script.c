/*--
Include this library if you want an object to be unlockable by a key. While 
locked the object has the action "Locked".
@title Lockable
@author Marky
@version 0.2.0
--*/

#strict 2

local key_id;

local swallow_key;


protected func IsLocked()
{
	return GetAction() == "Locked";
}

protected func RejectCollect(id def, object obj)
{
	if (IsLocked())
	{
		// never collect anything but try unlocking it
		UnlockWithKey(obj);
		return true;
	}
	else
	{
		return _inherited();
	}
}

private func SetLocked(id key, bool remove_key)
{
	ChangeAction("Locked");
	swallow_key = remove_key;
}

private func UnlockWithKey(object key)
{
	if (!key || !key->~IsKey()) return;
	if (!IsLocked()) return Message("$MessageNotLocked$", Contained(key));
	if (key->~GetKeyID() != key_id) return Message("$MessageWrongKey$", Contained(key));

	this->~OnUnlockWithKey(key);
}

private func LockWithKey(object key)
{
	if (!key || !key->~IsKey()) return;
	if (IsLocked()) return Message("$MessageLocked$", Contained(key));
	if (key->~GetKeyID() != key_id) return Message("$MessageWrongKey$", Contained(key));

	ChangeAction("Locked");
	this->~OnLockWithKey(key);
}
  
