/*
Implements the Hazard 2 damage system.
@title Damage System
@author Hazard Team
@version 0.1.0
*/

#strict 2

//Schadens-Konstanten
static const DMG_Melee      = 1;
static const DMG_Projectile = 2;
static const DMG_Fire       = 3;
static const DMG_Explosion  = 4;
static const DMG_Energy     = 5;
static const DMG_Bio        = 6;

/**
 * Inflicts damage to an object. DoDmg replaces DoEnergy und DoDamage. 
 * It can damage objects (DoDamage) and living beings (DoEnergy). Whenever damage should be inflicted (and not healed/repaired),
 * use DoDmg, since this also provides the damage type. Various objects may be resistant to certain kinds of damage.
 * @param iDmg The amount of damage.
 * @param iType. The damage type. 
 * @param pTarget. [optional] Object that is to be damaged. 0 in local call.
 * @param iPrecision [opt] Precision. No value or 0 defaults to 10.
 * @param [opt] Player that caused the damage, base is 1. Passing 0 means, that no player damaged the object.
 * @note Calls
 * {@code
 * public int OnDmg(int iDmg, int iType)
 * }
 * In the object to get the damage reduction in percent. If OnDmg() returns 10, then the damage is reduced to 90%.@br
 * Calls
 * {@code
 * public LastDamageType(int iType)
 * }
 * In the object to save the last damage type.
 */
global func DoDmg(int iDmg, int iType, object pTarget, int iPrecision, int iByPlayer)
{
	if (!pTarget)
		if (!(pTarget = this))
			return 0;
			
	if (!iPrecision)
		iPrecision = 10;
	
	var dmg;
	var dmgdealer = iByPlayer - 1;
	if (dmgdealer < 0)
		dmgdealer = GetController(pTarget);
	
	var red = pTarget->~OnDmg(iDmg, iType); //reduction
	// reduction
	dmg = iDmg * (100 - red) * iPrecision;
	
	// Killer setzen
	if (this && pTarget->GetOCF() & OCF_CrewMember || iByPlayer)
	{
		SetKiller(dmgdealer, pTarget);
	}
	
	//Schaden machen
	var pFrom;
	
	pFrom = this;
	pTarget->~LastDamageType(iType);
	pTarget->~OnHit(dmg / 1000, iType, pFrom, dmg);
	
	
	if (GetCategory(pTarget) & C4D_Living)
		return DoEnergy(-dmg, pTarget, true);
		
	return DoDamage(dmg / 1000, pTarget);
}
