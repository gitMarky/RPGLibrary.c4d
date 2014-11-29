/*--
 Helper object that can be used to build dialogue options. The advantage in using this object is,
 that you do not have to remember the order of entries in the dialogue option array. It saves a lot
 of time.
 @title Dialogue builder
 @author Marky
 @version 0.1.0
--*/

#strict 2

local dialogue;

public func Initialize()
{
	SetPosition();
	dialogue = [];
}

/**
 * Initializes a default dialogue option.
 * @par index The dialogue index. This has to be a unique integer for the specific dialogue.
 * @par parent The option will be displayed as a child of this dialogue index. Passing a single
 *        int makes this option available only under one option. Passing an array of int makes
 *        the option available under all the specified options. Parent -1 means, that the option will
 *        be displayed under every option in the dialogue, except itself.
 * @par option The dialogue option is usually a menu item in a menu. This is the caption of the menu item.
 * @return object Returns the helper object, such that the dialogue option can be further modified.
 */
global func DlgOption(int index, parent, string option)
{
	var builder = CreateObject(ID_Helper_DialogueBuilder);

	builder->Init(index, option);

	builder->Parent(parent);

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
	dialogue[gDialogue_ARRAYPOS_MenuOption] = 	option;
}

public func Parent(parent)
{
	dialogue[gDialogue_ARRAYPOS_Parent] = ExpandDialogueArray(dialogue[gDialogue_ARRAYPOS_Parent], parent, C4V_Int);

	return this;
}

/**
 * Specifies the text that will be displayed after selecting an option.
 * @par text The message text. Accepts a string or an array. If you input an array, then one of 
 * the options will be displayed randomly. Calling the method several times in a row works has the
 * same effect as passing an array.
 * @return object Returns the helper object, such that the dialogue option can be further modified.
 */
public func Text(text)
{
	dialogue[gDialogue_ARRAYPOS_Text] = ExpandDialogueArray(dialogue[gDialogue_ARRAYPOS_Text], text, C4V_String);

	return this;
}

/**
 * Specifies the conditions that have to be fulfilled for the option to be displayed in the dialogue.
 * @par event The events. See TODO.
 * @return object Returns the helper object, such that the dialogue option can be further modified.
 */
public func Conditions(event)
{
	var currentConditions = dialogue[gDialogue_ARRAYPOS_Conditions];

	dialogue[gDialogue_ARRAYPOS_Conditions] = ExpandDialogueArray(currentConditions, event, C4V_String);

	return this;
}

/**
 * Specifies the events that fire after selecting an option.
 * @par event The events. See TODO.
 * @return object Returns the helper object, such that the dialogue option can be further modified.
 */
public func Events(event)
{
	var currentEvent = dialogue[gDialogue_ARRAYPOS_Events];

	dialogue[gDialogue_ARRAYPOS_Events] = ExpandDialogueArray(currentEvent, event, C4V_String);

	return this;
}

protected func ExpandDialogueArray(currentArray, content, type)
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
		ErrorLog(Format("The parameter is of invalid type: %v - only %v and array allowed", content, type));

	return newArray;
}
