/*-- Buch --*/

#strict 2
#include LF_R // ist lesbar, oh Wunder :)

local aQuestIndex;
local aQuestOpen;
local aQuestDone;
local aQuestFail;
local aQuestDesc;
local aQuestRead;

local aDialogue;

public func QuestColorText(){ return RGB(255,255,0);}
public func QuestColorNew(){ return RGB(255,0,0);}
public func QuestColorOld(){ return RGB(0,255,0);}
public func QuestColorTitle(){ return RGB(0,0,255); }


public func AddQuest(szName)
{
	Sound("Ding",1,0,0,GetOwner()+1);
	PushFront(szName,aQuestOpen);
	PushBack(szName,aQuestIndex);
	PushBack(0,aQuestDesc);
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
	aQuestOpen = aQuestDone = aQuestFail = aQuestDesc = aQuestIndex = aQuestRead = [];

	SetOwner(2);
	AddQuest("MainQuest");
	AddQuest("TestQuest1");
	AddQuest("TestQuest2");
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

	var iIndex = 1;

	for( var quest in aQuestOpen)
	{
		var log = GetQuestLog( quest, pPlayer );
		if( GetType(log) != C4V_Array ) continue;

		var szName = log[0][1];
		var iLines, bNew, iSigns, iIndexAdd;

		// Dialogoption zusammenbauen
		var qIndex = GetArrayItemPosition(quest,aQuestIndex);
		var iOldLen = aQuestDesc[qIndex];
		if(GetLength(log) != iOldLen) aQuestRead[qIndex]=0;
		aQuestDesc[qIndex]=GetLength(log);


		// der Eintrag mit dem Namen auf der Hauptseite
		var dwClr = QuestColorOld();
		if(!aQuestRead[qIndex]) dwClr = QuestColorNew();

		var aMenuStyle = [ NONE, dwClr ];

		var aTextStyle = [ false, 0, true, 0, false, -1 ];
		var iParent = iIndex;
		var iOldParent;
		var iPage = 0;

		var aDlg = [ iIndex, 0, szName, szName, 0, aMenuStyle, aTextStyle , "true", Format("aQuestRead[%d]=1",qIndex) ];
		PushBack(aDlg,aDialogue);

		iIndexAdd = 1;

		//////////////////////////////////////////////////////////////////////////////////
		// außerdem eine Questbeschreibung, die je 2 max. Einträge als Kind von iParent einnimmt
		// und dann einen "-> zurückblättern"-Eintrag hat, sowie einen "<- zurück"

		// Questbeschreibung zusammenbauen
		var szDesc = "", szLastspeaker = "";
		for( var i=GetLength(log)-1; i >= 0; i-- )
		{
			Log("i %d / %d",i,GetLength(log)-1);
			var szSpeaker = log[i][0], szText = log[i][1];
			var szNewDesc = "";

			/*if( szLastspeaker == szSpeaker)
			{
				szNewDesc = Format("%s",ColorizeString(szText,QuestColorText()));
			}
			else
			{*/
				if( szDesc == "")
					szNewDesc = Format("%s|%s",szSpeaker,ColorizeString(szText,QuestColorText()));
				else
				{
					if(iIndexAdd == 2) Log("newDescAdd = %s",szDesc);
					szNewDesc = Format("%s|%s",szSpeaker,ColorizeString(szText,QuestColorText()));
				}
			//}
			szLastspeaker = szSpeaker;

			iSigns = GetLength(szNewDesc)+GetLength(szDesc);
			iLines = GetLetterCount("|",szNewDesc) + GetLetterCount("|",szDesc) + iSigns/30;

			// zu viel? einen leer-Eintrag machen und den iIndexAdd hochzählen
			if(( iSigns > 240 || iLines > 8 ) && !( GetLetterCount("|",szDesc) > 8))
			{
				PushBack([ iIndex+iIndexAdd, iParent, "", szDesc ],aDialogue);
				iIndexAdd++;
				szDesc=szNewDesc;
				//Log("newdesclength %d",GetLength(szNewDesc));
				//Log("newdesccont: %s",szNewDesc);

				// an dieser Stelle soll er eine neue Seite anfangen
				if(iIndexAdd >= 3)
				{
					if( szNewDesc != "(null)|(null)" )
						PushBack([iIndex+iIndexAdd, iParent, "Zurück blättern", szName, 0, [LMM2,0,0,2,4], aTextStyle , "true", 0 ],aDialogue); // verweis auf die nächste Seite

					// man kann auch eine Seite vorblättern!
					if( iPage )
						PushBack([iIndex+iIndexAdd+1, iParent, "Vorwärts blättern", szName, 0, [LMM2,0,0,2,0,iOldParent], aTextStyle , "true", 0 ],aDialogue); // verweis auf die nächste Seite

					iOldParent = iIndex+1;
					iParent = iIndex+iIndexAdd; // daran sollen die neuen Dialoge angefügt werden
					iIndex = iParent;
					if(iPage) iIndex++; // weil wir das Vorwärtsblättern drin haben
					iIndexAdd = 1;
					++iPage; // wir haben eine neue Seite erreicht
					szLastspeaker = ""; // hier hat noch niemand gesprochen

					//Log("new page, newdesc = %s", szNewDesc );
				}
			}
			// passt!
			else
			{
				if( szDesc == "" )
					szDesc = szNewDesc;
				else
					szDesc = Format("%s|%s",szDesc,szNewDesc);

				szDesc = ReplaceAll(szDesc,"(null)|(null)","");

				if( i==0 && iIndexAdd >= 1)
				{
					PushBack([ iIndex+iIndexAdd, iParent, "", szDesc ],aDialogue);
					iIndex += iIndexAdd+1;
					iIndexAdd = 1;
				}
			}
		}

		// der Rest
		/*
		var dwClr = QuestColorOld();
		if(bNew) dwClr = QuestColorNew();

		var aMenuStyle = [ NONE, dwClr ];

		var aTextStyle = [ false, 0, true, 0, false, -1, name ];
		var iParent = 0;

		var aDlg = [ iIndex, iParent, szName, szDesc, 0, aMenuStyle, aTextStyle , "true", 0 ];
		PushBack(aDlg,aDialogue);
*/
		iIndex++;


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


