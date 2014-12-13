/*-- Marvin, the sorcerer --*/

#strict 2
#appendto _STY

static const marvins_quest = "ArthursArmor";

public func MsgDialogueMarvin() {
	var asked_about_rock = "asked_about_rock";
	var rock_with_name = "rock_with_name";
	
    return [
	DlgOption(0, -1)->Text("Hello stranger!")
	->Events(DlgEventSetCancel(true)),
	DlgOption(2, [0, 3, 4, 5, 9], "Is this your tower?")->Text("Yes."),
	DlgOption(3, [0, 2, 4, 5, 9], "Can you make a rock with my name?")->Text("Sure.")
	->Conditions(DlgStdVar("", asked_about_rock,"==false"))
	->Events(DlgStdVar("Create", asked_about_rock, " = true")),
	DlgOption(4, [0, 2, 3, 9], "Please make a rock with my name on it!")->Text("Ok, here it is!")
	->Conditions(DlgStdVar("", asked_about_rock, "== true"))
	->Conditions(DlgStdVar("", rock_with_name, "== 0"))
	->Events(DlgStdVar("Create", rock_with_name, " = CreateObject(ROCK, 0, 0, NO_OWNER)"))
	->Events(DlgStdVar("", rock_with_name, "->SetPosition(pSpeaker->GetX() + 20, pSpeaker->GetY())"))
	->Events(DlgStdVar("", rock_with_name, "->SetName(GetName(pTarget))")),
	DlgOption(5, [0, 2, 3, 9], "Please make another rock with my name on it!")->Text("One rock is enough.")
	->Conditions(DlgStdVar("", rock_with_name, "!= 0")),
	DlgOption(6, [0, 2, 3, 4, 5], "Can I do anything for you?")->Text("Actually, yes")
	->Conditions(DlgQuestStage("Get", marvins_quest, "== 0"))
	->Events(DlgEventSetCancel(false)),
	DlgOption(7, 6, "What is it?")->Text("I have a magic armor for Arthur, and it has yet to be delivered to him"),
	DlgOption(8, [7], "Can't you deliver it yourself? He lives right behind your tower!")->Text("Oh, come on, what would be the fun in that?"),
	DlgOption(9, [7, 8], "Ok, I'll bring it to him")->Text("Great. It is in my tower, just grab it from the shelf")
	->Events(DlgEventSetCancel(true))
	->Events(Format("ActivateQuest(\"%s\",pTarget)", marvins_quest))
	->Events(SetQuestStage(marvins_quest, 1)) // so that the dialogue option for helping Marvin does not appear anymore 
	->Events("CreateContents(BRMR, FindObject(WTWR))"),
	DlgOption(10, -1, "Arthur has his armor")->Text("Thanks. Here, have a cookie!")
	->Conditions(DlgQuestStage("Get", marvins_quest, "== 2"))
	->Events(Format("SetQuestStage(\"%s\",3,pTarget,true)", marvins_quest)),
	DlgOptionCancelToggle()
];}

public func DscQuestInfoArthursArmor()
{
	return QstInfo("Armor Delivery");
}

public func DscQuestStagesArthursArmor()
{
	return [ QstStage(1)->Conditions("FindContents(BRMR, FindObject(CST2))")->Events("pQuest->~SetStage(8, pActivePlayer, true)"),
			 QstStage(2),
			 QstStage(3)->Events(["pQuest->~GiveReward(pActivePlayer,\"CreateContents(COKI,pActivePlayer)\")", Format("FinishQuest(\"%s\", pActivePlayer, true)", marvins_quest)]),
			 QstStage(4)->Events(["StartFilm()", "GetFilm()->PosCam(FindObject(KNIG),0,true)","SetCommand(FindObject(KNIG),\"Enter\", FindObject(CST2))", "pQuest->SetStage(5,0,true)"]),
			 QstStage(5)->Conditions("FindContents(KNIG, FindObject(CST2))")->Events(["Enter(FindObject(KNIG), FindObject(BRMR))", "SetCommand(FindObject(KNIG), \"MoveTo\", 0, 345, 390)", "pQuest->SetStage(6,0,true)"])->Timer(10),
			 QstStage(6)->Conditions("!GetCommand(FindObject(KNIG))")->Events(["FindObject(BRMR)->Activate(FindObject(KNIG))", "pQuest->SetStage(7,0,true)"]),
			 QstStage(7)->Conditions("GetAction(FindObject(KNIG)) == \"Walk\"")->Events(["StopFilm()", "pQuest->SetStage(2,0,true)"]),
			 QstStage(8)];	 
//	return[
//	[1, "FindContents(BRMR, FindObject(CST2))", "pQuest->~SetStage(8, pActivePlayer, true)"], // sets the quest stage to 8 for all players
//	[2], // this is just a dummy. The final stage is set in Marvin's dialogue
//	[3, 0, ["pQuest->~GiveReward(pActivePlayer,\"CreateContents(COKI,pActivePlayer)\")", Format("FinishQuest(\"%s\", pActivePlayer, true)", marvins_quest)]] ,
//	[4, 0, ["StartFilm()", "GetFilm()->PosCam(FindObject(KNIG),0,true)","SetCommand(FindObject(KNIG),\"Enter\", FindObject(CST2))", "pQuest->SetStage(5,0,true)"]],
//	[5, "FindContents(KNIG, FindObject(CST2))", ["Enter(FindObject(KNIG), FindObject(BRMR))", "SetCommand(FindObject(KNIG), \"MoveTo\", 0, 345, 390)", "pQuest->SetStage(6,0,true)"], 10],
//	[6, "!GetCommand(FindObject(KNIG))", ["FindObject(BRMR)->Activate(FindObject(KNIG))", "pQuest->SetStage(7,0,true)"]],
//	[7, "GetAction(FindObject(KNIG)) == \"Walk\"", ["StopFilm()", "pQuest->SetStage(2,0,true)"]],
//	[8]
//	];
}

func DayNightCycleMarvin()
{
	return [[6,0,"AddBehaviour(pTarget, gBehaviour_Wander_Point, 20, [[215, 400],30,5,35,50], gBehaviour_Priority_Idle)"],
	        [18,0,"RemoveBehaviour(pTarget, gBehaviour_Wander_Point, gBehaviour_Priority_Idle)"]];
}