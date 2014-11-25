/*-- In this tutorial you will learn how to set up a dialogue in a demo scenario. If you have not done so yet, read 
the [TODO: Link to Scenario Basics: Script Architecture] first.
{@section Creating the scenario}
Create a new scenario with the options
{@code
[Definitions]
Definition1=Objects.c4d
Definition2=Knights.c4d
Definition3=Fantasy.c4d
Definition4=RPGLibrary.c4d
@br 
[Game]
StructNeedEnergy=0
@br 
[Player1]
Crew=CLNK=1
@br
[Player2]
Crew=CLNK=1
@br
[Player3]
Crew=CLNK=1
@br
[Player4]
Crew=CLNK=1
}
and the minimal scenario script
{@code
/bc
Scenario script
bc/
@br
#strict 2
@br
	func Initialize()
	{
		// this should be empty
	}
@br
	func DoStartSzen()
	{
		// do the scenario setup, empty for now
	}
@br
	protected func InitializePlayer(int player, int tx, int ty, object pBase, int iTeam)
	{
		DoInitPlayer(player);
	}
@br
	// information for the script goal object
	public func IsGoalFulfilled() { return false; }
	public func GetGoalDesc() { return "This scenario cannot be fulfilled until it has valid script goals"; }
}
Set the landscape to be flat. Starting this scenario should already work, but you cannot do much.
{@section Adding a NPC}
First of all we will add a sorcerer to the scenario, and his wizard tower.
{@code
func DoStartSzen()
{
	// create Marvin the sorcerer
	var sorcerer = CreateObject(SCLK, 200, 390, NO_OWNER);
	sorcerer->SetName("Marvin");
	AttachDialogue(sorcerer, "Marvin");
@br
	// create Marvin's tower
	var tower = CreateObject(WTWR, 215, 400, NO_OWNER);
}
}
Now we have Marvin standing around, and pressing [Up] in front of him would activate a dialogue. However, we have not 
defined that dialogue yet. At the moment, nothing happens, but your Clonk will not be able to jump in front of Marvin.
{@section Defining the dialogue}
Dialogues are defined as arrays, containing further arrays with dialogue information. For the mechanical part of those
arrays, see [TODO]. More important is knowing how the engine gets the dialogue information when calling AttachDialogue().
It accepts the dialogue array itself as a parameter, or a string "X". If you pass a string, then it tries getting the array
from the function MsgDialogueX().@br
First of all, it looks for that function in the story object, and if it cannot find anything, then it tries calling a
global function of that name. Conveniently, our basic scenario script has already created a story object, so we can
define the dialogue there.@br
Create the group "System.c4g" in your scenario and add a script "NPC_Marvin.c". You can sort the scripts any way you want,
but quests and day-night-cycles also look up the story object, so defining the NPC behaviour, his dialogue and his quests
in the same file can be of advantage.@br
Anyway, here is Marvin's dialogue:
{@code
/bc-- Marvin, the sorcerer --bc/
@br
#strict 2
#appendto _STY
@br
public func MsgDialogueMarvin()
{
return [
	DlgOption(0, -1)->Text("Hello stranger!")
];
}
}
This does the same as
{@code
/bc-- Marvin, the sorcerer --bc/
@br
#strict 2
#appendto _STY
@br
public func MsgDialogueMarvin()
{
	return [[0,-1,0,"Hello stranger!"]];
}
}
but is more readable. The first version uses a dialogue builder object that saves the input in the same format as
the second version. Next to readability, using the builder object has the advantage that the dialogue will still
be correct if the internal structure of the array gets changed in a future version of RPG Library.@br
Otherwise you have to adjust all your dialogue arrays, which can be tedious work. Also this is a fairly simple
script, but with more advanced options, such as portraits and text styles, remembering the correct order for all
entries is quite hard.@br
Our dialogue now creates a dialogue with ID 0 that is automatically displayed when the dialogue opens, since
ID 0 is the default node in a dialogue tree, see DlgOption(). Then it assigns the text "Hello stranger!" to that
node. With this configuration, the dialogue can be opened, but there is no option to cancel the dialogue.
So we expand our array. An option for canceling a dialogue is so common, that we provided a function for it:
{@code
public func MsgDialogueMarvin()
{
	return [
	DlgOption(0, -1)->Text("Hello stranger!"),
	DlgOptionCancelAlways()];
}
}
{@section Introducing variables, conditions and events}
Now we actually want to ask Marvin something. Usually you have options that are always there, and options that
can be asked only once. Variables, conditions and events solve this issue:
{@code
public func MsgDialogueMarvin()
{
	var asked_about_rock = "asked_about_rock";
	var rock_with_name = "rock_with_name";
@br
    return [
	DlgOption(0, -1)->Text("Hello stranger!"),
	DlgOption(2, -1, "Is this your tower?")->Text("Yes."), // this dialogue will be displayed after every choice
	DlgOption(3, -1, "Can you make a rock with my name?")->Text("Sure.")
	->Conditions(DlgStdVar("", asked_about_rock,"==false")) // is only displayed if you have not asked about the rock yet
	->Events(DlgStdVar("Create", asked_about_rock, " = true")), // sets a variable in Marvin, that you have asked him about the rock 
	DlgOption(4, -1, "Please make a rock with my name on it!")->Text("Ok, here it is!")
	->Conditions(DlgStdVar("", asked_about_rock, "== true")) // is only displayed if you have asked about the rock
	->Conditions(DlgStdVar("", rock_with_name, "== 0")) // and if the rock has not been created
	->Events(DlgStdVar("Create", rock_with_name, " = CreateObject(ROCK, 0, 0, NO_OWNER)")) // once we choose this option, the rock gets spawned
	->Events(DlgStdVar("", rock_with_name, "->SetPosition(pSpeaker->GetX() + 20, pSpeaker->GetY())")) // we drop it right of Marvin
	->Events(DlgStdVar("", rock_with_name, "->SetName(GetName(pTarget))")), // we give it the name of the guy who asked Marvin
	DlgOption(5, -1, "Please make another rock with my name on it!")->Text("One rock is enough.")
	->Conditions(DlgStdVar("", rock_with_name, "!= 0")), // alternate dialogue if the rock has been created
	DlgOptionCancelAlways()];
}
}
This should be clear already. The DlgStdVar() syntax might be confusing. Please suggest better options in the forum!
There are two comfortable ways for saving and accessing DlgStdVar() and DlgObjVar() is the object that the variable
is connected with:@br
DlgStdVar() saves the information in the object that hosts the dialogue, so it can be accessed only there. DlgObjVar()
instead is stored in the object controlled by the player. That means it can transport information from one dialogue to
another, examples will follow.@br
In our example, the Clonk you control can ask about the rock only once. Add another Clonk to your crew and he will be able
to ask about the rock, too. The same goes for other players.
It is recommended to save the variable identifiers such as {@i "asked_about_rock"} in a variable for easier debugging and changes.
The same applies to the dialogue node indices, but it was not applied in this example.
@title Demo Scenario: Adding a dialogue
@category #Tutorials
@file tutorialDemoScenario001Dialogue.html
@author Marky
--*/
