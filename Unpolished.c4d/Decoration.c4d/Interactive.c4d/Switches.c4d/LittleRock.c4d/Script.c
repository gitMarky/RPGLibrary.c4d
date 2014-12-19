/*-- A floor switch. It can have the state 0 and 1 (hardcoded). The effect of the switches is still up to you.	
@author Gecko
@version 0.2.0
 --*/

#strict 2

#include LF_S // Switch

local activation_mass;
local stays_locked;

static const gFloorButton_NeededWeight = 10;
static const gFloorButton_Infinite_Lock = -1;

public func Initialize()
{
	activation_mass = gFloorButton_NeededWeight;
	stays_locked = gFloorButton_Infinite_Lock;
}

/**
 The switch is activated if objects with the given mass are on the switch.
 @par mass The total mass of all objects that are on the switch. Must be >= 1.
 @return The new activation mass.
 */
public func SetActivationMass(int mass)
{
	if (activation_mass < 1)
	{
		ErrorLog("Activation mass must be >= 1, you passed %d", mass);
		return false;
	}
	
	return activation_mass = mass;
}

/**
 By default the button stays pressed once it is activated. With this function you can adjust the timer.
 @par interval The switch stays pressed at least this many frames. Only multiples of 10 make sense here,
 because the timer is checked every 10 frames. {@c gFloorButton_Infinite_Lock} locks the button forever.
 */
public func SetLockDuration(int interval)
{
	stays_locked = interval;
}

protected func GetMassOnSwitch()
{
	var mass;
	for (var pObj in FindObjects
		(
			Find_Or(Find_AtPoint(-3, -7), Find_AtPoint(0, -7), Find_AtPoint(3, -7)),
			Find_NoContainer(),
			Find_Exclude(this)
		)) 
	{
		if (pObj->GetContact(0, -1, 8))
			mass += pObj->~GetMass();
	}
	
	return mass;
}

protected func CheckWhileUp()
{
	if (GetMassOnSwitch() >= activation_mass)
	{
		if (GetActiveState() == 0)
		{
			SetAction("Down");
			Sound("Click");
		}
	}
}

protected func OnDown()
{
	SetState(1);
}

public func CheckWhileDown()
{
	if (stays_locked == gFloorButton_Infinite_Lock) return;

	if (GetMassOnSwitch() < activation_mass)
	{
		if (GetActiveState() == 1 && GetActTime() > stays_locked)
		{
			SetAction("Up");
			Sound("Click");
		}
	}
}

protected func OnUp()
{
	SetState(0);
}

public func CheckSolidmask()
{
	if (GetAction() == "Down")
	{
		SetSolidMask(GetPhase() * 11, 0, 11, 12);
	}
	else if (GetAction() == "Up")
	{
		SetSolidMask(22 - GetPhase() * 11, 0, 11, 12);
	}
}