/*-- Day-night-cycle controller --*/

#strict 2
#include LF_C

global func AttachDayNightCycle(object pTarget, vCycle)
{
	var cycleController = CreateObject(DNCC, 0, 0, -1);
	cycleController->~Set(pTarget, vCycle);
}

public func Set(object target, vCycle)
{
	pTarget = target;
	SetAction("Attach", target);
	SetDayNightCycle(vCycle);
	SetVisibility(VIS_None);
}
