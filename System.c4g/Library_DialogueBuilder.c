/*
 * Contains standard options for dialogues
 */

#strict 2

//2147483647
global func StdDlgEventCancel(bool bState){ return Format("CreateDlgVar(\"Cancel\",pTarget,pSpeaker)=%v",bState);}
global func StdDlgArrayExitAlways(){ return [2147483646,-1,"Abbrechen","",0,[MCMX,0,0,0,0,-1],-1,0,"StopDialogue(pTarget)"];}
global func StdDlgArrayExitCancel(){ return [2147483646,-1,"Abbrechen","",0,[MCMX,0,0,0,0,-1],-1,"DlgVar(\"Cancel\",pTarget,pSpeaker)","StopDialogue(pTarget)"];}
global func StdDlgVar(string prefix, string variable, string operation){ return Format("%sDlgVar(\"%s\",pTarget,pSpeaker)%s",prefix,variable,operation); }
global func StdDlgVarSafeRemoveObject(string variable){ return Format("!DlgVar(\"%s\",pTarget,pSpeaker)||RemoveObject(DlgVar(\"%s\",pTarget,pSpeaker))", variable, variable);}

global func DlgObjVar(string prefix, string variable, string operation){ return Format("%sLocalN2(\"%s\",pTarget)%s",prefix,variable,operation); }

/*
 * Icons
 */
global func StdDlgIconContinue(){ return [LMM2,0,0,2,4]; }
global func StdDlgIconReturn(){ return LMM2; }

/*
 * Text Styles
 */

global func StdDlgTextStyleTrackQuest(string quest, bool bNoLogChoice)
{
	return [true,0,0,0,false,0,0,quest,bNoLogChoice];
}

/*
 * Condition helpers
 */

global func StdDlgQuestStage(string prefix, string quest, string operator)
{
	return Format("%sQuestStage(\"%s\",pTarget)%s",prefix, quest, operator);
}

/*
 * This helps build a dialogues array
 */

global func DlgOption(&dialogue, int index, parent, string option, string text)
{
	if (GetType(dialogue) != C4V_Array)
	{
		DebugLog("Invalid dialogue");
		return;
	}

	var entry = DlgOptionGet(dialogue, index, true);

	dialogue[entry][gDialogue_ARRAYPOS_Index] = 		index;
	dialogue[entry][gDialogue_ARRAYPOS_Parent] = 		parent;
	dialogue[entry][gDialogue_ARRAYPOS_MenuOption] = 	option;
	dialogue[entry][gDialogue_ARRAYPOS_Text] = 			text;
}

/*
 * Appends a dialogue option array to the dialogue.
 *
 * Use with arrays from StdDlgArray* functions.
 *
 * dialogue		the dialogue
 * option		the array from a StdDlgArray* function.
 */
global func DlgOptionAppend(&dialogue, option)
{
	PushBack(option, dialogue);
}

global func DlgOptionGet(&dialogue, int index, bool createIfNotFound)
{
	if (GetType(dialogue) != C4V_Array)
	{
		DebugLog("Invalid dialogue");
		return;
	}

	var entry = -1;

	for (var i = 0; i < GetLength(dialogue); i++)
	{
		if (dialogue[i][gDialogue_ARRAYPOS_Index] == index)
		{
			entry = i;
			break;
		}
	}

	if (entry == -1 && createIfNotFound)
	{
		DebugLog(Format("Entry with index %d not found, creating new entry", index));
		entry = GetLength(dialogue);
		PushBack([index],dialogue);
	}

	return entry;
}

global func DlgEvent(&dialogue, int index, event)
{
	if (GetType(dialogue) != C4V_Array)
	{
		DebugLog("Invalid dialogue");
		return;
	}

	var entry = DlgOptionGet(dialogue, index, true);

	dialogue[entry][gDialogue_ARRAYPOS_Events] = 		event;
}
