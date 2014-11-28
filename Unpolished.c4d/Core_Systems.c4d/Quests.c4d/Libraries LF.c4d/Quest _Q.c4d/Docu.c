/*-- In this tutorial you will learn how to set up a quest in a demo scenario. If you have not done so yet, read 
the [TODO: Link to Scenario Basics: Adding a dialogue] first.
{@section Quest basics}
A quest is defined by three parts: An identifier, a title and stages. Just as dialogues, quests can be be defined by passing the title
and phases directly, or by defining them as a function in the story object or scenario script. A quest with the identifier "SampleQuest"
needs the following functions in your story object:
{@code
public func DscQuestSampleQuest(bool bStages) // DscQuest* is called, where * is substituted by the identifier
{
	if (bStages) return []; // does nothing

	return ["A sample quest"]; // the quest title
}
}
The quest phases are an array of quest stage arrays. These define, what the quest does at any given moment. A quest stage array looks like this:
{@code
[Quest stage ID: int, conditions: string or array, events: string or array, timer: int]
}
At the moment, there is no helper object like the dialogue builder, so you have to remember this structure yourself. The quest stage specifies that, as long as
the stage with this ID is active, every {@i timer} frames if all {@i conditions} are fulfilled, then the {@i events} happen.
If no timer is specified, then the conditions are checked every 36 frames. Quest stage 0 is always active, and it usually can be left blank, so that it does nothing.@br
The active quest stage is tracked for every player individually, but it is also possible to change the stage for all players simultaneously. This gives you several options:
The players each complete the quest, the players can compete who finishes the quest first, the players can finish the quest cooperatively.@br
A very important aspect of quests are dummy stages, that is an empty stage which merely has an ID, such as {@i [1]}. You can use these to "pause" the quest, because you need
input from a dialogue or some other event to happen, that is not controlled by the quest itself.
{@section Sample quest}
Now we are going to add a sample quest to our demo scenario: Marvin has a magic armor for the knight who lives in the castle. He wants us to bring it to the knight,
and as a reward he gives us a cookie.@br
First, let us add the knight and the castle to our scenario, and create the object that manages the quest progress:
{@code
static const marvins_quest = "ArthursArmor";
@br
func DoStartSzen()
{
	// create Marvin the sorcerer
	var sorcerer = CreateObject(SCLK, 200, 390, NO_OWNER);
	sorcerer->SetName("Marvin");
	AttachDialogue(sorcerer, "Marvin");
	@br
	// create Marvin's tower
	var tower = CreateObject(WTWR, 215, 400, NO_OWNER);
	@br
	// create the castle
	var castle = CreateObject(CST2, 330, 400, NO_OWNER);
	@br
	// create Arthur the knight
	var knight = CreateObject(KNIG, 345, 390, NO_OWNER);
	knight->SetName("Arthur");
	AttachDialogue(knight, "Arthur");
	@br
	// create the quest
	CreateQuest(marvins_quest);
}
}
Next, we create a file NPC_Arthur.c in System.c4g with the following dialogue:
{@code
/bc-- Arthur, the knight --bc/
@br
#strict 2
#appendto _STY
@br
public func MsgDialogueArthur() {
	return [
	DlgOption(0, -1)->Text("Greetings."),
	DlgOption(1, -1, "Not much to say?")->Text("No, talk to the sorcerer first, he may have a 'quest' for a 'mighty adventurer' like you.")
	->Conditions(DlgQuestStage("Get", marvins_quest, "== 0")),
	DlgOption(2, -1, "I have your armor.")->Text("Ok, put it in the castle, please.")
	->Conditions("FindContents(BRMR, pTarget)"),
	DlgOption(3, -1, "Your armor is in the castle.")->Text("Thank you. Marvin may have a 'reward' for you.")
	->Conditions("FindContents(BRMR, FindObject(CST2))")
	->Conditions(DlgQuestStage("Get", marvins_quest, "> 0")), // quest is active
	DlgOptionCancelAlways()
	];
}
}
This is just so that Arthur has something to say, but this does not actually matter. The most important part is the quest that we are adding.
Create a new script file, put it in Arthur's script because it is his armor, or put it in Marvin's script because he gives you the reward.
{@code
public func DscQuestArthursArmor(bool bStages)
{
	if (bStages) return[
	[1, "FindContents(BRMR, FindObject(CST2))", "pQuest->~SetStage(2, pActivePlayer, true)"], // this stage is activated by Marvin's dialogue. Once the armor is in the castle, it sets the quest stage to 2 for all players
	[2], // this is just a dummy. The final stage is set in Marvin's dialogue
	[3, 0, ["pQuest->~GiveReward(pActivePlayer,@qCreateContents(COKI,pActivePlayer)@q)", Format("FinishQuest(@q%s@q, pActivePlayer, true)", marvins_quest)]] // rewards the player with a cookie and successfully finishes the quest for all players
	];
@br
	return [ "Armor Delivery" ];
}
}
As you see, the quest does not do much. Before talking to Marvin, it does nothing. Once we set the quest stage to 1 it checks whether the armor is in the castle every 36 frames, and if so it sets the stage to 2.
Marvin's dialogue will get another option when the quest stage is 2. Confirming that option will set the stage to 3 and, since this condition is always true, it will immediately fire the events that give a cookie
to the player and finish the quest.@br
A simple quest like this could be done with DlgStdVar() and DlgObjVar(), too. This is a great exercise for practicing the use of variables. Modeling the quest with a quest framework has the advantage that it handles
multiplayer quests better and tracks them in the quest log. The quest log is available in the {@i _CLN} Clonk, but you should be able to add a context menu for the normal Clonk as well.@br
Finally, we need to expand Marvin's dialogue a bit:
{@code
public func MsgDialogueMarvin()
{
	var asked_about_rock = "asked_about_rock";
	var rock_with_name = "rock_with_name";
	@br
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
	->Events(Format("ActivateQuest(@q%s@q,pTarget)", marvins_quest))
	->Events(SetQuestStage(marvins_quest, 1)) // so that the dialogue option for helping Marvin does not appear anymore 
	->Events("CreateContents(BRMR, FindObject(WTWR))"),
	DlgOption(10, -1, "Arthur has his armor")->Text("Thanks. Here, have a cookie!")
	->Conditions(DlgQuestStage("Get", marvins_quest, "== 2"))
	->Events(Format("SetQuestStage(@q%s@q,3,pTarget,true)", marvins_quest)),
	DlgOptionCancelToggle()
];
}
}
To avoid the dialogue options popping up everywhere, the parent nodes are properly set. Also, the player should not be able to cancel 
the dialogue once he chooses option 7, so we changed the script in this regard. Congratulations, we finished our first quest.
@title Demo Scenario: Adding a quest
@category #Tutorials
@file tutorialDemoScenario002Quest.html
@author Marky
--*/

/*--
In this tutorial you will learn how to set up a cut scene in a demo scenario. If you have not done so yet, read 
the [TODO: Link to Scenario Basics: Adding a quest] first.@br
In the last tutorial we added the quest to give an armor to the knight Arthur. This time we modify Arthur's dialogue,
such that he puts on the armor in a cutscene, once you tell him that it is in the castle.
The cutscene is just more quest stages, but it uses the camera object to prevent the players from moving. First, we expand Arthur's dialogue,
such that he starts a new quest stage once we tell him about the armor in his castle:
{@code
public func MsgDialogueArthur() {
	return [
	DlgOption(0, -1)->Text("Greetings."),
	DlgOption(1, -1, "Not much to say?")->Text("No, talk to the sorcerer first, he may have a 'quest' for a 'mighty adventurer' like you.")
	->Conditions(DlgQuestStage("Get", marvins_quest, "== 0")),
	DlgOption(2, -1, "I have your armor.")->Text("Ok, put it in the castle, please.")
	->Conditions("FindContents(BRMR, pTarget)"),
	DlgOption(3, -1, "Your armor is in the castle.")->Text("Thank you. Marvin may have a 'reward' for you.")
	->Conditions("FindContents(BRMR, FindObject(CST2))")
	->Conditions(DlgQuestStage("Get", marvins_quest, "== 8"))
	->Events("StopDialogue(pTarget)") // we have to stop the dialogue or else the knight will not execute commands
	->Events(Format("SetQuestStage(@q%s@q,4,pTarget,true)", marvins_quest)), // this is the quest stage that does the cutscene stuff
	DlgOptionCancelAlways()
	];
}
}
Now, we expand the quest by the relevant stages.
{@code
public func DscQuestArthursArmor(bool bStages)
{
	if (bStages) return[
	[1, "FindContents(BRMR, FindObject(CST2))", "pQuest->~SetStage(8, pActivePlayer, true)"], // sets the quest stage to 8 for all players
	[2], // this is just a dummy. The final stage is set in Marvin's dialogue
	[3, 0, ["pQuest->~GiveReward(pActivePlayer,@qCreateContents(COKI,pActivePlayer)@q)", Format("FinishQuest(@q%s@q, pActivePlayer, true)", marvins_quest)]] ,
	[4, 0, ["StartFilm()", "GetFilm()->PosCam(FindObject(KNIG),0,true)","SetCommand(FindObject(KNIG),@qEnter@q, FindObject(CST2))", "pQuest->SetStage(5,0,true)"]],
	[5, "FindContents(KNIG, FindObject(CST2))", ["Enter(FindObject(KNIG), FindObject(BRMR))", "SetCommand(FindObject(KNIG), @qMoveTo@q, 0, 345, 390)", "pQuest->SetStage(6,0,true)"], 10],
	[6, "!GetCommand(FindObject(KNIG))", ["FindObject(BRMR)->Activate(FindObject(KNIG))", "pQuest->SetStage(7,0,true)"]],
	[7, "GetAction(FindObject(KNIG)) == @qWalk@q", ["StopFilm()", "pQuest->SetStage(2,0,true)"]],
	[8]
	];
@br
	return [ "Armor Delivery" ];
}
} 
@title Demo Scenario: Adding a cutscene
@category #Tutorials
@file tutorialDemoScenario003Cutscene.html
@author Marky
--*/