/* Contains standard options for dialogues */

#strict 2

//2147483647

/**
 * A dialogue option that lets you cancel the dialogue at any time
 * @return A dialogue option array, ready to use in a dialogue definition.
 */
global func DlgOptionCancelAlways(){ return [2147483646,-1,"Abbrechen","",0,[MCMX,0,0,0,0,-1],-1,0,"StopDialogue(pTarget)"];}
/**
 * A dialogue option that lets you cancel the dialogue, if DlvEventSetCancel() is set to true.
 * @return A dialogue option array, ready to use in a dialogue definition.
 */
global func DlgOptionCancelToggle(){ return [2147483646,-1,"Abbrechen","",0,[MCMX,0,0,0,0,-1],-1,"DlgVar(\"Cancel\",pTarget,pSpeaker)","StopDialogue(pTarget)"];}

/**
 * Sets the dialogue cancel variable to the specified state.
 * @param bState Set to false if you want to disable cancelling the dialogue.
 * @return string A string that can be used in with ProcessEvents()
 */ 
global func DlgEventSetCancel(bool bState){ return Format("CreateDlgVar(\"Cancel\",pTarget,pSpeaker)=%v",bState);}
global func DlgEventSafeRemoveVar(string variable){ return Format("!DlgVar(\"%s\",pTarget,pSpeaker)||RemoveObject(DlgVar(\"%s\",pTarget,pSpeaker))", variable, variable);}
global func DlgEventSafeRemoveObject(object obj){ var variable = ObjectNumber(obj); return Format("!Object(%d)||RemoveObject(Object(%d))", variable, variable);}

/**
 * Method for creating events or conditions with a DlgVar() in the dialogue object. 
 * @param prefix The string that is added before the DlgVar() command.
 * @param variable The DlgVar() identifier.
 * @param operation The string that is added after the DlgVar() command.
 * @example
 * {@code
 *  DlgStdVar("Create", "Dummy", " = 2")
 *  // is equal to
 * "CreateDlgVar(@qDummy@q, pTarget, pSpeaker) = 2"
 * }
 */
global func DlgStdVar(string prefix, string variable, string operation){ return Format("%sDlgVar(\"%s\",pTarget,pSpeaker)%s",prefix,variable,operation); }
/**
 * Method for creating events or conditions with a LocalN2() in the user controlled object, such as the crew member. 
 * @param prefix The string that is added before the LocalN2() command.
 * @param variable The LocalN2() identifier.
 * @param operation The string that is added after the LocalN2() command.
 * @example
 * {@code
 *  DlgObjVar("Create", "Dummy", " = 2")
 *  // is equal to
 * "CreateLocalN2(@qDummy@q, pTarget) = 2"
 * }
 */
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
/**
 * Method for creating events or conditions with a QuestStage(). 
 * @param prefix The string that is added before the QuestStage() command.
 * @param variable The quest identifier.
 * @param operation The string that is added after the QuestStage() command.
 * @example
 * {@code
 *  DlgQuestStage("Get", "Dummy", " == 2")
 *  // is equal to
 * "GetQuestStage(@qDummy@q, pTarget) == 2"
 * }
 */

global func DlgQuestStage(string prefix, string quest, string operator)
{
	return Format("%sQuestStage(\"%s\",pTarget)%s",prefix, quest, operator);
}
