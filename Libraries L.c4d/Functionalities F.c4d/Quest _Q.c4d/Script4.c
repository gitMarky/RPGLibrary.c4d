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

global func SetQuestStage( string szName, int iStage, object pPlayer, bool bGlobal, string szFunc, vP1, vP2 )
{
	var quest = FindQuest( szName );
	if(!quest) return false;

	quest->SetStage( iStage, pPlayer, bGlobal, szFunc, vP1, vP2 );
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
	[ int iStage, aConditions, aEvents, int iTimer ]

	int iStage:
		wenn der Counter aProgress auf dieser Zahl steht, dann sollen aConditions überprüft werden. So kann eine Quest-Phase mehrere Ergebnisse haben
		für iStage == 0 wird die Bedingung immer überprüft, solange die Quest aktiv ist
		iStage == 1 ist die allererste Stufe einer Quest
		iStage == -1 wird gesetzt, wenn die Quest abgeschlossen ist
	aConditions:
		ein string/array mit den bekannten eval-Scripten, wird alle iTimer Frames überprüft
	aEvents:
		ein string/array mit den bekannten eval-Scripten, wenn die Bedingungen erfüllt sind, dann werden alle diese Events ausgeführt
		Wechsel zu einer anderen Quest-Phase müssen explizit als Befehl in das Event eingebaut werden
		allerdings kann auch ein fehlender Phasenwechselbefehl sinnvoll sein, z.B. weil durch die Quest eine magische Todeszone gilt
		die Quest-Phase kann außerdem durch Dialoge oder andere Objekte gewechselt werden
	int iTimer:
		alle iTimer Frames werden die Bedingungen überprüft. Wird mit FramcCounter()%iTimer geprüft!

	die Bedingungen/Phasen/Events sollten auch für eine KI oder Tagesabläufe verwendbar sein, also braucht man dafür kein eigenes Objekt :)


Platzhalter für eval-Scripte:
- pPlayer überprüft alle Spieler, für welche die Quest gilt (nur bei Belohnungen verwenden!!!)
- pActivePlayer überprüft nur den gerade aktiven Spieler
*/
protected func Initialize()
{
	aPlayers = [];
	aProgress = [];
	aQuest = [];
	bQuestActive = false;

	//AddEffect("MonitorQuest",this,50,50,this);
	AddEffect("MonitorQuest",this,50,150,this);
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

public func SetStage( int iStage, object pPlayer, bool bGlobal, string szFunc, vP1, vP2 )
{
	for( var i = 0; i < GetLength( aPlayers ); i++ )
	{
		var pPlr = aPlayers[i];

		if( bGlobal || pPlr == pPlayer )
		{
			var bSet = false;
			if( szFunc )
			{
				if( ObjectCall( pPlr, szFunc, vP1, vP2 ) ) bSet = true;
			}
			else
				bSet = true;

			if( bSet ) aProgress[i]=iStage;
		}
	}
}

public func GiveReward( object pPlayer, aReward, aRewardGlobal )
{
	if( GetArrayItemPosition( pPlayer, aPlayers ) >= 0)
		ProcessEvents( aReward, pPlayer, true );

	for( var obj in aPlayers )
		ProcessEvents( aRewardGlobal, obj, true);
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
	//StartScriptProfiler(LF_Q);

	var players = GetHumanPlayersEx();
	for( pPlr in players )
	{
		var iIndex = GetArrayItemPosition( pPlr, aPlayers );

			// wenn der Spieler die Quest bereits hat
			if( iIndex > -1 )
			{
				//StartScriptProfiler(LF_Q);
				var bFinished = true;

				for( var stg in aQuestStages )
				{
					var iStage = stg[0];

					// allgemeine Phase oder spezielle Phase behandeln
					if( !iStage || iStage == aProgress[iIndex] )
					{
						var aConditions = stg[1];
						var aEvents = stg[2];
						var iTimer = stg[3];

						if(!iTimer) iTimer = 36;

						if( !(iEffectTime%iTimer))
							if( CheckConditions( aConditions, pPlr, false ) )
								ProcessEvents( aEvents, pPlr );
					}

					if( aProgress[iIndex] >= 0 ) bFinished = false;
				}
				//StopScriptProfiler(LF_Q);

				if( bFinished ) return -1; // fertig :)
			}
			// wenn eine Bedingung zu erfüllen ist
			else if( aQuest[0])
			{
				//StartScriptProfiler(LF_Q);

				var aConditions = aQuest[1];
				var fCheck = false;

						/*if(GetType(aConditions)==C4V_Array)
						{
							var timer = aConditions[0];
							// kein Timer angegeben, dann einfach immer die Bedingungen prüfen
							if( GetType(timer) == C4V_String )
							{
								fCheck = true;
							}
							// sonst alle timer Frames die Bedingung prüfen
							else if( !(iEffectTime%timer))
							{
								fCheck = true;
							}
						}
						else
						{
								if( !(iEffectTime%36)) fCheck = true;
						}
						if(fCheck)*/ CheckConditions( aConditions, pPlr, true);
					//StopScriptProfiler(LF_Q);
			}

	}

	//StopScriptProfiler(LF_Q);
}


protected func CheckConditions( aConditions, object pActivePlayer, bool bActivate)
{
	var fFulfilled = true; // im Zweifel für den Angeklagten
	var aCond;
	var fCheck = true;

	// keine Bedingungen sind immer erfüllt
	if(!aConditions) fCheck = false;
	if( GetType(aConditions) == C4V_Array ) if(GetLength(aConditions) < 1) fCheck = false;

	if( fCheck )
	{
			//var aClonks = GetHumanPlayersEx();

			//var len = GetLength( aClonks );
			//for(var i = 0; i < len; i++)
			//{
					if( GetType( aConditions ) == C4V_String )
					{
						if(!CheckCondition( aConditions, pActivePlayer/*, aClonks[i]*/ ))
						{
							fFulfilled = false;
							//break;
						}
					}
					else
					{
							for( aCond in aConditions )
								// die erste nicht-erfüllte Bedingung macht ihn schuldig ;)
								if(!CheckCondition( aCond, pActivePlayer/*, aClonks[i]*/ ))
								{
									fFulfilled = false;
									break;
								}
					}
			//}
	}

	if( fFulfilled && bActivate && pActivePlayer != this )
	{
		ChangeEffect ("MonitorQuest", this, 0, "MonitorQuest", 1); 
		AddPlayer( pActivePlayer );
	}

	return fFulfilled;
}

protected func ProcessEvents( aEvents, object pActivePlayer, bool bPlayer )
{
	// keine Bedingungen sind immer erfüllt
	if(!aEvents) return true;
	if( GetType(aEvents) == C4V_Array ) if(GetLength(aEvents) < 1) return true;

	var aEv;
	//var aClonks = aPlayers;

	//var len = GetLength( aClonks );
	//for(var i = 0; i < len; i++)
	//{
			if( GetType( aEvents ) == C4V_String )
				CheckCondition( aEvents, pActivePlayer, bPlayer );
			else
			{
				for( aEv in aEvents )
					CheckCondition( aEv, pActivePlayer, bPlayer );
			}
	//}
}

protected func CheckCondition( aCondition, object pActivePlayer, bool bPlayer )
{
	// keine Bedingungen sind immer erfüllt
	if(!aCondition) return true;
	if( GetType(aCondition) != C4V_Array && GetType(aCondition) != C4V_String ) return true;

	var szEval = aCondition;

	if(bPlayer) szEval = ReplaceAll(szEval,"pPlayer",Format("Object(%d)",ObjectNumber(pActivePlayer)));
	//szEval = ReplaceAll(szEval,"pPlayer",Format("Object(%d)",ObjectNumber(pActivePlayer)));
	szEval = ReplaceAll(szEval,"pActivePlayer",Format("Object(%d)",ObjectNumber(pActivePlayer)));
	szEval = ReplaceAll(szEval,"pQuest", "this");

	DebugLog("Evaluating Quest Condition \"%s\", %d",szEval, FrameCounter());

	var result = eval( szEval );
	DebugLogEx("Evaluation: %v", result);

	return result;
}