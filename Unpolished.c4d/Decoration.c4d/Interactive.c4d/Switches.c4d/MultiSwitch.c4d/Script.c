/*-- Schalter --*/

#strict 2
#include LF_S

local playing_animation;

protected func Initialize()
{
	SetAction("Switch");
	playing_animation = false;
}

public func StartInactive()
{
	SetPhase(6);
	active_state = gSwitch_State_Inactive;
}

protected func UpdatePhase()
{
	if (active_state == gSwitch_State_Inactive)
	{
		SetPhase(6);
	}
	else
	{
		SetPhase(active_state);
	}
}


protected func ControlRight(object controller)
{
	[$DescSwitch$]
	return ControlSwitch(controller, +1);
}

protected func ControlLeft(object controller)
{
	[$DescSwitch$]
	return ControlSwitch(controller, -1);
}

protected func ControlSwitch(object controller, int change)
{
	if (playing_animation) return true; // block input
	
	var success = false;
	
	success = OperateSwitch(-1);
	
	if (!success && RequiresSuccessfulCalls())
	{
		return MessageBlocked(controller);
	}

	Sound("Click");
	return true;
}

private func OperateSwitch(int change)
{
	var old_state = GetActiveState();
	var new_state = BoundBy(old_state + change, 0, 12);
	
	if (old_state == new_state) return false;
	
	// pre-filter, because the original switch script
	// does not support jumping over non-existing states
	
	var does_new_state_exist = false; 
	for (var i = 0; i < GetLength(switch_states); i++)
	{
		if (switch_states[i][gSwitch_Def_State] == new_state)
		{
			does_new_state_exist = true;
			break;
		}
	}
	
	if (!does_new_state_exist)
	{
		playing_animation = true;
		
		SetPhase(new_state);
		
		ScheduleCall(this, "OperateSwitch", 3, 0, change);
		
		return true;
	}
	else
	{
		playing_animation = false;
		
		return SetState(new_state);
	}
}
