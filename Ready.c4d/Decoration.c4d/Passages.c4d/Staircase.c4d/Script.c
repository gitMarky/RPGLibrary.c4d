/*--Connects to castle part staircases.@title Staircase@version 0.1.0@author Unknown, possibly Randrian--*/
#strict 2
#include CPT1 // Staircase#include LF_D // Decoration
/* Staircase */
local target_up;
local target_down;/** * Connects the staircase to an object.  * * @par target The user exits at this object upon pressing ControlDown. */
public func SetTargetDown(object target)
{
	target_down = target;
}/** * Connects the staircase to an object.  * * @par target The user exits at this object upon pressing ControlUp. */
public func SetTargetUp(object target)
{
	target_up = target;
}
protected func Initialize()
{
	// Always open
	SetEntrance(1);
	// Transfer zone
	UpdateTransferZone();
}

private func FindStaircase(int ox, int oy)
{
	// Find other staircase in direction
	var obj;
	while (obj = FindObject(0, ox, oy, 0, 0, OCF_Fullcon, 0, 0, NoContainer(), obj))
		if (obj->~IsStaircase())
			return obj;
	// Found nothing
	return 0;
}
private func FindStaircaseAbove()
{
	if (target_up)
		return target_up;
	var obj = FindStaircase(0, -CastlePartHeight() / 2);
	if (obj)
		return obj;
	obj = FindStaircase(0, -CastlePartHeight() / 2 - 10);
	return obj;
}
private func FindStaircaseBelow()
{
	if (target_down)
		return target_down;
	var obj = FindStaircase(0, CastlePartHeight() / 2);
	if (obj)
		return obj;
	obj = FindStaircase(0, CastlePartHeight() / 2 + 10);
	return obj;
}
/* Status */
public func CastlePartHeight()
{
	return 140;
}
/* Transfer */
public func UpdateTransferZone()
{
	// Only if a staircase is found
	if (FindStaircaseAbove())
		if (GetOCF() & OCF_Fullcon)
			return SetTransferZone(-8, -80, 16, 60);
	// No interaction possible
	return SetTransferZone();
}

public func ControlTransfer(object pObj, int tx, int ty)
{
	// Ascend
	if (Inside(tx - GetX(), -20, 20) && Inside(ty - GetY(), -80, -50))
		if (Contained(pObj) == this || GetY(pObj) > GetY() - 50)
			return HandleTransferAscend(pObj, tx, ty);
	
	// Descend
	if (Inside(tx - GetX(), -20, 20) && Inside(ty - GetY(), -20, 20))
		if (Contained(pObj) == this || GetY(pObj) < GetY() - 50)
			return HandleTransferDescend(pObj, tx, ty);
	
	// Transfer not possible
	return 0;
}
