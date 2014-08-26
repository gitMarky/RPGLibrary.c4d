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
