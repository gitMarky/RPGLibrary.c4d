/*--
An object that attaches to objects. It starts a dialogue if you press [Up]
in front of the object.
@title Talker
@version 0.1.0
@author --*/

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
 * @par target The helper object attaches to this object.
 * @par dialogue Can be any dialogue. See "Demo Scenario: Adding a dialogue".
 */
global func AttachDialogue(object target, dialogue)
{
	var talker = CreateObject(TALK, 0, 0, -1);
	talker->~Set(target, dialogue);
}

public func Set(object target, dialogue)
{
	SetAction("Attach", target);
	SetName(GetName(target));
	talkto = target;
	SetDialogue(dialogue);
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

public func ActivateEntrance(object clonk)
{
	SetCommand(clonk, "None");
	if (!(GetOCF(clonk) & OCF_CrewMember))
		return;
		
	if (GetX(clonk) < GetX(talkto))
	{
		talkto->SetDir(0);
		clonk->SetDir(1);
	}
	else
	{
		talkto->SetDir(1);
		clonk->SetDir(0);
	}
	
	StartDialogue(clonk);
}

public func GetSpeaker()
{
	return talkto;
}

/**
 Starts a dialogue with an object without having to press [Up] in front of it.
 @par target A player crew member.
 @version 0.1.0
 */
public func StartDialogue(object target)
{
	_inherited(target);
	
	AddBehaviour(talkto, "Stop", 1, 0, gBehaviour_Priority_Override);
}

/**
 Stops a running dialogue.
 @par target The crew member that started the dialogue.
 @version 0.1.0
 */
public func StopDialogue(object target)
{
	_inherited(target);
	
	RemoveBehaviour(talkto, "Stop", gBehaviour_Priority_Override);
}

