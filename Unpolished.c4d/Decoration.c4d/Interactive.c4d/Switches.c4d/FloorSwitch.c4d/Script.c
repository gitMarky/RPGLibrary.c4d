/*-- 
This touch plate is sensitive to weight placed on it.

@title Floor switch
@author Unknown
@version 0.1.0
--*/

#strict 2

static const gFloorSwitch_NeededWeight = 20;
static const gFloorSwitch_DownPos      = 4;

local iYPos;
local idSpecific;	 // only items with matching ID are counted
local iNeededWeight;
local idMaster;		 // Super ID which handles the floor switch with an object
local iYPosBottom;	 // additional lower position
local aTargets;

protected func Initialize()
{
	aTargets = CreateArray();
	iYPosBottom = gFloorSwitch_DownPos;
}

protected func Timer10()
{
	// Calculate position by current weight
	var iWgt, obj;
	while (obj = FindObject(idSpecific, -25, -30, 50, 30, 0, 0, 0, NoContainer(), obj))
		if (obj->GetContact(0, -1, 8))
			if (~obj->GetCategory() & C4D_StaticBack)
				iWgt += obj->GetMass();
				
	if (idMaster)
		if (obj = FindObject(idMaster, -25, -30, 50, 30, 0, 0, 0, NoContainer()))
			if (obj->GetContact(0, -1, 8))
				if (~obj->GetCategory() & C4D_StaticBack)
					iWgt += iNeededWeight + gFloorSwitch_NeededWeight;
					
	var iDesiredY = 0;
	
	// Check weight, either use own value or default value
	if (iNeededWeight)
	{
		if (iWgt >= iNeededWeight)
			iDesiredY = iYPosBottom;
	}
	else if (iWgt >= gFloorSwitch_NeededWeight)
		iDesiredY = iYPosBottom;
	
	// At desired position?
	if (iDesiredY == iYPos)
		return;
	
	// No target? "stuck"!
	if (!GetLength(aTargets))
	{
		iYPos = iDesiredY;
		Sound("ArrowHit");
		Message("$MsgStuck$", this);
		return;
	}
	
	// Sound when the switch starts moving
	if (!iYPos || iYPos == gFloorSwitch_DownPos)
		Sound("Elevator");
	
	// Move up/down
	iDesiredY = BoundBy(iDesiredY - iYPos, -1, 1);
	
	while (obj = FindObject(0, -25, -30, 50, 30, 0, 0, 0, NoContainer(), obj))
		if (obj->GetContact(0, -1, 8))
			if (~obj->GetCategory() & C4D_StaticBack)
				obj->SetPosition(obj->GetX(), obj->GetY() + iDesiredY);
	
	SetPosition(GetX(), GetY() + iDesiredY);
	iYPos += iDesiredY;
	
	// Notify connected objects
	for (var target in aTargets)
	{
		var obj = target[0];
		var direction = target[1];
	
		if (iDesiredY * direction > 0)
			obj->ControlDown(this);
		else
			obj->ControlUp(this);
	}
	
	// Sound when stopping
	if (!iYPos || iYPos == gFloorSwitch_DownPos)
		Sound("Chuff");
}

/**
 * Adds a target to the switch. When the switch is at its bottom most position the target receives a function call.
 * @par pTarget The object that will receive the command.
 * @par iDir [optional] The switch calls ControlDown() in the target if iDir is 1, and ControlUp() if iDir is -1. If no parameter is passed, then ControlDown() is the default call.
 */
public func AddTarget(object pTarget, int iDir)
{
	if (iDir != 0 && iDir != 1) return;
	if (!iDir)
		iDir = 1;
	var i;
	while (Local(i))
		++ ++i;
	Local(i++) = pTarget;
	Local(i) = iDir;
}

/**
 * Identifies this object as a switch.
 * @return bool true.
 */
public func IsSwitch()
{
	return true;
}

/**
 * Determines, how far the plate is pushed into the ground when the full weight is placed on it.
 * @par iYDistance The plate moves downwards this many pixels. The default distance is 4 pixels.
 */
public func SetMoveDistance(int iYDistance)
{
	iYPosBottom = iYDistance;
}

/**
 * Sets the weight value that is needed for the floor switch to go off.
 * @par iWeight The total weight value of objects that lie on top of the switch. The default weight is 20.
 */
public func SetNeededWeight(int iWeight)
{
	iNeededWeight = iWeight;
}

/**
 * Limits the weight detection to objects of a certain ID.
 * @par definition Only objects with this ID count towards the weight limit. 0 resets the specific ID.
 */
public func SetSpecificID(id definition)
{
	idSpecific = definition;
}

/**
 * Sets a master ID for the switch.
 * @par definition Triggers the switch if at least one object of that ID is on the plate. 0 removes the master ID.
 */
public func SetMasterID(id definition)
{
	idMaster = definition;
}