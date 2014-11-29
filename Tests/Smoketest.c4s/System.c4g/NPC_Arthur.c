/*-- Arthur, the knight --*/

#strict 2
#appendto _STY

public func MsgDialogueArthur() {
	return [
	DlgOption(0, -1)->Text("Greetings."),
	DlgOption(1, -1, "Not much to say?")->Text("No, talk to the sorcerer first, he may have a 'quest' for a 'mighty adventurer' like you")
	->Conditions(DlgQuestStage("Get", marvins_quest, "== 0")),
	DlgOption(2, -1, "I have your armor")->Text("Ok, put it in the castle, please.")
	->Conditions("FindContents(BRMR, pTarget)"),
	DlgOption(3, -1, "Your armor is in the castle.")->Text("Thank you. Marvin may have a 'reward' for you.")
	->Conditions("FindContents(BRMR, FindObject(CST2))")
	->Conditions(DlgQuestStage("Get", marvins_quest, "== 8"))
	->Events("StopDialogue(pTarget)")
	->Events(Format("SetQuestStage(\"%s\",4,pTarget,true)", marvins_quest)),
	DlgOptionCancelAlways()
	];
}

//-> Clonk::GetX()
// = 284
//-> Clonk::GetX()
// = 336
//-> Clonk::GetY()
// = 358
 
func DayNightCycleArthur()
{
	return [[6,0,"AddBehaviour(pTarget, gBehaviour_WanderArea, 20, [284,336,1,15,75], gBehaviour_Priority_Idle)"],
	        [18,0,"RemoveBehaviour(pTarget, gBehaviour_WanderArea, gBehaviour_Priority_Idle)"]];

}