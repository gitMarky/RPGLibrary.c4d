/* Expanded the castle part staircase: It is now possible to set a target for ascending or descending the staircase. */

#strict 2

#appendto CPT1

/* Staircase */

local target_ascend;
local target_descend;

/**
Overrides the target when descending the staircase.
@par target The target.
 */
public func SetTargetDown(object target)
{
	target_descend = target;
}

/**
Overrides the target when ascending the staircase.
@par target The target.
 */
public func SetTargetUp(object target)
{
	target_ascend = target;
}

private func FindStaircaseAbove()
{
	if (target_ascend)
		return target_ascend;
		
	return _inherited();
}

private func FindStaircaseBelow()
{
	if (target_descend)
		return target_descend;
		
	return _inherited();
}
