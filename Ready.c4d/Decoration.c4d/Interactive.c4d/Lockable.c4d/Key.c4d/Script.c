/*-- Put it in a lock, turn it around, see what happens (usually nothing!).
@author Sven2 (assumed)
@version 0.2.0
--*/

#strict 2

static const gKey_Effect_Name = "IntKeyMarker";

/* Key */

protected func Initialize()
{
	SetAction("Key1");
}

protected func Hit()
{
	Sound("Crystal*");
}

/* Status */

/**
 Can be called on any object.
 @return The effect number of a "IntKeyMarker" effect of the object.
 */
global func IsKey()
{
	if (!this) return false;
	
	return GetEffect(gKey_Effect_Name, this);
}

/**
 Ask the key ID of an object.
 @return 0, if the object is not a key, or the key ID if it is one.
 @see BecomeKey()
 */
global func GetKeyID()
{
	if (!this) return false;

	var key = IsKey();
	
	if (key) return EffectVar(0, this, key);
}

/**
 Makes the object a key that can be used to open certain locks.
 @par keyID The lock requests this key ID from the
  key. It can be any value, even a C4ID if you like.
  A value of {@c 0} makes the object not a key anymore.
  @par color [optional] Color modulation for the key, in dword format.
  @par target [optional] The object that should become the key. 0 is the calling object.
 */
global func BecomeKey(keyID, int color, object target)
{
	if (!target) if (!(target = this)) return false;
	
	var key = IsKey();
	
	if (color)
	{
		SetClrModulation(color, target);
	}
	
	if (key)
	{
		
		if (keyID)
		{
			EffectVar(0, target, key) = keyID;
		}
		else
		{
			RemoveEffect(key, this);
		}
		
		return true;
	}
	else
	{
		if (keyID)
		{
			AddEffect(gKey_Effect_Name, target, 50, 0, 0, _KEY, keyID);
			return true;
		}
	}
	
	return false;
}

public func FxIntKeyMarkerStart(object target, int effect_nr, int temp, keyID)
{
	if (!temp)
	{
		EffectVar(0, target, effect_nr) = keyID;
	}
}

