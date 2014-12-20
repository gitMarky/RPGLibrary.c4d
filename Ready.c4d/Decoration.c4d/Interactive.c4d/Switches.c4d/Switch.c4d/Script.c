/*-- Schalter --*/

#strict 2
#include LF_S // Switch
#include LF_L // Lockable;

local has_three_states;

protected func Initialize()
{
	SetAction("Switch");
}

public func TwoStatesSwitch()
{
	has_three_states = false;
	
	SetPhase(0 + Random(2) * 2);
}

public func StartInactive()
{
	SetPhase(1);
	if (!has_three_states)
	{
		active_state = gSwitch_State_Inactive;
	}
	else
	{
		active_state = 1;
	}
}

protected func UpdatePhase()
{
	if (!has_three_states)
	{
		if (active_state == gSwitch_State_Inactive)
		{
			SetPhase(1);
		}
		else
		{
			SetPhase(active_state * 2);
		}
	}
	else
	{
		if (active_state == gSwitch_State_Inactive)
		{
			SetPhase(1);
		}
		else
		{
			SetPhase(active_state);
		}
	}
}

public func ThreeStatesSwitch()
{
	has_three_states = true;
	
	SetPhase(Random(3));
}

protected func ControlLeft(object controller)
{
	[$DescSwitch$]
	// already leftmost?
	if (GetActiveState() == 0) return true;	
	if (IsLocked()) return MessageLocked(controller);
	
	var success = false;
	
	// switch it
	if (!has_three_states)
	{
		success = SetState(0);
	}
	else
	{
		success = SetState(BoundBy(GetActiveState() - 1, 0, 2));
	}
	
	UpdatePhase();
	
	if (!success && RequiresSuccessfulCalls())
	{
		return MessageBlocked(controller);
	}
	
	Sound("Click");
	return true;
}

protected func ControlRight(object controller)
{
	[$DescSwitch$]
	// already rightmost?
	if (!has_three_states)
	{
		if (GetActiveState() == 1) return true;	
	}
	else
	{
		if (GetActiveState() == 2) return true;	
	}
	if (IsLocked()) return MessageLocked(controller);
	
	var success = false;
	
	// switch it
	if (!has_three_states)
	{
		success = SetState(1);
	}
	else
	{
		success = SetState(BoundBy(GetActiveState() - 1, 0, 2));
	}
	
	UpdatePhase();
	
	if (!success && RequiresSuccessfulCalls())
	{
		return MessageBlocked(controller);
	}
	
	Sound("Click");
	return true;
}
public func AttachBottom()
{
	var obj = FindObject(0, 0, 13, 0, 0);
	if (!obj)
		return;
	// Vertex ankleben
	SetVertex(0, 0, obj->GetX() - GetX() + GetVertex(0, 0, obj));
	SetVertex(0, 1, obj->GetY() - GetY() + GetVertex(0, 1, obj));
	if (GetAction() == "Vertical")
		SetAction("AttachVertical", obj);
	else
		SetAction("Attach", obj);
	return true;
}

protected func RejectCollect(id def, object obj)
{
	// never collect anything but try unlocking it
	UnlockWithKey(obj);
	return true;
}

private func OnUnlockWithKey(object key)
{
	Sound("Connect");
	SwitchMessage("$MessageUnlocked$", Contained(key));
	
	ChangeAction("Switch");
	UpdatePhase();
}
  
