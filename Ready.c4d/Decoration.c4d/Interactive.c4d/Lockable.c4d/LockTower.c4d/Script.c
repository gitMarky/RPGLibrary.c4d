/*-- Lockable tower part with steel gate.
@author Sven2
@version 0.2.0
--*/

#strict 2

local gate;
local lock_left;
local lock_right;

protected func Initialize()
{
	SetAction("Be");
}


public func SetDir(direction)
{
	if (direction == 1)
	{
		SetSolidMask(15, 74, 15, 74);
	}
	else
	{
		SetSolidMask(15, 0, 15, 74);
	}
	
	if (gate)
	{
		gate->SetPosition(GetX() + 3 * (1 - direction), gate->GetY());
	}
	
	if (lock_left)
	{
		lock_left->SetPosition(GetX() - 15 + direction, lock_left->GetY());
	}
	
	if (lock_right)
	{
			lock_right->SetPosition(GetX() + 15 + direction, lock_right->GetY());
	
	}
	
	return _inherited(direction);
}

public func GetGate()
{
	return gate;
}

public func SetLocked(key_id, int bit_direction, id gate_id)
{
	if (!gate_id) gate_id = ID_Passage_SteelGate;

	if (!gate || gate_id != gate->GetID())
	{
		if (gate) gate->RemoveObject();
		
		gate = CreateObject(gate_id, 0, 0, NO_OWNER);
		
		gate->SetPosition(GetX() + 3 * (1 - GetDir()), GetY() + 10);
	}
	
	
	if (bit_direction & 1 && !lock_left)
	{
		lock_left = CreateObject(LOCK, 0, 0, NO_OWNER);
		lock_left->SetPosition(GetX() - 15 + GetDir(), GetY() + 25);
		lock_left->SetLocked(key_id);
		
		if (gate)
		{
			var switch_states = [
				SwitchDef(0)->TargetCall(gate, "Activate", lock_left),
				SwitchDef(1)->TargetCall(gate, "Activate", lock_left)
			];
			
			lock_left->SetSwitchStates(switch_states);
		}
	}
	
	if (bit_direction & 2 && !lock_right)
	{
		lock_right = CreateObject(LOCK, 0, 0, NO_OWNER);
		lock_right->SetPosition(GetX() + 15 + GetDir(), GetY() + 25);
		lock_right->SetLocked(key_id);
		
		if (gate)
		{
			var switch_states = [
				SwitchDef(0)->TargetCall(gate, "Activate", lock_right),
				SwitchDef(1)->TargetCall(gate, "Activate", lock_right)
			];
			
			lock_right->SetSwitchStates(switch_states);
		}
	}
}