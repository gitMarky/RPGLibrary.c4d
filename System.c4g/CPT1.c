/*-- Treppenhaus --*/

#strict 2

#appendto CPT1 // Treppenhaus

/* Treppenhaus */

local pTargetUp;
local pTargetDown;

func SetTargetDown(obj) { pTargetDown = obj; }
func SetTargetUp(obj) { pTargetUp = obj; }

private func FindStaircaseAbove()
{
	if(pTargetUp) return pTargetUp;
	return _inherited();
}

private func FindStaircaseBelow()
{
	if(pTargetDown) return pTargetDown;
	return _inherited();
}
