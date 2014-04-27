/*-- Grundscript für alle Quests --*/

#strict 2

local aQuest,aProgress,aPlayers;
local aQuestStages;
local szQuestName;
local bQuestActive;


global func CreateQuest( string szName, array aDesc, array aStages )
{
	var obj;
	if( !(obj = FindQuest( szName ) ))
	{
		var obj = CreateObject( ID_QuestHandler, 0, 0, -1 );
		obj->SetPosition(0,0);	
	}

	if( GetType(aDesc) == C4V_Array )
		obj->~SetQuest( szName, aDesc, aStages );
	else
		obj->~SetQuest( szName, szName ); // wir versuchen es mit dem Szenario-Script
}

global func FindQuest( string szName )
{
	return FindObject2( Find_ID( ID_QuestHandler ), Find_Func( "CompareQuestName", szName ) );
}

global func ActivateQuest( string szName, object pTarget )
{
	var quest = FindQuest( szName );
	if(!quest) return false;

	quest->AddPlayer( pTarget );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// lokale Funktionen

/* Beschreibung der Datenstruktur

[ array aQuestStage1, ... array aQuestStageN  ]

array aQuest
	[ int iStartType , string/array vStartCondition ]

	//szQuestName:
	//	Name, mit dem die Quest im Spiel referenziert wird
	iStartType:
		0 - inaktiv, muss manuell aktiviert werden
		1 - startet automatisch, Bedingung wird in vStartCondition definiert
	vStartCondition:
		Array, enthält beliebig viele eval-Scripte und einen Integer, der das Timer-Intervall angibt. Wird kein Integer spezifiziert, dann gilt das Intervall 1

array aQuestStage
	[  ]


Platzhalter für eval-Scripte:
- pPlayer überprüft alle Spieler, für welche die Quest gilt
- pActivePlayer überprüft nur den gerade aktiven Spieler
*/
protected func Initialize()
{
	aPlayers = [];
	aProgress = [];
	aQuest = [];
	bQuestActive = false;

	AddEffect("MonitorQuest",this,50,1,this);
}

public func CompareQuestName( string szName )
{
	return szQuestName == szName;
}

public func AddPlayer( object pTarget )
{
	if(!pTarget) return;
	if( GetArrayItemPosition( pTarget, aPlayers ) > 0 ) return false; // der Spieler hat die Quest schon


	// spätestens jetzt ist die Quest aktiv!
	bQuestActive = true;
	
	// Spieler speichern und Status auf 0 setzen
	//aPlayers[GetLength(aPlayers)] = pTarget;
	//aProgress[GetLength(aPlayers)] = 0;
	PushBack( pTarget, aPlayers );
	PushBack( 1, aProgress );

	DebugLog("Added Player %s/%s to Quest %s", GetName(pTarget),GetPlayerName(GetOwner(pTarget)),szQuestName);
}

// das Array manipulieren

public func SetQuest( sNm, aQst, aStg )
{
	szQuestName = sNm;

	// Direkteingabe
	if( GetType(aQst) == C4V_Array )
		aQuest = aQst;
	// oder per Szenario-Script / System.c4g
	// sollte im Szenario-Script gesetzt werden, nicht im Editor, damit das Objekt
	// den korrekten Dialog aus System.c4g erhält!
	else if( aQst )
		aQuest = GameCall(Format("DscQuest%s", aQst), false);

	// Direkteingabe
	if( GetType(aStg) == C4V_Array )
		aQuestStages = aStg;
	// oder per Szenario-Script / System.c4g
	// sollte im Szenario-Script gesetzt werden, nicht im Editor, damit das Objekt
	// den korrekten Dialog aus System.c4g erhält!
	else if( aQst )
		aQuestStages = GameCall(Format("DscQuest%s", aQst), true);
}

public func AddPhase( aOption )
{
	if( GetType(aQuestStages) == C4V_Array )
		PushBack( aOption, aQuestStages );
}

public func GetUnusedDlgIndex()
{
	var aIndices = [];
	for( aOption in aQuest )
		PushBack( aOption[0], aIndices );

	var i=0;
	for( i=0; i<= GetLength(aQuest); i++)
		if(GetArrayItemPosition(i,aIndices)==-1)
			break;

	return i;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Quest-Mechaniken

protected func GetHumanPlayersEx()
{
	var aClonks = [];

	for(var i = 0; i < GetPlayerCount(C4PT_User); i++)
		PushBack( GetHiRank(GetPlayerByIndex(i, C4PT_User)), aClonks );

	return aClonks;
}

protected func FxMonitorQuestTimer(object pTarget, int iEffectNumber, int iEffectTime)
{
	var players = GetHumanPlayersEx();
	for( pPlr in players )
	{
		var iIndex = GetArrayItemPosition( pPlr, aPlayers );

			// wenn der Spieler die Quest bereits hat
			if( iIndex > -1 )
			{
			}
			// wenn eine Bedingung zu erfüllen ist
			else if( aQuest[0])
			{
				var aConditions = aQuest[1];

				//for( var pPlr in players )
				//{
					// bereits aktivierte Spieler überspringen
					//if( pPlr == this ) continue;
					//if( GetArrayItemPosition( pPlr, pPlayers ) > -1 ) continue;

						if(GetType(aConditions)==C4V_Array)
						{
							var timer = aConditions[0];
							// kein Timer angegeben, dann einfach immer die Bedingungen prüfen
							if( GetType(timer) == C4V_String )
							{
								//for( var pPlr in players )
								//{
								//	if( pPlr == this ) continue;
									CheckConditions( aConditions, pPlr, true);
								//}
							}
							// sonst alle timer Frames die Bedingung prüfen
							else if( !(iEffectTime%timer))
							{
								//for( var pPlr in players )
								//{
								//	if( pPlr == this ) continue;
									CheckConditions( aConditions, pPlr, true);
								//}
							}
						}
						else
						{
							//for( var pPlr in players )
							//{
								//if( pPlr == this ) continue;
								CheckConditions( aConditions, pPlr, true);
							//}
						}
				//}
			}
	}
}


protected func CheckConditions( aConditions, object pActivePlayer, bool bActivate)
{
	// bereits aktivierte Quests muss man nicht mehr prüfen
	//if( bActivate && GetArrayItemPosition( pActivePlayer, aPlayers ) != -1 ) return;

	var fFulfilled = true; // im Zweifel für den Angeklagten
	var aCond;
	var fCheck = true;

	// keine Bedingungen sind immer erfüllt
	if(!aConditions) fCheck = false;
	if( GetType(aConditions) == C4V_Array ) if(GetLength(aConditions) < 1) fCheck = false;

	if( fCheck )
	{
			var aClonks = GetHumanPlayersEx();

			var len = GetLength( aClonks );
			for(var i = 0; i < len; i++)
			{
					if( GetType( aConditions ) == C4V_String )
					{
						if(!CheckCondition( aConditions, pActivePlayer, aClonks[i] ))
						{
							fFulfilled = false;
							break;
						}
					}
					else
					{
							for( aCond in aConditions )
								// die erste nicht-erfüllte Bedingung macht ihn schuldig ;)
								if(!CheckCondition( aCond, pActivePlayer, aClonks[i] ))
								{
									fFulfilled = false;
									break;
								}
					}
			}
	}

	if( fFulfilled && bActivate && pActivePlayer != this )
	{
		AddPlayer( pActivePlayer );
	}

	return fFulfilled;
}

protected func CheckCondition( aCondition, object pActivePlayer, object pPlayer )
{
	// keine Bedingungen sind immer erfüllt
	if(!aCondition) return true;
	if( GetType(aCondition) != C4V_Array && GetType(aCondition) != C4V_String ) return true;

	var szEval = aCondition;

	szEval = ReplaceAll(szEval,"pPlayer",Format("Object(%d)",ObjectNumber(pPlayer)));
	szEval = ReplaceAll(szEval,"pActivePlayer",Format("Object(%d)",ObjectNumber(pActivePlayer)));
	szEval = ReplaceAll(szEval,"pQuest", "this");

	DebugLogEx("Evaluating Quest Condition \"%s\", %d",szEval, FrameCounter());

	var result = eval( szEval );
	DebugLogEx("Evaluation: %v", result);
	return result;
}