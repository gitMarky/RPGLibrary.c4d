/*-- Buch --*/

#strict 2

#include LF_R // ist lesbar

protected func Initialize()
{
  SetColorDw(HSL(Random(256), 255, Random(128)+128));
  TestBook();
}
  
protected func Activate(object player)
{
	[$CtrlRead$]

	StartDialogue( player );
	return 1;
}

public func TestBook()
{
	//[ int iIndex, array/int iParentIndex, string szMenuOption, string szText, int iObjectNr, array aMenuStyle, array aTextStyle , array aConditions, array aEvents ]

	/*SetDialogue([
		[0,-1,"","Dies ist die Erste Seite des Buches"],
		[1,0,"Zu S.2","Dies ist die zweite Seite des Buches",0,[MCM2]],
		[2,[0,1],"Zu S.3","Dies ist die dritte Seite des Buches",0,[MCM3]],
		[3,[1,2,6],"Zurück zu S.1","Dieser Text erscheint nicht, wir springen direkt zu Seite 1!",0,[MCMC,0,0,0,0,1]],
		[4,0,"","Dies ist ein Info-Text",0,0],
		[5,2,"Genauer Untersuchen","Hier gibt der Clonk einen Kommentar ab!",-1,[MCMQ]],
		[6,[0,1,2,5],"Zu S.4","Dies ist die vierte Seite des Buches. Außerdem wird es hier farbig und sofort angezeigt|Eine Sensation, bei so einem langen Text",0,[MCM4],[true,RGB(255,0,0),true]]
		]);*/

	// wichtig: die Sachen erscheinen auch in der Reihenfolge, wie sie im Array stehen. D.h. zurück zu S.1 immer ans ende usw.

	/*
	SetDialogue([
		[0,-1,"","Dies ist die Erste Seite des Buches"],
		[1,0,"Zu S.2","Dies ist die zweite Seite des Buches",0,[MCM2]],
		[2,[0,1],"Zu S.3","Dies ist die dritte Seite des Buches",0,[MCM3]],
		[4,0,"","Dies ist ein Info-Text",0,0],
		[5,2,"Genauer Untersuchen","Hier gibt der Clonk einen Kommentar ab!",-1,[MCMQ]],
		[6,[0,1,2,5],"Zu S.4","Dies ist die vierte Seite des Buches. Außerdem wird es hier farbig und sofort angezeigt|Eine Sensation, bei so einem langen Text",0,[MCM4],[true,RGB(255,0,0),true]],
		[3,[1,2,6],"Zurück zu S.1","Dieser Text erscheint nicht, wir springen direkt zu Seite 1!",0,[MCMC,0,0,0,0,1]],
		[7,[0,1,2,6],"Schließen","",0,[MCMX,0,0,0,0,-1]]
	]);*/

	SetDialogue([
		[0,-1,0,"Dies ist die Erste Seite des Buches",0,MCMC],
		[1,0,"Zu S.2","Dies ist die zweite Seite des Buches",0,MCM2],
		[2,[0,1],"Zu S.3","Dies ist die dritte Seite des Buches",0,MCM3],
		[4,0,"","Dies ist ein Info-Text",0,0],
		[5,2,"Genauer Untersuchen","Hier gibt der Clonk einen Kommentar ab!",-1,[MCMQ]],
		[6,[0,1,2,5],"Zu S.4","Dies ist die vierte Seite des Buches. Außerdem wird es hier farbig und sofort angezeigt|Eine Sensation, bei so einem langen Text",0,[MCM4],[true,RGB(255,0,0),true]],
		[3,[1,2,6],"Zurück zu S.1","Dieser Text erscheint nicht, wir springen direkt zu Seite 1!",0,[MCMC,0,0,0,0,1]],
/*
		[8,-1,"Ich habe ein Schwert","Diese Option erscheint, wenn der Clonk ein Schwert hat",-1,[SWOR],0,[["GetItem",-1,SWOR,">= 1"]]],
		[9,-1,"Ich habe ein Schwert2","Diese Option erscheint, wenn der Clonk ein Schwert hat",-1,[AXE1,0,1],0,[["GetItem",-1,SWOR,">= 1"]]],
		[10,-1,"Ich habe kein Gold","Blabla",0,[GOLD,0,1],0,[["GetWealth",-1,"== 0"]]],
		[11,-1,"Ich habe Gold","Blabla",0,[GOLD,0,1],0,[["GetWealth",-1,"> 0"]]],
		[12,-1,"Du hast kein Gold","Blabla",0,[GOLD,0,1],0,[["GetWealth",0,"== 0"]]],
*/
		[14,-1,"2 Ich habe kein Gold","Blabla",0,[GOLD,0,1],0,"pTarget->~GetMoney() == 0"],
		[15,-1,"2 Ich habe Gold","Blabla",0,[GOLD,0,1],0,"GetWealth(GetOwner(pTarget)) > 0"],
		[16,-1,"2 Du hast kein Gold","Blabla",0,[GOLD,0,1],0,"pSpeaker->~GetMoney() == 0"],
		[17,-1,"Es gibt Schwert und Schild","Blabla",0,[SWOR,0,1],0,[ "FindObject(SWOR)", "FindObject(SHIE)" ]],
		[13,-1,"Sepp ist eine Variable","Blabla",0,[CLNK,0,1],0,"GlobalN(\"Sepp\")"],
		[18,-1,"Gib mir einen Flint","Hier hast Du ihn",0,[FLNT],0,"!FindContents(FLNT,pTarget)","CreateContents(FLNT,pTarget)"],
		[19,-1,"Hier hast Du den Flint zurück","Danke",0,[FLNT],0,"FindContents(FLNT,pTarget)","!FindContents(FLNT,pTarget) || RemoveObject(FindContents(FLNT,pTarget));"],
		[7,-1,"Schließen","",0,[MCMX,0,0,0,0,-1],-1,0,"CloseMenu(pTarget)"]
	]);

	Activate(GetHiRank());
}

public func GetDialogueColor(){ Log("Color!!"); return RGB(0,255,255); }