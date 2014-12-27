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
 Initializes a default dialogue option.
 @par index The dialogue index. This has to be a unique integer for the specific dialogue.
 @par parent The option will be displayed as a child of this dialogue index. Passing a single
        int makes this option available only under one option. Passing an array of int makes
        the option available under all the specified options. Parent -1 means, that the option will
        be displayed under every option in the dialogue, except itself.
 @par option The dialogue option is usually a menu item in a menu. This is the caption of the menu item.
 @return object Returns the helper object, such that the dialogue option can be further modified.
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
	
	SafeInitArray(gDialogue_ARRAYPOS_TextStyle);
	
	dialogue[gDialogue_ARRAYPOS_TextStyle][gTextStyle_ARRAYPOS_Name] = true;
}

public func Parent(parent)
{
	dialogue[gDialogue_ARRAYPOS_Parent] = PushBack(parent, dialogue[gDialogue_ARRAYPOS_Parent], C4V_Int);

	return this;
}

/**
 Specifies the text that will be displayed after selecting an option.
 @par text The message text. Accepts a string or an array. If you input an array, then one of 
 the options will be displayed randomly. Calling the method several times in a row works has the
 same effect as passing an array.
 @return object Returns the helper object, such that the dialogue option can be further modified.
 */
public func Text(text)
{
	dialogue[gDialogue_ARRAYPOS_Text] = PushBack(text, dialogue[gDialogue_ARRAYPOS_Text], C4V_String);

	return this;
}

/**
 Specifies the conditions that have to be fulfilled for the option to be displayed in the dialogue.
 @par condition The conditions. See TODO.
 @return object Returns the helper object, such that the dialogue option can be further modified.
 */
public func Conditions(condition)
{
	var currentConditions = dialogue[gDialogue_ARRAYPOS_Conditions];

	dialogue[gDialogue_ARRAYPOS_Conditions] = PushBack(condition, currentConditions, C4V_String);

	return this;
}

/**
 Specifies the events that fire after selecting an option.
 @par event The events. See TODO.
 @return object Returns the helper object, such that the dialogue option can be further modified.
 */
public func Events(event)
{
	var currentEvent = dialogue[gDialogue_ARRAYPOS_Events];

	dialogue[gDialogue_ARRAYPOS_Events] = PushBack(event, currentEvent, C4V_String);

	return this;
}

/**
 Sets the icon for the options in the dialogue.
 @par icon The icon ID.
 @return object Returns the helper object, such that the dialogue option can be further modified.
 @version 0.2.0
 */
public func Icon(id icon)
{
	SafeInitArray(gDialogue_ARRAYPOS_MenuStyle);
	
	dialogue[gDialogue_ARRAYPOS_MenuStyle][gMenuStyle_ARRAYPOS_Icon] = icon;
	
	return this;
}

/**
 Sets the {@code iExtra} value of the dialogue option, as in AddMenuItem().@br
{@table
{@tr {@th Bit} {@th Description}}
{@tr {@td 0} {@td Normal}}
{@tr {@td 1} {@td Rank symbol. You cannot specify {@c iCount}!}}
{@tr {@td 2} {@td Picture facet, shifted to the right MenuXPar1() times.}}
{@tr {@td 3} {@td Draws the picture graphic of object MenuXPar1() with a rank symbol.}}
{@tr {@td 4} {@td Draws the picture graphic of object MenuXPar1().}}
}
 @par bit The extra value.
 @par xpar1 The extra parameter.
 @par xpar2 The second extra parameter.
 @return object Returns the helper object, such that the dialogue option can be further modified.
 @version 0.2.0
 */
public func MenuExtra(int bit, int xpar1, int xpar2)
{
	SafeInitArray(gDialogue_ARRAYPOS_MenuStyle);
	
	dialogue[gDialogue_ARRAYPOS_MenuStyle][gMenuStyle_ARRAYPOS_Extra] = bit;
	dialogue[gDialogue_ARRAYPOS_MenuStyle][gMenuStyle_ARRAYPOS_XPar1] = xpar1;
	dialogue[gDialogue_ARRAYPOS_MenuStyle][gMenuStyle_ARRAYPOS_XPar2] = xpar2;
	
	return this;
}

/**
 Instead of displaying the Text() the dialogue jumps to DlgOption() with the specified index.
 @par index The dialogue option index.
 @return object Returns the helper object, such that the dialogue option can be further modified.
 @version 0.2.0
 */
public func NextDialogue(int index)
{
	SafeInitArray(gDialogue_ARRAYPOS_MenuStyle);
	
	dialogue[gDialogue_ARRAYPOS_MenuStyle][gMenuStyle_ARRAYPOS_Index] = index + 1;
	
	return this;
}

/**
 This displays the option, even if it is not available. Useful for shops.
 @par amount If the first {@c amount} Conditions() are fulfilled, then the option is displayed, but
 it is colored red and cannot be selected. Once all conditions are fulfilled the option is selectable.
 By default {@c amount = 0}, and in this case the option is never displayed red: It is not displayed at
 all until all conditions are fulfilled.
 @return object Returns the helper object, such that the dialogue option can be further modified.
 @version 0.2.0
 */
public func RequiredConditions(int amount)
{
	SafeInitArray(gDialogue_ARRAYPOS_MenuStyle);
	
	dialogue[gDialogue_ARRAYPOS_MenuStyle][gMenuStyle_ARRAYPOS_Conditions] = amount + 1;
	
	return this;
}

/**
 Sets the color for the message string that is displayed in the dialogue.
 @par dword The color modulation. Is modified by the dialogue with ColorizeString() to make the text readable.
 @return object Returns the helper object, such that the dialogue option can be further modified.
 @version 0.2.0
 */
public func MenuItemColor(int dword)
{
	SafeInitArray(gDialogue_ARRAYPOS_MenuStyle);
	
	dialogue[gDialogue_ARRAYPOS_MenuStyle][gMenuStyle_ARRAYPOS_Color] = dword;
	
	return this;
}

/**
 Logs the chosen dialogue option and response to the quest log of the specified quest. 
 Multiple quests can be specified by calling the function repeatedly.
 Logging the option can be disabled with LogSkipOption(). This is useful for options such as "Next".
 @par quest The quest name.
 @return object Returns the helper object, such that the dialogue option can be further modified.
 @version 0.2.0
 */
public func LogQuest(quest)
{
	SafeInitArray(gDialogue_ARRAYPOS_TextStyle);

	var currentQuests = dialogue[gDialogue_ARRAYPOS_TextStyle][gTextStyle_ARRAYPOS_Quest];
	dialogue[gDialogue_ARRAYPOS_TextStyle][gTextStyle_ARRAYPOS_Quest] = PushBack(quest, currentQuests, C4V_String);
	
	return this;
}

/**
 Does not log the selected dialogue option to the quest log, but still logs the Text().
 */
public func LogSkipOption()
{
	SafeInitArray(gDialogue_ARRAYPOS_TextStyle);

	dialogue[gDialogue_ARRAYPOS_TextStyle][gTextStyle_ARRAYPOS_LogOption] = true;
	
	return this;
	
}

/**
 Sets the caption text for the dialogue menu.
 @par caption The text that is displayed in the top left bar of the menu.
 @return object Returns the helper object, such that the dialogue option can be further modified.
 @version 0.2.0
 */
public func MenuCaption(string caption)
{
	SafeInitArray(gDialogue_ARRAYPOS_TextStyle);
	
	dialogue[gDialogue_ARRAYPOS_TextStyle][gTextStyle_ARRAYPOS_MenuCaption] = caption;
	
	return this;
}

/**
 Sets a menu decoration for the dialogue.
 @return object Returns the helper object, such that the dialogue option can be further modified.
 @version 0.2.0
 */
public func MenuDeco(id deco)
{
	SafeInitArray(gDialogue_ARRAYPOS_TextStyle);
	
	dialogue[gDialogue_ARRAYPOS_TextStyle][gTextStyle_ARRAYPOS_MenuDeco] = deco;
	
	return this;
}

/**
 Displays the dialogue as a message instead of a menu.
 @return object Returns the helper object, such that the dialogue option can be further modified.
 @version 0.2.0
 */
public func DisplayAsMessage()
{
	SafeInitArray(gDialogue_ARRAYPOS_TextStyle);
	
	dialogue[gDialogue_ARRAYPOS_TextStyle][gTextStyle_ARRAYPOS_Message] = true;
	
	return this;
}

/**
 Specifies a different portrait for the message. Useful for displaying item details or emotions.
 @param definition The default Clonk Rage portrait format (whatever that is).
 @return object Returns the helper object, such that the dialogue option can be further modified.
 @version 0.2.0
 */
public func Portrait(definition)
{
	SafeInitArray(gDialogue_ARRAYPOS_TextStyle);
	
	dialogue[gDialogue_ARRAYPOS_TextStyle][gTextStyle_ARRAYPOS_Portrait] = definition;
	
	return this;
}

/**
 Usually the text is displayed letter after letter. Calling this function displays the text instantly.
 @return object Returns the helper object, such that the dialogue option can be further modified.
 @version 0.2.0
 */
public func DisplayInstantly()
{
	SafeInitArray(gDialogue_ARRAYPOS_TextStyle);
	
	dialogue[gDialogue_ARRAYPOS_TextStyle][gTextStyle_ARRAYPOS_Instant] = true;
	
	return this;
}

/**
 Tells the dialogue which object is speaking. By default this is the object
 that the dialogue is set to, so use this if you want other objects to speak
 in the dialogue.
 @par speaker The object that is displayed as the speaker. You have several options here:@br
 - {@c gDialogue_Object_Target} displays the player character as the speaker,@br
 - {@c int nr} displays {@c Object(nr)} as the speaker,@br
 - {@c string variable_name} displays the global varialbe {@c GlobalN(variable_name)} as the speaker
 */
public func Speaker(speaker)
{
	dialogue[gDialogue_ARRAYPOS_Object] = speaker;
	
	return this;
}

/**
 Usually the character that is talking is displayed with his own color. Calling this function disables that.
 @return object Returns the helper object, such that the dialogue option can be further modified.
 @version 0.2.0
 */
public func DisableSpeakerColor()
{
	SafeInitArray(gDialogue_ARRAYPOS_TextStyle);
	
	dialogue[gDialogue_ARRAYPOS_TextStyle][gTextStyle_ARRAYPOS_Name] = false;
	
	return this;
}

/**
 Sets the color for the message string that is displayed as options.
 @par dword The color modulation. Is modified by the dialogue with ColorizeString() to make the text readable.
 @return object Returns the helper object, such that the dialogue option can be further modified.
 @version 0.2.0
 */
public func TextColor(int dword)
{
	SafeInitArray(gDialogue_ARRAYPOS_TextStyle);
	
	dialogue[gDialogue_ARRAYPOS_TextStyle][gTextStyle_ARRAYPOS_Color] = dword;
	
	return this;
}

protected func SafeInitArray(int index)
{
	if (GetType(dialogue[index]) != C4V_Array)
	{
		dialogue[index] = [];
	}
}