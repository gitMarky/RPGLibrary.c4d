#strict 2
#include LF_C

/**
@param cycle A day-night-cycle definition array.
@param target [optional] The target to attach the day-night-cycle to. No value or 0
referrs to the local object.
@author Marky
@version 0.1.0
*/
global func AttachDayNightCycle(object target, cycle)
{
	var obj = target;
	if (!target) obj = this;
	if (!obj)
	{
		ErrorLog("AttachDayNightCycle() target is not valid - probably because of call from global context.");
	}

	var cycle_controller = CreateObject(ID_Helper_DayNightCycleController, 0, 0, -1);
	cycle_controller->~Set(target, cycle);
}

public func Set(object target, cycle)
{
	cycle_target_object = target;
	SetAction("Attach", target);
	SetDayNightCycle(cycle);
	SetVisibility(VIS_None);
}
