/*-- John, the King --*/

#strict 2
#appendto _STY

public func MsgDialogueKing() {
	return [
	DlgOption(0, -1)->Text("Greetings."),
	DlgOption(1, -1, "Test 1")->Text("The icon should have been a {{FLNT}}")->Icon(FLNT),
	DlgOption(2, -1, "Test 2")->Text("Should display the clonk rank symbol")->Icon(CLNK)->MenuExtra(1),
	DlgOption(3, -1, "Test 3")->Text("Should display the mouse cursor axe")->Icon(MCCR)->MenuExtra(2, 3),
	DlgOption(4, -1, "Test 4")->Text("Blub")->NextDialogue(2),
	DlgOption(5,  3, "Test 5")->Text("This was displayed red until you collected a rock")->Conditions("true")->Conditions("pTarget->FindContents(ROCK)")->RequiredConditions(1),
	DlgOption(6, -1, "Test 6")->Text("This text should be green")->MenuItemColor(RGB(0, 255, 0)),
	DlgOption(7, -1, "Test 7")->Text("This has a new menu caption")->MenuCaption("John the funky king"),
	DlgOption(8, -1, "Test 8")->Text("Cool menu deco")->MenuDeco(HZDC),
	DlgOption(9, -1, "Test 9")->Text("aaaaaaaaa aaaaaaaa aaaaaaaaaaaaaaa aaaaaaaa aaaaaaaaaaa aaaaa aaaaaaaa aaaaaaaa aaaaaaa aaaaaa aaaaaa aaaaaa aaaaaa")->DisplayInstantly(),
	DlgOption(10, -1, "Test 10")->Text("My text has no name displayed")->DisableSpeakerColor(),
	DlgOption(11, -1, "Test 11")->Text("Either the option or the message is blue")->TextColor(RGB(0, 0, 200)),
	DlgOptionCancelAlways()
	];
}

func DayNightCycleKing()
{
	return [[6,0,"AddBehaviour(pTarget, gBehaviour_Wander_Area, 20, [284,332,1,15,75], gBehaviour_Priority_Idle)"],
	        [18,0,"RemoveBehaviour(pTarget, gBehaviour_Wander_Area, gBehaviour_Priority_Idle)"]];

}