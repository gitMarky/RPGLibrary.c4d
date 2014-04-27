/*-- Buch --*/

#strict 2
#include LF_R // ist lesbar, oh Wunder :)

local aQuestIndex;
local aQuestOpen;
local aQuestDone;
local aQuestFail;
local aQuestDesc;

local aDialogue;

public func QuestColorText(){ return RGB(255,255,0);}
public func QuestColorNew(){ return RGB(255,0,0);}
public func QuestColorOld(){ return RGB(150,0,0);}


public func AddQuest(szName)
{
	Sound("Ding",1,0,0,GetOwner()+1);
	PushFront(szName,aQuestOpen);
	PushBack(szName,aQuestIndex);
	PushBack("",aQuestDesc);
}

public func FinishQuest(szName)
{
	Sound("Ding",1,0,0,GetOwner()+1);
	PushFront(szName,aQuestOpen);
	DropArrayItem(szName,aQuestOpen);
}

public func FailQuest(szName)
{
	PushFront(szName,aQuestFail);
	DropArrayItem(szName,aQuestOpen);
}

protected func Initialize()
{
	SetColorDw(HSL(Random(256), 255, Random(128)+128));
	aQuestOpen = aQuestDone = aQuestFail = aQuestDesc = aQuestIndex = [];

	SetOwner(2);
	AddQuest("MainQuest");
	Activate(GetHiRank(2));
}

protected func Activate(object pPlayer)
{
	[$CtrlRead$]

	SetOwner(GetOwner(pPlayer));
	ConstructDialogue(pPlayer);
	StartDialogue( pPlayer );
	return 1;
}

// Baut die Dialogfunktion des Buches auf
public func ConstructDialogue( pPlayer )
{
	// bisherigen Dialog löschen
	aDialogue = [];

	PushBack([0,-1,0,"Dies ist die Zusammenfassung aller Aufgaben",0,SCRG,[false,0,0,0,false,-1,"Journal"]],aDialogue);

	var index = 1;

	for( var quest in aQuestOpen)
	{
		var log = GetQuestLog( quest, pPlayer );
		if( GetType(log) != C4V_Array ) continue;

		var name = log[0][1];
		var lines,

		// Questbeschreibung zusammenbauen
		var desc = "", lastspeaker = "";
		for( var i=GetLength(log)-1; i >= 0; i-- )
		{
			var speaker = log[i][0], text = log[i][1];

			if( lastspeaker == speaker)
			{
				desc = Format("%s|%s",desc,ColorizeString(text,QuestColorText()));
			}
			else
			{
				if( desc == "")
					desc = Format("%s|%s",speaker,ColorizeString(text,QuestColorText()));
				else
					desc = Format("%s||%s|%s",desc,speaker,ColorizeString(text,QuestColorText()));
			}
			lastspeaker = speaker;
		}

		// Dialogoption zusammenbauen
		var qIndex = GetArrayItemPosition(quest,aQuestIndex);
		var olddesc = aQuestDesc[qIndex];

		var dwClr = QuestColorOld();
		if(desc != olddesc) dwClr = QuestColorNew();

		var aMenuStyle = [ NONE, dwClr ];

		var aTextStyle = [ false, 0, true, 0, false, -1, name ];
		var parent = 0;

		var dlg = [ index, parent, name, desc, 0, aMenuStyle, aTextStyle , "true", 0 ];
		PushBack(dlg,aDialogue);

		index++;


	}
	/*PushBack(,aDialogue);


	[0,-1,0,"Dies ist die Erste Seite des Buches",0,MCMC],
	[1,0,"Zu S.2","Dies ist die zweite Seite des Buches",0,MCM2],
	[2,[0,1],"Zu S.3","Dies ist die dritte Seite des Buches",0,MCM3],
	[4,0,"","Dies ist ein Info-Text",0,0],
	[5,2,"Genauer Untersuchen","Hier gibt der Clonk einen Kommentar ab!",-1,[MCMQ]],
	[6,[0,1,2,5],"Zu S.4","Dies ist die vierte Seite des Buches. Außerdem wird es hier farbig und sofort angezeigt|Eine Sensation, bei so einem langen Text",0,[MCM4],[true,RGB(255,0,0),true]],
	[3,[1,2,6],"Zurück zu S.1","Dieser Text erscheint nicht, wir springen direkt zu Seite 1!",0,[MCMC,0,0,0,0,1]],*/
}


