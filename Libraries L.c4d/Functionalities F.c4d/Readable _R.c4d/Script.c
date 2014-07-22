/*-- Grundscript für alle lesbaren Dinge --*/

/*

bestes Beispiel wären Wallscripts, Bücher, Wegweiser, etc.

insgesamt soll alles einheitllich sein

Dialoge sind auch lesbar ;)

*/

#strict 2

static const gDialogue_Object_Speaker = 0;
static const gDialogue_Object_Target = -1;

/* Format für lesbare Dinge: eine eigene kleine Scriptsprache

Der Dialog selbst ist ein Array. Darin enthalten sind weitere Arrays, die nach folgendem Format aufgebaut sind:

[ int iIndex, array/int iParentIndex, string szMenuOption, string szText, int/string iObjectNr, array aMenuStyle, array aTextStyle , string/array aConditions, array aEvents ]

Dokumentation:

int iIndex:
	ein Index für den Dialog. Kinder-Knoten können so angeben, wer ihre Eltern sind. Eine indizierung über die Reihenfolge im Dialog-Array wäre auch möglich gewesen, erschwert aber das dynamische zuweisen neuer Dialoge

array/int iParentIndex:
	Wird als Option an den Dialog iParentIndex angehängt. Wird ein Array angegeben, dann wird es an alle Knoten im Array angehängt

string szMenuOption:
	falls der Dialog als Option in einem Menu erscheint, dann wird dieser Text zur Auswahl angezeigt

string szText:
	das, was man später lesen kann - in den meisten Fällen reicht die MenuAuswahl, dann einfach nur "" oder 0 angeben

int/string iObjectNr:
	die Objektnummer des Sprechers.
	0 entspricht dem Objekt, in dem der Dialog ist
	-1 entspricht dem aufrufendem Objekt, d.h. in der Regel dem Spieler
	"String" entspricht dem Objekt GlobalN("String")

array aMenuStyle:
	[ id idIcon, int dwColor, int iStyle, int iExtra, XPar1, int iOverrideIndex, string szPortrait ]
	idIcon:
		ID des Menusymbols
	dwColor:
		Farbe, in welcher der Text angegeben werden soll
	iStyle:
	    Anzahl der Bedingungen, die erfüllt werden müssen:
	    0 - wird immer ausgeblendet, wenn nicht alle Voraussetzungen erfüllt sind
	    x - wird ausgeblendet, solange die ersten x Voraussetzungen nicht erfüllt sind.
	        Wird rot eingeblendet, wenn die ersten x Voraussetzungen, aber nicht alle Voraussetzungen erfüllt sind.
	iExtra:
		siehe AddMenuItem
	XPar1:
		siehe AddMenuItem
	iOverrideIndex:
		springt zum angegebenen Dialogindex, anstatt die eigene Nachricht auszuwählen



array aTextStyle:
	[ bool fName, int dwColor, bool fInstant, string szPortrait, bool fAsMessage, id idMenuDeco, string szMenuCaption, string/array szQuestToLog, bool bNoLogChoice ]
	fName:
		soll der Name des Sprechers in dessen Farbe angezeigt werden? Standardmäßig ja
	dwColor:
		Farbe, in welcher der Rest des Texts angezeigt werden soll
	fInstant:
		soll der Text sofort angezeigt werden?
	szPortrait:
		gibt ein spezielles Portrait an
	fAsMessage:
		gibt die Nachricht als CustomMessage aus
	idMenuDeco:
		setzt eine neue MenuDecoration. Überschreibt auch die Definition aus GetDlgMsgDeco();
	szMenuCaption:
		gibt dem Menu einen Titel
	szQuestToLog:
		string oder array von Quest-Namen (Engine-Name), zu denen die Nachricht hinzugefügt werden soll
	bNoLogChoice:
		gibt an, ob die Auswahl gelogt werden soll, oder nicht. Standard: wird gelogt.

	wird anstatt eines Arrays -1 angegeben, dann wird kein neues Menu geöffnet

string/array aConditions:
	ist ein String aus einer Bedingung, oder ein Array aus mehreren Bedingungen
	die Bedingungen werden als String eingegeben. Dabei kann mit "pTarget" das ansprechende Objekt referenziert werden, und mit "pSpeaker" das angesprochene Objekt
	Beispiele:
	"GetName(pTarget)==\"Peter\"" ist true, wenn der Ansprechende "Peter" heißt
	"FindContents(ROCK,pSpeaker)" ist true, wenn der Angesprochene einen Stein hat
	Die Bedingungen werden mit eval() ausgeführt, man sollte also mit objekterstellenden "Bedingungen" vorsichtig sein, kann aber auch ein bisschen tricksen ;)


string/array aEvents:
	siehe aConditions. Die Befehle werden ausgeführt, wenn der Dialog ausgewählt wird
	da für eval kein "if" erlaubt ist, muss man eine Funktion im Objekt aufrufen oder ein wenig tricksen:
	"!FindContents(FLNT,pTarget) || RemoveObject(FindContents(FLNT,pTarget));" löscht einen Flint aus dem Objekt - falls kein Flint vorhanden wird aber das Objekt nicht gelöscht (der Parser erkennt || schon als wahr nachdem der Flint nicht vorhanden ist und führt das Löschen nicht aus)


*/

global func &DlgVar(string szVar, object pTarget, object pSpeaker)
{
	return LocalN2(Format("DlgVar%d%s",ObjectNumber(pTarget),szVar),pSpeaker);
}

global func &CreateDlgVar(string szVar, object pTarget, object pSpeaker)
{
	var szName = Format("DlgVar%d%s",ObjectNumber(pTarget),szVar);
	CreateLocalN2( szName,pSpeaker);
	return LocalN2( szName,pSpeaker);
}

//2147483647
global func StdDlgEventCancel(bool bState){ return Format("CreateDlgVar(\"Cancel\",pTarget,pSpeaker)=%v",bState);}
global func StdDlgArrayExitAlways(){ return [2147483647,-1,"Abbrechen","",0,[MCMX,0,0,0,0,-1],-1,0,"StopDialogue(pTarget)"];}
global func StdDlgArrayExitCancel(){ return [2147483647,-1,"Abbrechen","",0,[MCMX,0,0,0,0,-1],-1,"DlgVar(\"Cancel\",pTarget,pSpeaker)","StopDialogue(pTarget)"];}
global func StdDlgVar(string prefix, string variable, string operation){ return Format("%sDlgVar(\"%s\",pTarget,pSpeaker)%s",prefix,variable,operation); }
global func StdDlgVarSafeRemoveObject(string variable){ return Format("!DlgVar(\"%s\",pTarget,pSpeaker)||RemoveObject(DlgVar(\"%s\",pTarget,pSpeaker))", variable, variable);}

global func DlgObjVar(string prefix, string variable, string operation){ return Format("%sLocalN2(\"%s\",pTarget)%s",prefix,variable,operation); }

global func StdDlgIconContinue(){ return [LMM2,0,0,2,4]; }
global func StdDlgIconReturn(){ return LMM2; }

global func StdDlgQuestStage(string prefix, string quest, string operator)
{
	return Format("%sQuestStage(\"%s\",pTarget)%s",prefix, quest, operator);
}

local aDialogue, iStartDialogue, pLastSpeaker;

// das Array manipulieren

public func SetDialogue( aDlg )
{
	// Direkteingabe
	if( GetType(aDlg) == C4V_Array )
		aDialogue = aDlg;
	// oder per Szenario-Script / System.c4g
	// sollte im Szenario-Script gesetzt werden, nicht im Editor, damit das Objekt
	// den korrekten Dialog aus System.c4g erhält!
	else if( aDlg )
	{
		var story = FindObject( _STY );
		if( story )
			aDialogue = ObjectCall( story, Format("MsgDialogue%s", aDlg));
		else
			aDialogue = GameCall(Format("MsgDialogue%s", aDlg));
	}
}

public func SetStartDialogueEx( int iDlg )
{
	iStartDialogue = iDlg;
}

public func AddDialogue( aOption )
{
	if( GetType(aDialogue) == C4V_Array )
		PushBack( aOption, aDialogue );
}

public func GetUnusedDlgIndex()
{
	var aIndices = [];
	for( aOption in aDialogue )
		PushBack( aOption[0], aIndices );

	var i=0;
	for( i=0; i<= GetLength(aDialogue); i++)
		if(GetArrayItemPosition(i,aIndices)==-1)
			break;

	return i;
}


// Dialog aufbauen

public func StartDialogue( object pTarget )
{
	if( GetType(aDialogue) != C4V_Array ) return;

	// bei Filmen wird der Dialog global angezeigt
	//var bGlobal = false;
	//if(IsCutscene()) bCutscene = true;
	// Begrüßung heraussuchen
	ProcessDialogue( pTarget, iStartDialogue/*, bGlobal*/ );
}

public func StartSpecificDialogue( object pTarget, int iDialogue )
{
	if( GetType(aDialogue) != C4V_Array ) return;
	ProcessDialogue( pTarget, iDialogue );
}

public func StopDialogue( object pTarget )
{
	CloseMenu( pTarget );

	// bei globalen Nachrichten muss wieder der Cursor hergestellt werden
	/*if(IsCutscene())
		for( var i=0; i < GetPlayerCount(); i++ )
		{
			var iPlr = GetPlayerByIndex(i);
			SetCursor (iPlr, GetHiRank(i), true, true);
		}*/
}

protected func ProcessDialogue( object pTarget, int iDialogue, string szChoice/*, bool bCutscene*/ )
{
	var aOption;
	for( aOption in aDialogue )
	{
		if( aOption[0] == iDialogue ) break;
	}


	// Daten aufbauen
	var iIndex = aOption[0];
	var iParentIndex = aOption[1];
	var szMenuOption = aOption[2];
	var szText = aOption[3];
	var iObjectNr = aOption[4]; //
	var aMenuStyle = aOption[5];
	var aTextStyle = aOption[6]; //
	var aConditions = aOption[7];
	var aEvents = aOption[8];

	// Events passieren lassen
	ProcessEvents( aEvents, GetTargetString(), pTarget, GetUserString(), GetSpeaker() );

	if( GetType(aTextStyle) == C4V_Int )
		if( aTextStyle == -1)
			return;

	// Farbe überschreiben
	var dwClrOvrd = this->~GetDialogueColor();
	//if( dwClrOvrd )
	//{

	// Textstil behandeln
	var bNoLogChoice, szQuestToLog;
		if( GetType(aTextStyle) == C4V_Array )
		{
			aTextStyle[1]=dwClrOvrd; // dwColor
			bNoLogChoice = aTextStyle[8];
			szQuestToLog = aTextStyle[7];
		}
		else
		{
			aTextStyle = [];
			aTextStyle[0]=true; // fName
			aTextStyle[1]=dwClrOvrd; // dwColor
			bNoLogChoice = false;
		}
	//}



	// als Nachricht statt als Box
	var fAsMessage = false;
	
	if( GetType(aTextStyle) == C4V_Array ) fAsMessage = aTextStyle[4];

	// Sprecher raussuchen
	var pSpeaker = GetSpeaker();//this;
	if( iObjectNr == -1 ) pSpeaker = pTarget;
	if( GetType( iObjectNr ) == C4V_Int) if( iObjectNr > 0) pSpeaker = Object(iObjectNr);
	if( GetType( iObjectNr ) == C4V_String )
	{
		if( GlobalN( iObjectNr ) ) pSpeaker = GlobalN( iObjectNr );
		if( iObjectNr == "pTarget" ) pSpeaker = pTarget;
	}

	// Anschauen
	SetDirTo(pSpeaker,pLastSpeaker);
	pLastSpeaker = pSpeaker;

	// Zufallsauswahl
	if( GetType(szText) == C4V_Array )
	{
		szText = szText[Random(GetLength(szText))];
	}

	// Namen einsetzen, falls möglich
	if(szText) szText = Format(szText,GetName(pSpeaker));

	MsgBox( pTarget, szText, pSpeaker , /* szPortrait*/ 0,  fAsMessage, true, aTextStyle );

	// bei Zwischensequenzen den Cursor auf den Sprecher setzen
	/*if( bCutscene )
	{
		for( var i=0; i < GetPlayerCount(); i++ )
		{
			var iPlr = GetPlayerByIndex(i);
			SetCursor (iPlr, pTarget, true, true);
		}
	}*/

	// Auswahl hinzufügen
	if(!fAsMessage)
	for( aOption in aDialogue )
	{
		var add = false;
		if( GetType(aOption[1]) == C4V_Array )
		{

			if(GetLength(aOption[1]))
			if(GetArrayItemPosition(iDialogue,aOption[1])>-1)
			//if( GetArrayItemPosition( iIndex, aOption[1] ) > -1 )
			{
				Log("%d in %v",iIndex, aOption[1]);
				add = true;
			}
		}
		else
		{
			//if( aOption[1] == iIndex ) add = true;
			if( aOption[1] == iDialogue ) add = true;
			if( aOption[1] == -1 && aOption[2] ) add = true; // nur, wenn er eine Menu-Auswahl hat!

			//if( aOption[1] == -1 && GetType(aOption[2]) == C4V_String && aOption[2] != "") add = true;
			Log("check add %d: %d %d, %v", aOption[0], aOption[1], iDialogue, add);

		}

		//if(aOption[0] == 0) add = false;
		if(aOption[2] == "" && aOption[3] == "") {add = false; Log("Cancel add 0");}
		if( GetType(aOption[2]) != C4V_String && GetType(aOption[3]) != C4V_Array ) {add = false; Log("Cancel add 1");}
		if( GetType(aOption[3]) != C4V_String && GetType(aOption[3]) != C4V_Array ) {add = false; Log("Cancel add 2");}

		if( add )
		{
			//Log("Dialog Final: %d in %v: %s %s",iIndex, aOption[1] , aOption[2], aOption[3]);
			ProcessDialogueOption( pTarget, aOption[0] );
		}
	}


	// Eintrag ins Questlog
	if(szQuestToLog)
	{
		var bGlobal = false;
		if( pTarget == GetFilm()) bGlobal = true;

		if(!bNoLogChoice)
			AddQuestLog(szQuestToLog,[Format("<c %x>%s:</c>", GetColorDlg(pTarget), GetName(pTarget)),szChoice],pTarget,bGlobal);

		AddQuestLog(szQuestToLog,[Format("<c %x>%s:</c>", GetColorDlg(pSpeaker), GetName(pSpeaker)),szText],pTarget,bGlobal);
	}

}

protected func ProcessDialogueOption( object pTarget, iDialogue )
{
	var aOption;
	for( aOption in aDialogue )
	{
		if( aOption[0] == iDialogue ) break;
	}

	// Daten aufbauen
	var iIndex = aOption[0];
	var iParentIndex = aOption[1];
	var szMenuOption = aOption[2];
	var szText = aOption[3];
	var iObjectNr = aOption[4];
	var aMenuStyle = aOption[5];
	var aTextStyle = aOption[6];
	var aConditions = aOption[7];
	var aEvents = aOption[8];

	// vorerst gibt es noch keine Conditions
	var fAdd = false;

	//if( CheckConditions(  aConditions, GetTargetString(), pTarget, GetUserString(), GetSpeaker())) fAdd = true;
	var conditionInfo = CheckConditionsDetailed(  aConditions, GetTargetString(), pTarget, GetUserString(), GetSpeaker());

	fAdd = conditionInfo[0];
	var iFulfilled = conditionInfo[1];

	if( szMenuOption == 0 || szMenuOption == "" )
	{
		if( szText )
		{
			// hier fehlt noch die Verarbeitung von aTextStyle
			MsgBoxAddText(pTarget, szText);
		}
	}
	else
	{
		var idIcon = NONE;
		var extra, xPar;
		var iStyle = 0;
		var dwCol, iIndexOvr = iIndex;
		//var bNoLog = false;

		if( GetType(aMenuStyle) == C4V_Array )
		{
			idIcon = aMenuStyle[0];
			dwCol = aMenuStyle[1];
			iStyle = aMenuStyle[2];
			extra = aMenuStyle[3];
			xPar = aMenuStyle[4];
			if( aMenuStyle[5] != 0 ) iIndexOvr = aMenuStyle[5]-1;
		}
		if( GetType(aMenuStyle) == C4V_C4ID )
			idIcon = aMenuStyle;

		//if( GetType(aTextStyle) == C4V_Array )
		//	bNoLog = aMenuStyle[8];


		// Farbe überschreiben
		var dwClrOvrd = this->~GetDialogueOptionColor();
		if( dwClrOvrd ) dwCol = dwClrOvrd;
		if(!dwCol) dwCol = RGB(245,245,245); // etwas dunkler, damit man es vom Text unterscheiden kann
		if( fAdd == false ) dwCol = RGB(255,0,0);
		if( dwCol ) szMenuOption=ColorizeString(szMenuOption,dwCol);

		var szCommand=Format("ProcessDialogue(Object(%d),%d,\"%s\")",ObjectNumber(pTarget),iIndexOvr, szMenuOption );
		//if( bNoLog ) szCommand=Format("ProcessDialogue(Object(%d),%d,0)",ObjectNumber(pTarget),iIndexOvr );

		if( fAdd == false && iStyle == 1) szCommand = "eval(\"true\")";//"";

		if( fAdd || (iStyle > 0 && iFulfilled >= iStyle))
			MsgBoxAddOption( pTarget, idIcon, szMenuOption, szCommand, 0, extra, xPar);
	}

}

public func GetSpeaker(){ return this; }
public func GetUserString(){ return "pSpeaker"; }
public func GetTargetString(){ return "pTarget"; }

public func printTree()
{
	local tree;
	local mapping;
	local visited;
	tree = [];
	mapping = [];
	visited = [];
	var treeindex = 0;
	for(var i=0; i<GetLength(aDialogue); i++)
	{
		mapping[i]=aDialogue[i][0];
	}
	for(var i=0; i<GetLength(aDialogue); i++)
	{
		var parentid = aDialogue[i][0];
		var node = [i,[]]; // node, children

		for(var j=0; j<GetLength(aDialogue); j++)
		{
			if(j == i) continue;

			var parentIndex = aDialogue[j][1];

			if( GetType(parentIndex) == C4V_Array )
			{
				for(var parent in parentIndex)
				{
					if(parent == parentid || parent == -1)
					{
						PushBack(GetArrayItemPosition(aDialogue[j][0],mapping),node[1]);
					}
				}
			}
			else if(parentIndex == parentid || parentIndex == -1)
			{
				if(parentid == 1)
					Log("XXX Adding quest: %d",j);
				PushBack(GetArrayItemPosition(aDialogue[j][0],mapping),node[1]); // add j as child of node;
			}
		}

		PushBack(node,tree);
	}

	//for(var node in tree)
	//{
	//	printChildren(node[0],0);
	//}
		printChildren(tree[0][0],0,[-1]);

}

public func printChildren(int parentid, int depth, array children)
{
	var node;
	for(var index = 0; index < GetLength(tree); index++)
	{
		node = tree[index];
		if(node[0] == parentid) break;
	}

	visited[parentid]++;

	var message = "";
	for(var i = 0; i<depth; i++)
	{
		var space = "   ";
		if(i==depth-1)
		{
			space = " * ";
		}
		message = Format("%s%s", message,space);
	}

	var option, answer;
	answer = aDialogue[node[0]][3];
	option = aDialogue[node[0]][2];

	if(GetType(answer) != C4V_String) answer = "(null)";
	if(GetType(option) != C4V_String) option = "(null)";

	message = Format("%s[%s]:%s",message,option,answer);

	Log(message);

	if(visited[parentid] > 1) return;

	for(var j = 0; j<GetLength(node[1]); j++)
	{
			var child = node[1][j];
//		var cancel = false;
//		if(GetArrayItemPosition(parentid,children)>= 0 )
//		{
//		 if(depth>50) cancel = true;
//		}
//		if(cancel) continue;
		printChildren(child,depth+1,node[1]);
	}
}

public func printEntries()
{
	for(var field in aDialogue)
	{
		Log("%d, parents %v: %s  - %s", field[0],field[1],field[2],field[3]);
	}
}
