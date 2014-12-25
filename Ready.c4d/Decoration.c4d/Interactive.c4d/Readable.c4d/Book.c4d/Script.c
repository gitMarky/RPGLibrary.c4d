/*--
An item, that looks like a book. Can have a dialogue.

@note The item gets initialized with a standard dialogue.

@title Book
@version 0.1.0
@author Sven2 (Graphics), Marky (Script)
 --*/

#strict 2

#include LF_R // is readable

protected func Initialize()
{
	SetColorDw(HSL(Random(256), 255, Random(128) + 128));
}
 
/**
 * Starts the dialogue.
 *
 * @par player The object that activated the book. The dialogue is displayed to this object.
 */ 
protected func Activate(object player)
{
	[$CtrlRead$]
	
	StartDialogue(player);
	return true;
}

private func TestBook()
{
/*
	SetDialogue
	(
		[
			[0, -1, 0, "Dies ist die Erste Seite des Buches", 0, MCMC],
			[1, 0, "Zu S.2", "Dies ist die zweite Seite des Buches", 0, MCM2],
			[2, [0, 1], "Zu S.3", "Dies ist die dritte Seite des Buches", 0, MCM3],
			[4, 0, "", "Dies ist ein Info-Text", 0, 0],
			[5, 2, "Genauer Untersuchen", "Hier gibt der Clonk einen Kommentar ab!", -1, [MCMQ]],
			[
				6,
				[0, 1, 2, 5],
				"Zu S.4",
				"Dies ist die vierte Seite des Buches. Außerdem wird es hier farbig und sofort angezeigt|Eine Sensation, bei so einem langen Text",
				0,
				[MCM4],
				[true, RGB(255, 0, 0), true]
			],
			[
				3,
				[1, 2, 6],
				"Zurück zu S.1",
				"Dieser Text erscheint nicht, wir springen direkt zu Seite 1!",
				0,
				[MCMC, 0, 0, 0, 0, 1]
			],
			[14, -1, "2 Ich habe kein Gold", "Blabla", 0, [GOLD, 0, 1], 0, "pTarget->~GetMoney() == 0"],
			[15, -1, "2 Ich habe Gold", "Blabla", 0, [GOLD, 0, 1], 0, "GetWealth(GetOwner(pTarget)) > 0"],
			[16, -1, "2 Du hast kein Gold", "Blabla", 0, [GOLD, 0, 1], 0, "pSpeaker->~GetMoney() == 0"],
			[
				17,
				-1,
				"Es gibt Schwert und Schild",
				"Blabla",
				0,
				[SWOR, 0, 1],
				0,
				["FindObject(SWOR)", "FindObject(SHIE)"]
			],
			[13, -1, "Sepp ist eine Variable", "Blabla", 0, [CLNK, 0, 1], 0, "GlobalN(\"Sepp\")"],
			[
				18,
				-1,
				"Gib mir einen Flint",
				"Hier hast Du ihn",
				0,
				[FLNT],
				0,
				"!FindContents(FLNT,pTarget)",
				"CreateContents(FLNT,pTarget)"
			],
			[
				19,
				-1,
				"Hier hast Du den Flint zurück",
				"Danke",
				0,
				[FLNT],
				0,
				"FindContents(FLNT,pTarget)",
				"!FindContents(FLNT,pTarget) || RemoveObject(FindContents(FLNT,pTarget));"
			],
			[7, -1, "Schließen", "", 0, [MCMX, 0, 0, 0, 0, -1], -1, 0, "CloseMenu(pTarget)"]
		]
	);
*/

	SetDialogue
	(
		[
			DlgOption(0, -1)->Text("Dies ist die Erste Seite des Buches")->Icon(MCMC),
			DlgOption(1, 0, "Zu S.2")->Text("Dies ist die zweite Seite des Buches")->Icon(MCM2),
			DlgOption(2, [0, 1], "Zu S.3")->Text("Dies ist die dritte Seite des Buches")->Icon(MCM3),
			DlgOption(4, 0)->Text("Dies ist ein Info-Text"),
			DlgOption(5, 2, "Genauer Untersuchen")->Text("Hier gibt der Clonk einen Kommentar ab!")->Speaker(gDialogue_Object_Target)->Icon(MCMQ),
			DlgOption(6, [0, 1, 2, 5], "Zu S.4")
				->Text("Dies ist die vierte Seite des Buches. Außerdem wird es hier farbig und sofort angezeigt|Eine Sensation, bei so einem langen Text")
				->Icon(MCM4)->DisplayInstantly()->TextColor(RGB(255, 0, 0)),
			DlgOption(3, [1, 2, 6], "Zurück zu S.1")->Text("Dieser Text erscheint nicht, wir springen direkt zu Seite 1!")->Icon(MCMC)->NextDialogue(1),
			
			DlgOption(14, -1, "2 Ich habe kein Gold")->Text("Blabla")->Icon(GOLD)->RequiredConditions(0)->Conditions("pTarget->~GetMoney() == 0"),
			DlgOption(15, -1, "2 Ich habe Gold")->Text("Blabla")->Icon(GOLD)->RequiredConditions(0)->Conditions("GetWealth(GetOwner(pTarget)) > 0"),
			DlgOption(16, -1, "2 Du hast kein Gold")->Text("Blabla")->Icon(GOLD)->RequiredConditions(0)->Conditions("pSpeaker->~GetMoney() == 0"),
			DlgOption(17, -1, "Es gibt Schwert und Schild")->Text("Blabla")->Icon(SWOR)->RequiredConditions(0)->Conditions(["FindObject(SWOR)", "FindObject(SHIE)"]),
			DlgOption(14, -1, "Sepp ist eine Variable")->Text("Blabla")->Icon(CLNK)->RequiredConditions(0)->Conditions("GlobalN(\"Sepp\")"),
			DlgOption(18, -1,"Gib mir einen Flint")->Text("Hier hast Du ihn")->Icon(FLNT)
				->Conditions("!FindContents(FLNT,pTarget)")
				->Events("CreateContents(FLNT,pTarget)"),
			DlgOption(19, -1, "Hier hast Du den Flint zurück")->Text("Danke")->Icon(FLNT)
				->Conditions("FindContents(FLNT,pTarget)")
				->Events("!FindContents(FLNT,pTarget) || RemoveObject(FindContents(FLNT,pTarget));"),
			DlgOption(7, -1, "Schließen")->Text("")->Icon(MCMX)->NextDialogue(-1)->Events("CloseMenu(pTarget)")
		]
	);

	Activate(GetHiRank());
}

public func GetDialogueColor()
{
	return RGB(0, 255, 255);
}