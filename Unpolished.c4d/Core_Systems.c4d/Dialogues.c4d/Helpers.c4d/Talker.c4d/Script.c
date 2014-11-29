/*-- Talker --*/

#strict 2
#include LF_R // is "readable" ;)

local talkto;

public func IgnoreInventory()
{
	return true;
}

public func MenuQueryCancel()
{
	return true;
}

/**
 * This is the preferred method for linking an NPC with a dialogue.
 * Creates a helper object with an entrance, that attaches to an object. 
 * Entering the helper object starts the dialogue.
 * @par pTarget The helper object attaches to this object.
 * @par vDlg Can be any dialogue. TODO.
 */
global func AttachDialogue(object pTarget, vDlg)
{
	var talker = CreateObject(TALK, 0, 0, -1);
	talker->~Set(pTarget, vDlg);
}

public func Set(object target, vDlg)
{
	SetAction("Attach", target);
	SetName(GetName(target));
	talkto = target;
	SetDialogue(vDlg);
	SetVisibility(VIS_None);
}

public func GetMsgPortrait()
{
	return Format("Portrait:%i::%x::%s", GetID(talkto), GetColorDw(talkto), "1");
}

public func GetDlgMsgColor()
{
	return GetColorDw(talkto);
}

public func ActivateEntrance(object pBy)
{
	SetCommand(pBy, "None");
	if (!(GetOCF(pBy) & OCF_CrewMember))
		return;
	
	StartDialogue(pBy);
	
	if (GetX(pBy) < GetX())
		talkto->SetDir(0);
	else
		talkto->SetDir(1);
}

public func GetSpeaker()
{
	return talkto;
}

public func StartDialogue(object pTarget)
{
	_inherited(pTarget);
	
	AddBehaviour(talkto, "Stop", 1, 0, gBehaviour_Priority_Override);
}


public func StopDialogue(object pTarget)
{
	_inherited(pTarget);
	
	RemoveBehaviour(talkto, "Stop", gBehaviour_Priority_Override);
}

