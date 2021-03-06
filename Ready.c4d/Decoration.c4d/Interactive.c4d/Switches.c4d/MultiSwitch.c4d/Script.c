/*--
A switch that can have up to 13 different state (0 to 12).
Switching left and right operates the switch. Each state corresponds with
one phase of the switch object.@br
Pressing left and right goes to the next available state. For example if 
state 0, 6 and 12 are defined, then the {@c ControlRight} shifts the state
to 6 on the first press and 12 on the second.
@author Unknown (original graphics), Marky (Script)
@version 0.2.0
 --*/

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
	active_state = 6;
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
	
	success = OperateSwitch(GetActiveState(), change);
	
	if (!success && RequiresSuccessfulCalls())
	{
		return MessageBlocked(controller);
	}

	Sound("Click");
	return true;
}

public func OperateSwitch(int old_state, int change)
{
	var new_state = BoundBy(old_state + change, 0, 12);
	
	if (old_state == new_state)
	{
		//DebugLog("Cannot change state: old %d == new: %d", old_state, new_state);
		return false;
	}
	
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
	
	//DebugLog("Does switch state %d exist? %v", new_state, does_new_state_exist);
	
	if (!does_new_state_exist)
	{
		playing_animation = true;
		
		SetPhase(new_state);
		
		ScheduleCall(this, "OperateSwitch", 3, 0, new_state, change);
		
		DebugLog("Playing switch animation");
		
		return true;
	}
	else
	{
		playing_animation = false;
		
		return SetState(new_state);
	}
}
