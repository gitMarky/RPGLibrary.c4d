/*-- 
This touch plate is sensitive to weight placed on it.

@title Floor switch
@author Unknown
@version 0.1.0
--*/

#include LF_S // Switch

#strict 2

static const gFloorSwitch_NeededWeight = 20;
static const gFloorSwitch_DownPos      = 4;

local y_pos_current;
local specific_id;	 // only items with matching ID are counted
local activation_mass;
local master_id;		 // Super ID which handles the floor switch with one object
local y_pos_bottom;	 // additional lower position

protected func Initialize()
{
	y_pos_bottom = gFloorSwitch_DownPos;
}

protected func Timer10()
{
	// Calculate position by current weight
	var mass, target;
	while (target = FindObject(specific_id, -25, -30, 50, 30, 0, 0, 0, NoContainer(), target))
		if (target->GetContact(0, -1, 8))
			if (~target->GetCategory() & C4D_StaticBack)
				mass += target->GetMass();
				
	if (master_id)
		if (target = FindObject(master_id, -25, -30, 50, 30, 0, 0, 0, NoContainer()))
			if (target->GetContact(0, -1, 8))
				if (~target->GetCategory() & C4D_StaticBack)
					mass += activation_mass + gFloorSwitch_NeededWeight;
					
	var y_pos_desired = 0;
	
	// Check weight, either use own value or default value
	if (activation_mass)
	{
		if (mass >= activation_mass)
			y_pos_desired = y_pos_bottom;
	}
	else if (mass >= gFloorSwitch_NeededWeight)
		y_pos_desired = y_pos_bottom;
	
	// At desired position?
	if (y_pos_desired == y_pos_current)
		return;
	
	// No target? "stuck"!
	if (GetActiveState() == gSwitch_State_Inactive)
	{
		y_pos_current = y_pos_desired;
		Sound("ArrowHit");
		Message("$MsgStuck$", this);
		return;
	}
	
	// Sound when the switch starts moving
	if (!y_pos_current || y_pos_current == gFloorSwitch_DownPos)
		Sound("Elevator");
	
	// Move up/down
	y_pos_desired = BoundBy(y_pos_desired - y_pos_current, -1, 1);
	
	while (target = FindObject(0, -25, -30, 50, 30, 0, 0, 0, NoContainer(), target))
		if (target->GetContact(0, -1, 8))
			if (~target->GetCategory() & C4D_StaticBack)
				target->SetPosition(target->GetX(), target->GetY() + y_pos_desired);
	
	SetPosition(GetX(), GetY() + y_pos_desired);
	y_pos_current += y_pos_desired;
	
	// notify targets
	if (y_pos_desired > 0)
	{
		SetState(1);
	}
	else if (y_pos_desired < 0)
	{
		SetState(0);
	}
	
	// Sound when stopping
	if (!y_pos_current || y_pos_current == gFloorSwitch_DownPos)
	{
		Sound("Chuff");
	}
}

/**
 * Determines, how far the plate is pushed into the ground when the full weight is placed on it.
 * @par distance The plate moves downwards this many pixels. The default distance is 4 pixels.
 */
public func SetMoveDistance(int distance)
{
	y_pos_bottom = distance;
}

/**
 * Sets the weight value that is needed for the floor switch to go off.
 * @par mass The total weight value of objects that lie on top of the switch. The default weight is 20.
 */
public func SetNeededWeight(int mass)
{
	activation_mass = mass;
}

/**
 * Limits the weight detection to objects of a certain ID.
 * @par definition Only objects with this ID count towards the weight limit. 0 resets the specific ID.
 */
public func SetSpecificID(id definition)
{
	specific_id = definition;
}

/**
 * Sets a master ID for the switch.
 * @par definition Triggers the switch if at least one object of that ID is on the plate. 0 removes the master ID.
 */
public func SetMasterID(id definition)
{
	master_id = definition;
}