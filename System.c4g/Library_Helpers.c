#strict 2

/**
 * Checks if the object is crew member of a non-AI player.
 * @return true, if the object is a crew member and the player type of the owner is {@i C4PT_User}.
 */
global func IsUserCrewMember(object target)
{
	return GetOCF(target) & OCF_CrewMember && GetPlayerType(GetOwner(target)) == C4PT_User;
}

static const gChangeAction_Delete = -1;

global func ChangeAction(string action, object target1, object target2, bool direct)
{
	if (!this) return false;
	return ObjectChangeAction(this, action, target1, target2, direct);
}

global func ObjectChangeAction(object target, string action, object target1, object target2, bool direct)
{
	if (!target) if (!(target = this)) return false;
	
	var dir = target->GetDir();
	var phase = target->GetPhase();
	var old_action = target->GetAction();
	var old_target1 = target->GetActionTarget(0);
	var old_target2 = target->GetActionTarget(1);
	
	// targets
	if (target1 == 0)
	{
		target1 = old_target1;
	}
	else if (target1 == gChangeAction_Delete)
	{
		target1 = 0;
	}
	
	if (target2 == 0)
	{
		target2 = old_target2;
	}
	else if (target2 == gChangeAction_Delete)
	{
		target2 = 0;
	}
	
	// change targets only
	if (!action)
	{
		target->SetActionTargets(target1, target2);
	}
	else
	{
		ObjectSetAction(target, action, target1, target2, direct);
		
		if (GetActMapVal("Length", action, GetID(target)) == GetActMapVal("Length", old_action, GetID(target)))
		{
			target->SetPhase(phase);
			target->SetDir(dir);
		}
	}
	
	return target->GetAction();
}