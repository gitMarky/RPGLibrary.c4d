#strict 2

local dialogue;

public func Initialize()
{
	SetPosition();
	dialogue = [];
}

global func DlgOpt(int index, string option)
{
	var builder = CreateObject(_DLB);

	builder->Init(index, option);

	return builder;
}

public func Create()
{
	RemoveObject(this);
	return dialogue;
}

public func Init(int index, string option)
{
	dialogue[gDialogue_ARRAYPOS_Index] = 		index;
	//dialogue[gDialogue_ARRAYPOS_Parent] = 		parent;
	dialogue[gDialogue_ARRAYPOS_MenuOption] = 	option;
	//dialogue[gDialogue_ARRAYPOS_Text] = 			text;
}

public func Parent(parent)
{
	dialogue[gDialogue_ARRAYPOS_Parent] = ExpandArray(dialogue[gDialogue_ARRAYPOS_Parent], parent);

	return this;
}

public func Text(string text)
{
	dialogue[gDialogue_ARRAYPOS_Text] = ExpandArray(dialogue[gDialogue_ARRAYPOS_Text], text, C4V_String);

	return this;
}

public func Events(event)
{
	var currentEvent = dialogue[gDialogue_ARRAYPOS_Events];

	dialogue[gDialogue_ARRAYPOS_Events] = ExpandArray(currentEvent, event, C4V_String);

	return this;
}

protected func ExpandArray(currentArray, content, type)
{
	var newArray;
	if (GetType(currentArray) == C4V_Array)
		newArray = currentArray;
	else
		newArray = [];

	if (GetType(content) == type)
		PushBack(content, newArray);
	else if (GetType(content) == C4V_Array)
	{
		for (var item in content)
			PushBack(item, newArray);
	}
	else
		ErrorLog(Format("The parameter is of invalid type: %v - only string and array allowed", content));

	return newArray;
}
