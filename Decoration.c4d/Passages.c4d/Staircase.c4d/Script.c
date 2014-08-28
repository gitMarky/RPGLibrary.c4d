/*-- Staircase --*/
#strict 2
#include CPT1 // Staircase
/* Staircase */
local pTargetUp;
local pTargetDown;
public func SetTargetDown(obj)
{
	pTargetDown = obj;
}
public func SetTargetUp(obj)
{
	pTargetUp = obj;
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
	if (pTargetUp)
		return pTargetUp;
	var obj = FindStaircase(0, -CastlePartHeight() / 2);
	if (obj)
		return obj;
	obj = FindStaircase(0, -CastlePartHeight() / 2 - 10);
	return obj;
}
private func FindStaircaseBelow()
{
	if (pTargetDown)
		return pTargetDown;
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
