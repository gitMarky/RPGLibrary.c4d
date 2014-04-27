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


public func AddQuest(szName, bool bNoSound)
{
	if(!bNoSound) Sound("Ding",1,0,0,GetOwner()+1);
	PushFront(szName,aQuestOpen);
	PushBack(szName,aQuestIndex);
	PushBack(0,aQuestDesc);
}

public func UpdateQuest(szName, bool bNoSound)
{
	if(!bNoSound) Sound("Ding",1,0,0,GetOwner()+1);
	DropArrayItem(szName,aQuestOpen);
	PushFront(szName,aQuestOpen);
}

public func FinishQuest(szName)
{
	Sound("Ding",1,0,0,GetOwner()+1);
	PushFront(szName,aQuestDone);
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
	FinishQuest("TestQuest2");
	//Activate(GetHiRank(2));
}

public func Activate(object pPlayer)
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

	var iIndex = 1;
	var iMenuPage = 1;
	var iEntriesPerPage = 1;
	var iMenuPages = (GetLength(aQuestOpen)+iEntriesPerPage-1)/iEntriesPerPage;
	var iMenuPageParent = 0;
	var iMenuPageParentOld = 0;
	var iMenuPageParentOld2 = 0;

	PushBack([0,-1,0,Format("Offene Quests (%d/%d)",iMenuPage,iMenuPages),0,SCRG,[false,0,0,0,false,-1,"Journal"]],aDialogue);

	//for( var quest in aQuestOpen)
	/*for( var qIndex = 0; qIndex < GetLength(aQuestOpen); qIndex++)
	{
		var quest = aQuestOpen[qIndex];
		var log = GetQuestLog( quest, pPlayer );
		if( GetType(log) != C4V_Array ) continue;


		var szName = log[0][1];
		var iLines, bNew, iSigns, iIndexAdd;

		// Dialogoption zusammenbauen
		//var qIndex = GetArrayItemPosition(quest,aQuestIndex);
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

		var aDlg = [ iIndex, iMenuPageParent, szName, szName, 0, aMenuStyle, aTextStyle , "true", Format("aQuestRead[%d]=1",qIndex) ];
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

				if( szDesc == "")
					szNewDesc = Format("%s|%s",szSpeaker,ColorizeString(szText,QuestColorText()));
				else
				{
					if(iIndexAdd == 2) Log("newDescAdd = %s",szDesc);
					szNewDesc = Format("%s|%s",szSpeaker,ColorizeString(szText,QuestColorText()));
				}

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


		// neue Seite hinzufügen! Hiermit sollte man sich besser nicht befassen :D
		// es handelt sich um eine ziemliche Index-Schieberei, hauptsächlich für die "Zurück zu Seite"-Funktion
		if(!(qIndex%iEntriesPerPage) && (qIndex||iEntriesPerPage==1))
		{
			if( iMenuPage != iMenuPages)
			{
				iIndex ++;
				// Eintrag Seite X
				PushBack([iIndex,iMenuPageParent,"Weiter",Format("Offene Quests (%d/%d)",iMenuPage+1,iMenuPages),0,[LMM2,0,0,2,4],[false,0,0,0,false,-1,"Journal"]],aDialogue);
				iMenuPageParentOld2 = iMenuPageParentOld;
				iMenuPageParentOld = iMenuPageParent;
				// das nur, wenn wir nicht auf der letzten Seite sind!
				iMenuPageParent = iIndex;
			}
			else
			{
				// das muss immer angepasst werden
				iMenuPageParentOld2 = iMenuPageParentOld;
				iMenuPageParentOld = iMenuPageParent;
			}

			if(qIndex)
			{
				iIndex++;
				//              angehängt auf Seite X                                                                    führt auf Seite X-1
				PushBack([iIndex,iMenuPageParentOld, Format("Zurück zu Seite %d",iMenuPage-1),"Zurück", 0, [LMM2,0,0,2,0,iMenuPageParentOld2+1], [false,0,0,0,false,-1,"Journal"] ],aDialogue);
			}

			iMenuPage++;
			iIndex++;
		}

		iIndex++;



	}*/

	var aPagesIndex = [0];
	BuildQuestIndex( aQuestOpen, pPlayer, iIndex, aPagesIndex, iMenuPage, iMenuPages, iMenuPageParentOld2, iMenuPageParentOld, iMenuPageParent);

	// die abgeschlossenen Quests
	iMenuPage = 1;
	iMenuPages = (GetLength(aQuestDone)+iEntriesPerPage-1)/iEntriesPerPage;
	iMenuPageParent = iIndex;
	iMenuPageParentOld = iIndex;
	iMenuPageParentOld2 = iIndex;

	PushBack([iIndex,aPagesIndex,"Abgeschlossene Quests",Format("Abgeschlossene Quests (%d/%d)",iMenuPage,iMenuPages),0,SCRG,[false,0,0,0,false,-1,"Journal"]],aDialogue);
	iIndex++;

	var aDummy = [];
	BuildQuestIndex( aQuestDone, pPlayer, iIndex, aPagesIndex, iMenuPage, iMenuPages, iMenuPageParentOld2, iMenuPageParentOld, iMenuPageParent);

	// die gescheiterten Quests
	iMenuPage = 1;
	iMenuPages = (GetLength(aQuestFail)+iEntriesPerPage-1)/iEntriesPerPage;
	iMenuPageParent = iIndex;
	iMenuPageParentOld = iIndex;
	iMenuPageParentOld2 = iIndex;

	PushBack([iIndex,aPagesIndex,"Gescheiterte Quests",Format("Gescheiterte Quests (%d/%d)",iMenuPage,iMenuPages),0,SCRG,[false,0,0,0,false,-1,"Journal"]],aDialogue);
	iIndex++;

	var aDummy2 = [];
	BuildQuestIndex( aQuestFail, pPlayer, iIndex, aPagesIndex, iMenuPage, iMenuPages, iMenuPageParentOld2, iMenuPageParentOld, iMenuPageParent);

	// an alle ein "Hauptmenu" anhängen
	PushBack([iIndex,-1,"Hauptmenu","", 0, [LMM2,0,0,4,0,0], [ false, 0, true, 0, false, -1 ] , "true", 0 ],aDialogue);
}


protected func BuildQuestIndex(array aQuestOpen, object pPlayer, int &iIndex, array &aPagesIndex, int iMenuPage, int iMenuPages, int &iMenuPageParentOld2, int &iMenuPageParentOld, int &iMenuPageParent )
{
		for( var qIndex = 0; qIndex < GetLength(aQuestOpen); qIndex++)
		{
			var quest = aQuestOpen[qIndex];
			var log = GetQuestLog( quest, pPlayer );
			if( GetType(log) != C4V_Array ) continue;


			var szName = log[0][1];
			var iLines, bNew, iSigns, iIndexAdd;

			// Dialogoption zusammenbauen
			//var qIndex = GetArrayItemPosition(quest,aQuestIndex);
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
			var iEntriesPerPage = 1;

			var aDlg = [ iIndex, iMenuPageParent, szName, szName, 0, aMenuStyle, aTextStyle , "true", Format("aQuestRead[%d]=1",qIndex) ];
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

					if( szDesc == "")
						szNewDesc = Format("%s|%s",szSpeaker,ColorizeString(szText,QuestColorText()));
					else
					{
						if(iIndexAdd == 2) Log("newDescAdd = %s",szDesc);
						szNewDesc = Format("%s|%s",szSpeaker,ColorizeString(szText,QuestColorText()));
					}

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


			// neue Seite hinzufügen! Hiermit sollte man sich besser nicht befassen :D
			// es handelt sich um eine ziemliche Index-Schieberei, hauptsächlich für die "Zurück zu Seite"-Funktion
			if(!(qIndex%iEntriesPerPage) && (qIndex||iEntriesPerPage==1))
			{
				if( iMenuPage != iMenuPages)
				{
					iIndex ++;
					// Eintrag Seite X
					PushBack([iIndex,iMenuPageParent,"Weiter",Format("Offene Quests (%d/%d)",iMenuPage+1,iMenuPages),0,[LMM2,0,0,2,4],[false,0,0,0,false,-1,"Journal"]],aDialogue);
					iMenuPageParentOld2 = iMenuPageParentOld;
					iMenuPageParentOld = iMenuPageParent;
					// das nur, wenn wir nicht auf der letzten Seite sind!
					iMenuPageParent = iIndex;
					PushBack(iIndex,aPagesIndex);
				}
				else
				{
					// das muss immer angepasst werden
					iMenuPageParentOld2 = iMenuPageParentOld;
					iMenuPageParentOld = iMenuPageParent;
				}

				if(qIndex)
				{
					iIndex++;
					//              angehängt auf Seite X                                                                    führt auf Seite X-1
					PushBack([iIndex,iMenuPageParentOld, Format("Zurück zu Seite %d",iMenuPage-1),"Zurück", 0, [LMM2,0,0,2,0,iMenuPageParentOld2+1], [false,0,0,0,false,-1,"Journal"] ],aDialogue);
				}

				iMenuPage++;
				iIndex++;
			}

			iIndex++;

		}
}
