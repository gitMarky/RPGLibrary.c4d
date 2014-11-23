/*
 * Contains standard options for dialogues
 */

#strict 2

//2147483647

global func DlgOptionCancelAlways(){ return [2147483646,-1,"Abbrechen","",0,[MCMX,0,0,0,0,-1],-1,0,"StopDialogue(pTarget)"];}
global func DlgOptionCancelToggle(){ return [2147483646,-1,"Abbrechen","",0,[MCMX,0,0,0,0,-1],-1,"DlgVar(\"Cancel\",pTarget,pSpeaker)","StopDialogue(pTarget)"];}

/**
 * Sets the dialogue cancel variable to the specified state.
 * @param bState Set to false if you want to disable cancelling the dialogue.
 * @return string A string that can be used in with ProcessEvents()
 */ 
global func DlgEventSetCancel(bool bState){ return Format("CreateDlgVar(\"Cancel\",pTarget,pSpeaker)=%v",bState);}
global func DlgEventSafeRemoveVar(string variable){ return Format("!DlgVar(\"%s\",pTarget,pSpeaker)||RemoveObject(DlgVar(\"%s\",pTarget,pSpeaker))", variable, variable);}
global func DlgEventSafeRemoveObject(object obj){ var variable = ObjectNumber(obj); return Format("!Object(%d)||RemoveObject(Object(%d))", variable, variable);}

global func DlgStdVar(string prefix, string variable, string operation){ return Format("%sDlgVar(\"%s\",pTarget,pSpeaker)%s",prefix,variable,operation); }
global func DlgObjVar(string prefix, string variable, string operation){ return Format("%sLocalN2(\"%s\",pTarget)%s",prefix,variable,operation); }

/*
 * Icons
 */
global func DlgIconContinue(){ return [LMM2,0,0,2,4]; }
global func DlgIconReturn(){ return LMM2; }

/*
 * Text Styles
 */
global func DlgTextStyleTrackQuest(string quest, bool bNoLogChoice)
{
	return [true,0,0,0,false,0,0,quest,bNoLogChoice];
}

/*
 * Condition helpers
 */

global func DlgQuestStage(string prefix, string quest, string operator)
{
	return Format("%sQuestStage(\"%s\",pTarget)%s",prefix, quest, operator);
}
