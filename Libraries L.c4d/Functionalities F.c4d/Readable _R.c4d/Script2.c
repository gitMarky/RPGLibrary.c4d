/*-- Grundscript für alle lesbaren Dinge --*/

/*

bestes Beispiel wären Wallscripts, Bücher, Wegweiser, etc.

insgesamt soll alles einheitllich sein

Dialoge sind auch lesbar ;)

*/

#strict 2

/* Format für lesbare Dinge: eine eigene kleine Scriptsprache

Der Dialog selbst ist ein Array. Darin enthalten sind weitere Arrays, die nach folgendem Format aufgebaut sind:

[ int iIndex, array/int iParentIndex, string szMenuOption, string szText, int/string iObjectNr, array aMenuStyle, array aTextStyle , array aConditions, array aEvents ]

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
		Stil, in welchem der Text dargestellt werden soll:
		0 - wird ausgeblendet, wenn die Voraussetzungen nicht erfüllt sind
		1 - wird rot eingeblendet, wenn die Voraussetzungen nicht erfüllt sind
	iExtra:
		siehe AddMenuItem
	XPar1:
		siehe AddMenuItem
	iOverrideIndex:
		springt zum angegebenen Dialogindex, anstatt die eigene Nachricht auszuwählen


array aTextStyle:
	[ bool fName, int dwColor, bool fInstant ]
	fName:
		soll der Name des Sprechers in dessen Farbe angezeigt werden? Standardmäßig ja
	dwColor:
		Farbe, in welcher der Rest des Texts angezeigt werden soll
	fInstant:
		soll der Text sofort angezeigt werden?
	szPortrait:
		gibt ein spezielles Portrait an

array aConditions:
	ist entweder ein Array mit nur einer spezifizierten Bedingung, oder ein Array aus mehreren Bedingungs-Arrays.
	Ein Bedingungs-Array hat verallgemeinert folgende Struktur:
	[ string szName, object pTarget, ... Data ..., bool false or true ]


array aEvents:
	[]


*/

/* Bedingungs-Arrays

Hier werden die Bedingungen und ihre Datenstruktur erklärt

Bedingungs-Arten, können wohl alle durch calls ersetzt werden
//"GetItem"
//"GetWealth"
"FindObjects"
"GetGlobal"
"GetSkill"
"GetAttribute"
"GetPerk"
"GetRandom"
"GetEffect"
"GetTime"
//"Custom"

"GetItem"
	pTarget - das Objekt, welches durchsucht werden soll
	vType - ID oder Funktion, welche das Objekt erfüllen soll (z.B. "IsWeapon")
	szOperator - Operator-String, z.B. "<", "=", ">=" usw. + Anzahl, also "== 1"
	//iAmount - Wert, mit dem die Anzahl vergleicht werden soll

"GetWealth"
	pTarget - Objekt, dessen Besitzers Geld abgefragt werden soll
	szOperator 
	//iAmount

"GetGlobal"
	string - GlobalN("string")
	szOperator


"Custom"
	string - ein Skript als String, das per eval() überprüft werden kann
*/

local aDialogue, iStartDialogue;

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
		aDialogue = GameCall(Format("MsgDialogue%s", aDlg));
}

public func AddDialouge( aOption )
{
	if( GetType(aDialogue) == C4V_Array )
		PushBack( aOption, aDialogue );
}


// Dialog aufbauen

public func StartDialogue( object pTarget )
{
	if( GetType(aDialogue) != C4V_Array ) return;

	// Begrüßung heraussuchen
	ProcessDialogue( pTarget, iStartDialogue );
}

protected func ProcessDialogue( object pTarget, int iDialogue )
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
	ProcessEvents( aEvents );

	// Farbe überschreiben
	var dwClrOvrd = this->~GetDialogueColor();
	if( dwClrOvrd )
	{
		if( GetType(aTextStyle) == C4V_Array )
		{
			aTextStyle[1]=dwClrOvrd; // dwColor
		}
		else
		{
			aTextStyle = [];
			aTextStyle[0]=true; // fName
			aTextStyle[1]=dwClrOvrd; // dwColor
		}
	}

	// Sprecher raussuchen
	var pSpeaker = this;
	if( iObjectNr == -1 ) pSpeaker = pTarget;
	if( GetType( iObjectNr ) == C4V_Int) if( iObjectNr > 0) pSpeaker = Object(iObjectNr);
	if( GetType( iObjectNr ) == C4V_String )
	{
		if( GlobalN( iObjectNr ) ) pSpeaker = GlobalN( iObjectNr );
		if( iObjectNr == "pTarget" ) pSpeaker = pTarget;
	}

	MsgBox( pTarget, szText, pSpeaker , /* szPortrait*/ 0,  false, true, aTextStyle );

	// Auswahl hinzufügen
	for( aOption in aDialogue )
	{
		var add = false;
		if( GetType(aOption[1]) == C4V_Array )
		{
			if( GetArrayItemPosition( iIndex, aOption[1] ) > -1 ) add = true;
		}
		else
		{
			if( aOption[1] == iIndex ) add = true;
			if( aOption[1] == -1 && aOption[2] ) add = true; // nur, wenn er eine Menu-Auswahl hat!
		}

		if( add ) ProcessDialogueOption( pTarget, aOption[0] );
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

	if( CheckConditions( aConditions, pTarget )) fAdd = true;

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
		var idIcon = ROCK;
		var extra, xPar;
		var iStyle = 0;
		var dwCol, iIndexOvr = iIndex;

		if( GetType(aMenuStyle) == C4V_Array )
		{
			idIcon = aMenuStyle[0];
			dwCol = aMenuStyle[1];
			iStyle = aMenuStyle[2];
			extra = aMenuStyle[3];
			xPar = aMenuStyle[4];
			if( aMenuStyle[5] != 0 ) iIndexOvr = aMenuStyle[5]-1;
		}


		// Farbe überschreiben
		var dwClrOvrd = this->~GetDialogueOptionColor();
		if( dwClrOvrd ) dwCol = dwClrOvrd;
		if( fAdd == false ) dwCol = RGB(255,0,0);
		if( dwCol ) szMenuOption=ColorizeString(szMenuOption,dwCol);

		var szCommand=Format("ProcessDialogue(Object(%d),%d)",ObjectNumber(pTarget),iIndexOvr );
		if( fAdd == false && iStyle == 1) szCommand = "eval(\"true\")";//"";

		if( fAdd || iStyle == 1 )
			MsgBoxAddOption( pTarget, idIcon, szMenuOption, szCommand, 0, extra, xPar);
	}

}

protected func CheckConditions( aConditions, object pTarget)
{
	// keine Bedingungen sind immer erfüllt
	if(!aConditions) return true;
	if( GetType(aConditions) == C4V_Array ) if(GetLength(aConditions) < 1) return true;

	var fFulfilled = true; // im Zweifel für den Angeklagten
	var aCond;

	if( GetType( aConditions ) == C4V_String )
		fFulfilled = CheckCondition( aConditions, pTarget );
	else
	{
		// erster Modus: ein Array aus mehreren Bedingungen
		//if( GetType( aConditions[0] ) == C4V_Array )
		//{
			for( aCond in aConditions )
				// die erste nicht-erfüllte Bedingung macht ihn schuldig ;)
				if(!CheckCondition( aCond, pTarget ))
				{
					fFulfilled = false;
					break;
				}
		//}
		// zweiter Modus: nur eine Bedingung wird im Array angegeben
		//else
		//{
		//	fFulfilled = CheckCondition( aConditions, pTarget ); 
		//}
	}


	return fFulfilled;
}

protected func ProcessEvents( aEvents, object pTarget)
{
	// keine Bedingungen sind immer erfüllt
	if(!aEvents) return true;
	if( GetType(aEvents) == C4V_Array ) if(GetLength(aEvents) < 1) return true;

	var aEv;

	if( GetType( aEvents ) == C4V_String )
		CheckCondition( aEvents, pTarget );
	else
	{
		for( aEv in aEvents )
			CheckCondition( aEv, pTarget );
	}

}

protected func CheckCondition( aCondition, pTarget )
{
	// keine Bedingungen sind immer erfüllt
	if(!aCondition) return true;
	if( GetType(aCondition) != C4V_Array && GetType(aCondition) != C4V_String ) return true;

	var szEval = "true"; // Dummy

	if( GetType(aCondition) == C4V_Array )
	{
			var szName = aCondition[0];
			var pCheck = this;
			if( aCondition[1] == -1 ) pCheck = pTarget;
			if( GetType( aCondition[1] ) == C4V_Int) if( aCondition[1] > 0) pCheck = Object(aCondition[1]);
			if( GetType( aCondition[1] ) == C4V_String ) if( GlobalN( aCondition[1] ) ) pCheck = GlobalN( aCondition[1] );


			//
			//---------------------------
			// bestimmte Anzahl an Items
			//
			if( szName == "GetItem" )
			{
				var szTarget = Format("Object(%d)",ObjectNumber(pCheck));
				var szAdditional = "";
				var szFunc = "";
				var vType  = aCondition[2];
				if( GetType( vType ) == C4V_C4ID ) szFunc = Format("Find_ID(%i)", vType );
				if( GetType( vType ) == C4V_String ) szFunc = Format("Find_Func(\"%s\")", vType );


				if(LocalN("pBackpack",pTarget))
				{
					szAdditional = Format("%s, Find_Container( LocalN(\"pBackpack\",%s))", szAdditional, szTarget);
				}
				if(LocalN("pSaveBuffer",pTarget))
				{
					szAdditional = Format("%s, Find_Container( LocalN(\"pSaveBuffer\",%s))", szAdditional, szTarget);
				}

				var szContainer = "";

				if( szAdditional )
					szContainer = Format("Find_Or( Find_Container(%s) %s )",szTarget, szAdditional);
				else
					szContainer = Format("Find_Container(%s)",szTarget);


				szEval = Format("ObjectCount2( %s, %s ) %s",szFunc,szContainer,aCondition[3]);
			}
			//
			//---------------------------
			// bestimmtes Vermögen
			//
			if( szName == "GetWealth" )
			{
				var szTarget = Format("Object(%d)",ObjectNumber(pCheck));

				szEval = Format("ObjectCall(%s,\"GetMoney\") %s",szTarget,aCondition[2]);
			}
	}
	else
		szEval = aCondition;

	szEval = ReplaceAll(szEval,"pTarget",Format("Object(%d)",ObjectNumber(pTarget)));
	szEval = ReplaceAll(szEval,"pSpeaker", "this");//,Format("Object(%d)",ObjectNumber(this));

	DebugLog("Evaluating \"%s\"",szEval);

	var result = eval( szEval );
	DebugLog("Evaluation: %v", result);
	return result; //eval( szEval );
}