/*--
Template for melee weapons. Include this in an object, that can should be used as a weapon.

@title Weapon
@author Marky
@version 0.1.0
--*/

#strict 2
#include LF_I

/* Is a melee weapon */

/**
 * Identifies the object as a melee weapon.
 * @return bool Default value: true.
 */
public func IsWeapon()
{
	return true;
}

/**
 * Identifies the object as a shield.
 * @return bool Default value: false.
 */
public func IsShield()
{
	return false;
}

/**
 * Set to true, if the weapon has a horizontal swing animation.
 * @param szStyle Fighting style string, see TODO.
 * @return bool Default value: true.
 */
public func CanSwing(string szStyle)
{
	return true;
}

/**
 * Set to true, if the weapon has a thrust animation.
 * @param szStyle Fighting style string, see TODO.
 * @return bool Default value: true.
 */
public func CanThrust(string szStyle)
{
	return true;
}

/**
 * Set to true, if the weapon has a vertical strike animation.
 * @param szStyle Fighting style string, see TODO.
 * @return bool Default value: true.
 */
public func CanStrike(string szStyle)
{
	return true;
}

/**
 * Set to true, if the weapon has a low swing animation.
 * @param szStyle Fighting style string, see TODO.
 * @return bool Default value: true.
 */
public func CanLow(string szStyle)
{
	return true;
}


/**
 * Set to true, if the weapon can be forged.
 * @return Default value: true.
 */
public func IsAnvilProduct()
{
	return true;
}

public func GetOverlayGraphics(object pObj)
{
	var gfx = "Walk";
	
	if (pObj)
		if (pObj)
		{
			var pos = pObj->~CombatIsWeaponDrawn(this);
			if (pos)
				gfx = Format("Fight%s", pos);
			else
			{
				pos = pObj->~CombatGetWeaponSlot(this);
				gfx = Format("Walk%s", pos);
			}
		}
	return gfx;
}

/**
 * This tells the character which animation to use when attacking.
 * @note You do not have to overwrite this function, but it may be useful if you design a weapon with only one attack.
 * @param szStyle The weapon fighting style, see TODO.
 * @param fPrimary Pass true, if the weapon is the primary weapon.
 * @return The weapon attack animation name.
 */
public func GetStrikeAction(string szStyle,bool fPrimary)
{
	var szAct = "Cancel", szSuffix;
	
	if (szStyle == "SpSh" && (GetStrikeStyles() & 1))
	{
		if (!fPrimary)
		{
			if (!IsShield())
				return "Cancel"; // Sword and buckler style, this means the weapon is always primary
			else
				szSuffix = "Sh";
		}
		else
		{
			if (IsShield())
				return "Cancel";
			else
				szSuffix = "Sw";
		}
	}
	if (szStyle == "SwSh" && (GetStrikeStyles() & 2))
	{
		if (!fPrimary)
			szSuffix = "S";
		else
			szSuffix = "P";
	}
	
	var aTypes = [];
	
	if (CanStrike(szStyle))
		PushBack("Strike", aTypes);
	if (CanThrust(szStyle))
		PushBack("Thrust", aTypes);
	if (CanSwing(szStyle))
		PushBack("Swing", aTypes);
	if (CanLow(szStyle))
		PushBack("Low", aTypes);
	

	szAct = Format("%s%s", aTypes[Random(GetLength(aTypes))], szSuffix);
	
	return Format("%s_%s", szStyle, szAct);
}


public func GetBlockAction(string szStyle, bool fPrimary)
{
	var szAct = "Parry", szSuffix;
		
	if (szStyle == "SpSh")
	{
		if (fPrimary)
			szSuffix = "Sw";
		else
			szSuffix = "Sh";
	}
	else
	{
		if (fPrimary)
			szSuffix = "P";
		else
			szSuffix = "S";
	}
	
	return Format("%s_%s%s", szStyle, szAct, szSuffix);
}

public func GetStrikePos(string szStyle, bool fPrimary)
{
	if (szStyle == "SpSh")
		return ["P", "L"];
	
	if (fPrimary)
		return ["P", "R"];
	else
		return ["S", "L"];
}


public func GetStrikeStyles()
{
	return 1 | 2;
}
public func GetStrikeDistance()
{
	return -2;
}
public func GetStrikeDamage()
{
	return 8;
}
