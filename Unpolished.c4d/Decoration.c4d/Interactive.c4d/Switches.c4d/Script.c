/*-- Switch
@version 0.2.0
@author Marky
--*/

/*
A switch consists of several states. Each state can have several targets, who in turn have function calls.

Format:
switch [[state]
        [state, []]
        [state, [ [target, call], [target, call] ]]]

*/

#strict 2

local switch_states;
local active_state;
local requires_successful_calls;

static const gSwitch_Def_State = 		0;
static const gSwitch_Def_Targets = 		1;
static const gSwitch_Def_ColorBase =	2;
static const gSwitch_Def_ColorUser =	3;

static const gSwitch_State_Inactive = -1;

public func IsSwitch()
{
	return true;
}

protected func Initialize()
{
	if (GetID() == ID_Library_Switch)
	{
		SetPosition();
		switch_states = [];
	}
	
	active_state = gSwitch_State_Inactive;
	
	SetObjectBlitMode(GFX_BLIT_ClrSfc_OwnClr);
	_inherited(...);
}

/**
@param cycle A day-night-cycle definition array.
@param target [optional] The target to attach the day-night-cycle to. No value or 0
referrs to the local object.
@author Marky
@version 0.1.0
*/
public func SetSwitchStates(states)
{
	// direct input?
	if (GetType(states) == C4V_Array)
	{
		switch_states = states;
	}
	else if (states)
	{
		var story = GetStory();
		if (story)
			switch_states = ObjectCall(states, Format("SwitchStates%s", states));
		else
			switch_states = GameCall(Format("SwitchStates%s", states));
	}

	ValidateSwitchStates();
}

/**
 Verifies, that the dialogue array is valid and creates entries built by TODO.
 @note You do not have to call this yourself, this function gets called by SetDialogue()
 */
public func ValidateSwitchStates()
{
	if (GetType(switch_states) != C4V_Array) return 0;

	var reasons = [];

	for(var i = 0; i < GetLength(switch_states); i++)
	{
		if (!switch_states[i])
		{
			var reason = Format("switch_states[%d] is empty", i);
			ErrorLogAlways(reason);
			PushBack(reason, reasons);
			continue;
		}
		else if (GetType(switch_states[i]) == C4V_C4Object && GetID(switch_states[i]) == ID_Helper_SwitchBuilder)
		{
			var actual_state = switch_states[i]->Create(this);
			DebugLog(Format("Converted switch_state[%d] from %i-object to actual state", i, ID_Helper_SwitchBuilder));
			switch_states[i] = actual_state;
		}
		else if (GetType(switch_states[i]) != C4V_Array)
		{
			var reason = Format("No valid switch state: %v", switch_states[i]);
			ErrorLogAlways(reason);
			PushBack(reason, reasons);
		}
	}

	if (GetLength(reasons) > 0)
	{
		switch_states = [];
	}
}

/////////////////////////////////////////////////////////////////////////
//
// build switch states

global func SwitchDef(int state)
{
	var obj = CreateObject(ID_Helper_SwitchBuilder, 0, 0, NO_OWNER);
	
	var def = [];
	def[gSwitch_Def_State] = state;
	def[gSwitch_Def_Targets] = [];
	
	obj->LocalN("switch_states") = def;
	
	return obj;
}

public func TargetCall(object target, string call, p2, p3, p4, p5, p6, p7, p8, p9)
{
	var option = [target, call, p2, p3, p4, p5, p6, p7, p8, p9];
	
	PushBack(option, switch_states[gSwitch_Def_Targets]);
	
	return this;
}

public func ColorBase(int value)
{
	switch_states[gSwitch_Def_ColorBase] = value;
	
	return this; 
}

public func ColorUser(int value)
{
	switch_states[gSwitch_Def_ColorUser] = value;
	
	return this;
}

public func Create(object switch)
{
	// TODO: Code is wrong
	// replace references to 'this' with the switch object
	for (var i = 0; i < GetLength(switch_states); i++)
	{
		if (!switch_states[i]) continue;
		
		for (var j = 0; j < GetLength(switch_states[i][gSwitch_Def_Targets]); j++)
		{
			var call = switch_states[i][gSwitch_Def_Targets][j];
			
			for (var k = 0; k < GetLength(call); k++)
			{
				if (call[k] == this) call[k] = switch;
			}
			
			switch_states[i][gSwitch_Def_Targets][j] = call;
		}
	}

	return switch_states;
}

/////////////////////////////////////////////////////////////////////////
//
// handle switching

public func SetState(int state)
{
	var i;
	var old_active_state = active_state;
	active_state = gSwitch_State_Inactive;
	for (i = 0; i < GetLength(switch_states); i++)
	{
		if (switch_states[i][gSwitch_Def_State] == state)
		{
			active_state = state;
			break;
		}
	}
	
	if (active_state == old_active_state) return false;
	
	var result = true;
	
	if (i < GetLength(switch_states)) // you can actually set to inactive and have a call!
	{
		for (var call in switch_states[i])
		{
			result &= DoTargetCall(call, active_state);
		}
	}
	
	if (RequiresSuccessfulCalls())
	{
		if (result)
		{
			UpdateColors(state);
		}
		return result;
	}
	else
	{
		UpdateColors(state);
		return true;
	}
}

private func DoTargetCall(call, int state)
{
	var call_target = call[0];
	
	if (GetType(call_target) == C4V_String)
	{
		call_target = GlobalN(call_target);
		
		if (!call_target)
		{
			DebugLog("Cannot execute target call %v: target is 0", call);
			return false;
		}
	}

	if (GetType(call[1]) == C4V_String)
	{
		if (!call_target)
		{
			return GameCall(call[1], call[2], call[3], call[4], call[5], call[6], call[7], call[8], call[9]);
		}
		else
		{
			return ObjectCall(call_target, call[1], call[2], call[3], call[4], call[5], call[6], call[7], call[8], call[9]);
		}
	}
	else
	{
		if (!call_target)
		{
			return GameCall("OnSwitchState", state, this);
		}
		else
		{
			return ObjectCall(call_target, "OnSwitchState", state, this);
		}
	}
}

public func GetActiveState()
{
	return active_state;
}

protected func RequiresSuccessfulCalls()
{
	return requires_successful_calls;
}

protected func SetSuccessfulCalls(bool require)
{
	return requires_successful_calls = require;
}

protected func UpdateColors(int state)
{
	if (GetType(switch_states[state]) != C4V_Array) return;
	
	var color_base = switch_states[state][gSwitch_Def_ColorBase];
	var color_user = switch_states[state][gSwitch_Def_ColorUser];
	
	if (color_base)
	{
		SetClrModulation(color_base);
	}
	
	if (color_user)
	{
		SetColorDw(color_user);
	}
}