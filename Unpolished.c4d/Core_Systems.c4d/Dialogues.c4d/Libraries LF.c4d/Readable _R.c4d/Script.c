/*-- Basic script for all readable things, especially dialogues.
@title Readable
@author Marky
@version 0.1.0
@note This library should indeed be used for all readable
 objects such as wall scripts, books, signposts and so on.
 It provides a powerful framework for text coloring, menu decoration and portraits.
--*/

#strict 2

static const gDialogue_Object_Speaker = 0;
static const gDialogue_Object_Target = -1;

static const gDialogue_ARRAYPOS_Index = 		0;
static const gDialogue_ARRAYPOS_Parent = 		1;
static const gDialogue_ARRAYPOS_MenuOption = 	2;
static const gDialogue_ARRAYPOS_Text = 			3;
static const gDialogue_ARRAYPOS_Object = 		4;
static const gDialogue_ARRAYPOS_MenuStyle = 	5;
static const gDialogue_ARRAYPOS_TextStyle  = 	6;
static const gDialogue_ARRAYPOS_Conditions  = 	7;
static const gDialogue_ARRAYPOS_Events = 		8;


static const gMenuStyle_ARRAYPOS_Icon  = 		0; // ID des Menusymbols
static const gMenuStyle_ARRAYPOS_Color = 		1; // Farbe, in welcher der Text angegeben werden soll
static const gMenuStyle_ARRAYPOS_Conditions = 	2; // Anzahl der Bedingungen, die erfüllt werden müssen:
static const gMenuStyle_ARRAYPOS_Extra = 		3; // siehe AddMenuItem
static const gMenuStyle_ARRAYPOS_XPar1  = 		4; // siehe AddMenuItem
static const gMenuStyle_ARRAYPOS_Index = 		5; // springt zum angegebenen Dialogindex, anstatt die eigene Nachricht auszuwählen
static const gMenuStyle_ARRAYPOS_XPar2  = 		6; // siehe AddMenuItem

// zu Conditions:
//	    0 - wird immer ausgeblendet, wenn nicht alle Voraussetzungen erfüllt sind
//	    x - wird ausgeblendet, solange die ersten x Voraussetzungen nicht erfüllt sind.
//	        Wird rot eingeblendet, wenn die ersten x Voraussetzungen, aber nicht alle Voraussetzungen erfüllt sind.

static const gTextStyle_INVALID = 				-1; // does not use text style

static const gTextStyle_ARRAYPOS_Name = 		0; // soll der Name des Sprechers in dessen Farbe angezeigt werden? Standardmäßig ja
static const gTextStyle_ARRAYPOS_Color = 		1; // Farbe, in welcher der Rest des Texts angezeigt werden soll
static const gTextStyle_ARRAYPOS_Instant = 		2; // soll der Text sofort angezeigt werden?
static const gTextStyle_ARRAYPOS_Portrait = 	3; // gibt ein spezielles Portrait an
static const gTextStyle_ARRAYPOS_Message  = 	4; // gibt die Nachricht als CustomMessage aus
static const gTextStyle_ARRAYPOS_MenuDeco = 	5; // setzt eine neue MenuDecoration. Überschreibt auch die Definition aus GetDlgMsgDeco();
static const gTextStyle_ARRAYPOS_MenuCaption = 	6; // gibt dem Menu einen Titel
static const gTextStyle_ARRAYPOS_Quest = 		7; // string oder array von Quest-Namen (Engine-Name), zu denen die Nachricht hinzugefügt werden soll
static const gTextStyle_ARRAYPOS_LogOption = 	8; // gibt an, ob die Auswahl gelogt werden soll, oder nicht. Standard: wird gelogt.

local dialogue_definition, dialogue_start_index, last_speaker;


/* Format für lesbare Dinge: eine eigene kleine Scriptsprache

Der Dialog selbst ist ein Array. Darin enthalten sind weitere Arrays, die nach folgendem Format aufgebaut sind:

[ int iIndex, array/int iParentIndex, string szMenuOption, string szText, int/string iObjectNr, array aMenuStyle, array aTextStyle , string/array aConditions, array aEvents ]

Dokumentation:

int iIndex:
	ein Index für den Dialog. Kinder-Knoten können so angeben, wer ihre Eltern sind. Eine indizierung über die Reihenfolge im Dialog-Array wäre auch möglich gewesen, erschwert aber das dynamische zuweisen neuer Dialoge

array/int iParentIndex:
	Wird als Option an den Dialog iParentIndex angehängt. Wird ein Array angegeben, dann wird es an alle Knoten im Array angehängt

string szMenuOption:
	falls der Dialog als Option in einem Menu erscheint, dann wird dieser Text zur Auswahl angezeigt

string szText:
	das, was man später lesen kann - in den meisten Fällen reicht die MenuAuswahl, dann einfach nur "" oder 0 angeben

int/string iObjectNr:
	die Objektnummer des Sprechers.
	0 entspricht dem Objekt, in dem der Dialog ist
	-1 entspricht dem aufrufendem Objekt, d.h. in der Regel dem Spieler
	"String" entspricht dem Objekt GlobalN("String")

array aMenuStyle:
	[ id idIcon, int dwColor, int iStyle, int iExtra, XPar1, int iOverrideIndex, string szPortrait ]
	idIcon:
		ID des Menusymbols
	dwColor:
		Farbe, in welcher der Text angegeben werden soll
	iStyle:
	    Anzahl der Bedingungen, die erfüllt werden müssen:
	    0 - wird immer ausgeblendet, wenn nicht alle Voraussetzungen erfüllt sind
	    x - wird ausgeblendet, solange die ersten x Voraussetzungen nicht erfüllt sind.
	        Wird rot eingeblendet, wenn die ersten x Voraussetzungen, aber nicht alle Voraussetzungen erfüllt sind.
	iExtra:
		siehe AddMenuItem
	XPar1:
		siehe AddMenuItem
	iOverrideIndex:
		springt zum angegebenen Dialogindex, anstatt die eigene Nachricht auszuwählen



array aTextStyle:
	[ bool fName, int dwColor, bool fInstant, string szPortrait, bool fAsMessage, id idMenuDeco, string szMenuCaption, string/array szQuestToLog, bool bNoLogChoice ]
	fName:
		soll der Name des Sprechers in dessen Farbe angezeigt werden? Standardmäßig ja
	dwColor:
		Farbe, in welcher der Rest des Texts angezeigt werden soll
	fInstant:
		soll der Text sofort angezeigt werden?
	szPortrait:
		gibt ein spezielles Portrait an
	fAsMessage:
		gibt die Nachricht als CustomMessage aus
	idMenuDeco:
		setzt eine neue MenuDecoration. Überschreibt auch die Definition aus GetDlgMsgDeco();
	szMenuCaption:
		gibt dem Menu einen Titel
	szQuestToLog:
		string oder array von Quest-Namen (Engine-Name), zu denen die Nachricht hinzugefügt werden soll
	bNoLogChoice:
		gibt an, ob die Auswahl gelogt werden soll, oder nicht. Standard: wird gelogt.

	wird anstatt eines Arrays -1 angegeben, dann wird kein neues Menu geöffnet
*/

/**
 Returns an a reference to an object variable. This reference can be read and written.
 @par name The variable name.
 @par target The variable is saved for this object.
 @par speaker The variable is saved in this object.
 @return Reference to the local variable "DlgVar[Object number][Variable name]" in pSpeaker.
 @note This should be used in dialogues, obviously. It works outside of dialogues, too.
 @example For scripting purposes the variable has the same parameter name, but it gets saved for different
 objects individually.<br>
 {@code
 for (var i = 0; i < GetLength(customers); i++)
 {
    var customerName = GetName(customers[i]);
    var depositValue = DlgVar("Deposit", customers[i], bank);
 
    Log("%s has %d gold pieces in his account", customerName, depositValue);
 }
 }
 Logs the value of deposits of each customer of this bank.
 */
global func &DlgVar(string name, object target, object speaker)
{
	return LocalN2(Format("DlgVar%d%s", ObjectNumber(target), name), speaker);
}

// TODO: Remove, handle everything in DlgVar!
global func &CreateDlgVar(string name, object target, object speaker)
{
	var var_name = Format("DlgVar%d%s", ObjectNumber(target), name);
	CreateLocalN2(var_name, speaker);
	return LocalN2(var_name, speaker);
}

/**
 Sets the dialogue contents.
 @par definition The final dialogue array or other data.@brIf no array is specified, then
 the function tries getting an array by calling:
 {@code
 Format("MsgDialogue%s", aDlg)
 }
 This method has to exist in either the story object (see GetStory()), or as a global function.
 Furthermore it has to contain a valid dialogue array. Then, ValidateDialoge() is called.
 */
public func SetDialogue(definition)
{
	// direct input
	if (GetType(definition) == C4V_Array)
		dialogue_definition = definition;
		
	// optionally via scenario-script / System.c4g
	// should not be set in the editor, such that the
	// object receives the correct dialogue from System.c4g
	else if (definition)
	{
		var story = GetStory();
		if (story)
			dialogue_definition = ObjectCall(story, Format("MsgDialogue%s", definition));
		else
			dialogue_definition = GameCall(Format("MsgDialogue%s", definition));
	}

	ValidateDialogue();
}

public func SetStartDialogueEx(int index)
{
	dialogue_start_index = index;
}

/**
 Adds a dialogue to the end of the dialogue array.
 @par option The new dialogue option.
 @note Will proably be removed
 */
public func AddDialogue(option)
{
	if (GetType(dialogue_definition) == C4V_Array)
		PushBack(option, dialogue_definition);
}

/**
 Opens the dialogue in an object.
 @par target The object for which the dialogue should be displayed.
 @note The dialogue starts at dialogue index 0 per default. You can use SetStartDialogue()
 to change the starting index permanently.
 */
public func StartDialogue(object target)
{
	if (GetType(dialogue_definition) != C4V_Array) return;

	// in cutscenes the dialogue is displayed globally
	//var bGlobal = false;
	//if (IsCutscene()) bCutscene = true;
	// look up greeting
	ProcessDialogue(target, dialogue_start_index);
}

/**
 Opens a specific dialogue in an object.
 @par target The object for which the dialogue should be displayed.
 @par index The dialogue ID at which the dialogue starts.
 @note Contrary to StartDialogue() this is used for opening the dialogue at a specific index once.
 */
public func StartSpecificDialogue(object target, int index)
{
	if (GetType(dialogue_definition) != C4V_Array) return;
	ProcessDialogue(target, index);
}

/**
 Closes the dialogue menu in an object and performs all necessary cleanup operations.
 @par target The object in which the dialogue is displayed. This is the pTarget from StartDialogue().
 */
public func StopDialogue(object target)
{
	CloseMenu(target);
}

protected func ProcessDialogue(object target, int index, string szChoice)
{
	var option;
	for (option in dialogue_definition)
	{
		if (option[gDialogue_ARRAYPOS_Index] == index) break;
	}

	// get data
	
	var message_text = 		option[gDialogue_ARRAYPOS_Text];
	var object_nr = 	option[gDialogue_ARRAYPOS_Object]; 
	var text_style = 	option[gDialogue_ARRAYPOS_TextStyle]; 
	var events = 		option[gDialogue_ARRAYPOS_Events];

	// fire all events for that dialogue
	ProcessEvents(events, GetTargetString(), target, GetUserString(), GetSpeaker());

	if (GetType(text_style) == C4V_Int && text_style == gTextStyle_INVALID)
			return;

	// override color
	var color_override = this->~GetDialogueColor();


	// handle text style
	var quest_log_skip_option, quest_log_quest_name;
	
	if (GetType(text_style) == C4V_Array)
	{
		if (color_override) text_style[gTextStyle_ARRAYPOS_Color] = color_override;
		quest_log_skip_option = text_style[gTextStyle_ARRAYPOS_LogOption];
		quest_log_quest_name = text_style[gTextStyle_ARRAYPOS_Quest];
	}
	else
	{
		text_style = [];
		text_style[gTextStyle_ARRAYPOS_Name] = true;
		if (color_override) text_style[gTextStyle_ARRAYPOS_Color] = color_override;
		quest_log_skip_option = false;
	}

	// message instead of text box?
	var display_as_message = false;
	
	if (GetType(text_style) == C4V_Array) display_as_message = text_style[gTextStyle_ARRAYPOS_Message];

	// get the speaker
	var current_speaker = GetSpeaker();
	if (object_nr == gDialogue_Object_Target) current_speaker = target;
	if (GetType(object_nr) == C4V_Int) if (object_nr > 0) current_speaker = Object(object_nr);
	if (GetType(object_nr) == C4V_String)
	{
		if (GlobalN(object_nr)) current_speaker = GlobalN(object_nr);
		if (object_nr == "pTarget") current_speaker = target;
	}

	// look at each other
	SetDirTo(current_speaker, last_speaker);
	last_speaker = current_speaker;

	// random text choice
	if (GetType(message_text) == C4V_Array)
	{
		message_text = message_text[Random(GetLength(message_text))];
	}

	// insert names if possible
	if (message_text) message_text = Format(message_text, GetName(current_speaker));

	MsgBox(target, message_text, current_speaker , /* szPortrait*/ 0, display_as_message, true, text_style);

	// add dialogue options
	if (!display_as_message)
	for (option in dialogue_definition)
	{
		var add = false;
		if (GetType(option[gDialogue_ARRAYPOS_Parent]) == C4V_Array)
		{

			if (GetLength(option[gDialogue_ARRAYPOS_Parent]))
			if (GetArrayItemPosition(index, option[gDialogue_ARRAYPOS_Parent]) > -1
		    || (GetArrayItemPosition(-1, option[gDialogue_ARRAYPOS_Parent])> -1 && option[gDialogue_ARRAYPOS_MenuOption]))
			{
				DebugLog("%d in %v", index, option[gDialogue_ARRAYPOS_Parent]);
				add = true;
			}
		}
		else
		{
			if (option[gDialogue_ARRAYPOS_Parent] == index) add = true;
			if (option[gDialogue_ARRAYPOS_Parent] == -1 && option[gDialogue_ARRAYPOS_MenuOption]) add = true; // nur, wenn er eine Menu-Auswahl hat!

			DebugLog("check add %d: %d %d, %v", option[gDialogue_ARRAYPOS_Index], option[gDialogue_ARRAYPOS_Parent], index, add);

		}

		if (option[gDialogue_ARRAYPOS_MenuOption] == "" && option[gDialogue_ARRAYPOS_Text] == "") {add = false; DebugLog("Cancel add 0");}
		if (GetType(option[gDialogue_ARRAYPOS_MenuOption]) != C4V_String && GetType(option[gDialogue_ARRAYPOS_Text]) != C4V_Array) {add = false; DebugLog("Cancel add 1");}
		if (GetType(option[gDialogue_ARRAYPOS_Text]) != C4V_String && GetType(option[gDialogue_ARRAYPOS_Text]) != C4V_Array) {add = false; DebugLog("Cancel add 2");}

		if (add)
		{
			ProcessDialogueOption(target, option[gDialogue_ARRAYPOS_Index]);
		}
	}


	// quest log entry
	if (quest_log_quest_name)
	{
		var quest_log_global = false;
		if (target == GetFilm()) quest_log_global = true;

		if (!quest_log_skip_option)
			AddQuestLog(quest_log_quest_name, [Format("<c %x>%s:</c>", GetColorDlg(target), GetName(target)), szChoice], target, quest_log_global);

		AddQuestLog(quest_log_quest_name, [Format("<c %x>%s:</c>", GetColorDlg(current_speaker), GetName(current_speaker)), message_text], target, quest_log_global);
	}
}

protected func ProcessDialogueOption(object target, index)
{
	var option;
	for(option in dialogue_definition)
	{
		if (option[gDialogue_ARRAYPOS_Index] == index) break;
	}

	// get data
	
	var menu_item_text = 	option[gDialogue_ARRAYPOS_MenuOption];
	var message_text = 		option[gDialogue_ARRAYPOS_Text];
	var menu_style = 	option[gDialogue_ARRAYPOS_MenuStyle];
	var conditions = 	option[gDialogue_ARRAYPOS_Conditions];

	var add = false;

	var conditionInfo = CheckConditionsDetailed(conditions, GetTargetString(), target, GetUserString(), GetSpeaker());

	add = conditionInfo[0];
	var conditions_fulfilled = conditionInfo[1];

	if (GetType(message_text) == C4V_Array)
	{
		message_text = message_text[Random(GetLength(message_text))];
	}

	if (menu_item_text == 0 || menu_item_text == "")
	{
		if (message_text)
		{
			// does not take text_style into account yet
			MsgBoxAddText(target, message_text);
		}
	}
	else
	{
		var menu_icon = NONE;
		var extra, xpar1, xpar2;
		var conditions_required = 0;
		var conditions_active = false;
		var message_color, index_override = index;


		if (GetType(menu_style) == C4V_Array)
		{
			menu_icon = menu_style[gMenuStyle_ARRAYPOS_Icon];
			message_color = menu_style[gMenuStyle_ARRAYPOS_Color];
			conditions_required = menu_style[gMenuStyle_ARRAYPOS_Conditions];
			
			extra = menu_style[gMenuStyle_ARRAYPOS_Extra];
			xpar1 = menu_style[gMenuStyle_ARRAYPOS_XPar1];
			xpar2 = menu_style[gMenuStyle_ARRAYPOS_XPar2];
			
			if (menu_style[gMenuStyle_ARRAYPOS_Index] != 0) index_override = menu_style[gMenuStyle_ARRAYPOS_Index]-1;
			
			if (conditions_required > 0)
			{
				conditions_required--;
				conditions_active = true;
			}
		}
		if (GetType(menu_style) == C4V_C4ID)
			menu_icon = menu_style;

		// Override the color
		var override_color = this->~GetDialogueOptionColor();
		if (override_color) message_color = override_color;
		if (!message_color) message_color = RGB(245, 245, 245); // make it darker, so that you can distinguish it from normal text
		if (add == false) message_color = RGB(255, 0, 0);
		
		if (message_color) menu_item_text = ColorizeString(menu_item_text, message_color);

		var command = Format("ProcessDialogue(Object(%d), %d, \"%s\")", ObjectNumber(target), index_override, menu_item_text);

		if (add == false && conditions_active && conditions_fulfilled < GetLength(conditions)) command = "eval(\"true\")"; // dummy command

		if (add || (conditions_active && conditions_fulfilled >= conditions_required))
			MsgBoxAddOption(target, menu_icon, menu_item_text, command, 0, extra, xpar1, xpar2);
	}

}

/**
 The object that the target speaks to in the dialogue. This is usually an NPC or the dialogue object itself.
 @return object An object, which is classified as the speaker. Default value: this.
 @note You do not need to change this.
 */
public func GetSpeaker(){ return this; }

/**
 The script system replaces this string with a reference to the day-night-cycle object.
 @return String that is used in the script language: "pSpeaker".
 */
public func GetUserString(){ return "pSpeaker"; }

/**
 The script system replaces this string with a reference to the target object.
 @return String that is used in the script language: "pTarget".
 */
public func GetTargetString(){ return "pTarget"; }

/**
 Prints a dialogue tree in the log. This may be useful for debugging and finding places where a dialogue option should be displayed, but isn't.
 @note The log output is a debug log, enable debug mode first.
 TODO: reference debug mode
 */
public func printTree()
{
	local tree;
	local mapping;
	local visited;
	tree = [];
	mapping = [];
	visited = [];
	
	for(var i = 0; i < GetLength(dialogue_definition); i++)
	{
		mapping[i] = dialogue_definition[i][gDialogue_ARRAYPOS_Index];
	}
	for(var i = 0; i<GetLength(dialogue_definition); i++)
	{
		var parentid = dialogue_definition[i][gDialogue_ARRAYPOS_Index];
		var node = [i, []]; // node, children

		for(var j = 0; j<GetLength(dialogue_definition); j++)
		{
			if (j == i) continue;

			var parentIndex = dialogue_definition[j][gDialogue_ARRAYPOS_Parent];

			if (GetType(parentIndex) == C4V_Array)
			{
				for(var parent in parentIndex)
				{
					if (parent == parentid || parent == -1)
					{
						PushBack(GetArrayItemPosition(dialogue_definition[j][gDialogue_ARRAYPOS_Index], mapping), node[1]);
					}
				}
			}
			else if (parentIndex == parentid || parentIndex == -1)
			{
				if (parentid == 1)
					DebugLog("XXX Adding quest: %d", j);
				PushBack(GetArrayItemPosition(dialogue_definition[j][gDialogue_ARRAYPOS_Index], mapping), node[1]); // add j as child of node;
			}
		}

		PushBack(node, tree);
	}

	printChildren(tree[0][0], 0, [-1]);

}

public func printChildren(int parentid, int depth, array children)
{
	var node;
	for(var index = 0; index < GetLength(tree); index++)
	{
		node = tree[index];
		if (node[0] == parentid) break;
	}

	visited[parentid]++;

	var message = "";
	for(var i = 0; i<depth; i++)
	{
		var space = "   ";
		if (i == depth-1)
		{
			space = " * ";
		}
		message = Format("%s%s", message, space);
	}

	var option, answer;
	answer = dialogue_definition[node[0]][gDialogue_ARRAYPOS_Text];
	option = dialogue_definition[node[0]][gDialogue_ARRAYPOS_MenuOption];

	if (GetType(answer) != C4V_String) answer = "(null)";
	if (GetType(option) != C4V_String) option = "(null)";

	message = Format("%s[%s]:%s", message, option, answer);

	DebugLog(message);

	if (visited[parentid] > 1) return;

	for(var j = 0; j<GetLength(node[1]); j++)
	{
		var child = node[1][j];

		printChildren(child, depth+1, node[1]);
	}
}

public func printEntries()
{
	for(var field in dialogue_definition)
	{
		DebugLog("%d, parents %v: %s  - %v", field[0], field[1], field[2], field[3]);
	}
}

/**
 Verifies, that the dialogue array is valid and creates entries built by TODO.
 @note You do not have to call this yourself, this function gets called by SetDialogue()
 */
public func ValidateDialogue()
{
	if (GetType(dialogue_definition) != C4V_Array) return 0;

	var reasons = [];

	for(var i = 0; i < GetLength(dialogue_definition); i++)
	{
		var option = dialogue_definition[i];

		if (!option)
		{
			var reason = Format("aDialogue[%d] is empty", i);
			ErrorLogAlways(reason);
			PushBack(reason, reasons);
		}
		else if (GetType(option) == C4V_C4Object && GetID(option) == ID_Helper_DialogueBuilder)
		{
			var actualDialogue = option->Create();
			DebugLog(Format("Converted aDialogue[%d] from %i-object to actual dialogue", i, ID_Helper_DialogueBuilder));
			dialogue_definition[i] = actualDialogue;
		}
		else if (GetType(option) != C4V_Array)
		{
			var reason = Format("No valid dialogue: %v", option);
			ErrorLogAlways(reason);
			PushBack(reason, reasons);
		}
	}

	if (GetLength(reasons) > 0)
	{
		dialogue_definition = [[0, -1, 0, "Dialogue is not functional, see error log for details."]];
	}
}
