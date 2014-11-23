/*-- Expanded the castle part staircase: It is now possible to set a target for ascending or descending the staircase. --*/

#strict 2

#appendto CPT1 // Treppenhaus

/* Staircase */

local pTargetUp;
local pTargetDown;

/**
 * Overrides the target when descending the staircase.
 * @param target The target.
 */
public func SetTargetDown(object target)
{
	pTargetDown = target;
}

/**
 * Overrides the target when ascending the staircase.
 * @param target The target.
 */
public func SetTargetUp(object target)
{
	pTargetUp = target;
}

private func FindStaircaseAbove()
{
	if (pTargetUp)
		return pTargetUp;
	return _inherited();
}

private func FindStaircaseBelow()
{
	if (pTargetDown)
		return pTargetDown;
	return _inherited();
}
