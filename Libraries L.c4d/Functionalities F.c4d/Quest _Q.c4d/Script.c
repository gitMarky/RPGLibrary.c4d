/*--
Basic script for quest functionality.

@title Quest
@author Marky
@version 0.1.0
--*/

#strict 2

// TODO: translate

local aQuest,aProgress,aPlayers,aQuestLog;
local aQuestStages;
local szQuestName;
local bQuestActive;

static const gQuestStageFinished = -1;
static const gQuestStageFailed = -2;


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

	ChangeEffect ("MonitorQuest", quest, 0, "MonitorQuest", 1);
	quest->AddPlayer( pTarget );
}

global func FinishQuest( string szName, object pPlayer, bool bGlobal, string szFunc, vP1, vP2 )
{
	var quest = FindQuest( szName );
	if(!quest) return false;

	quest->SetStage( gQuestStageFinished, pPlayer, bGlobal, szFunc, vP1, vP2 );
}

global func FailQuest( string szName, object pPlayer, bool bGlobal, string szFunc, vP1, vP2 )
{
	var quest = FindQuest( szName );
	if(!quest) return false;

	quest->SetStage( gQuestStageFailed, pPlayer, bGlobal, szFunc, vP1, vP2 );
}

global func SetQuestStage( string szName, int iStage, object pPlayer, bool bGlobal, string szFunc, vP1, vP2 )
{
	var quest = FindQuest( szName );
	if(!quest) return false;

	quest->SetStage( iStage, pPlayer, bGlobal, szFunc, vP1, vP2 );
}

global func GetQuestStage( string szName, object pPlayer, bool bGlobal, string szFunc, vP1, vP2 )
{
	var quest = FindQuest( szName );
	if(!quest) return false;

	return quest->GetStage( pPlayer, bGlobal, szFunc, vP1, vP2 );
}

global func AddQuestLog( string szName, array aMessage, object pPlayer, bool bGlobal, string szFunc, vP1, vP2 )
{
	var quest = FindQuest( szName );
	if(!quest) return false;

	quest->AddLog( aMessage, pPlayer, bGlobal, szFunc, vP1, vP2 );
}

global func GetQuestLog( string szName, object pPlayer )
{
	var quest = FindQuest( szName );
	if(!quest) return "";

	return quest->GetLog( pPlayer );
}

global func GetQuestVar( string szName, string szVarName )
{
	var quest = FindQuest( szName );
	if(!quest) return "";

	return LocalN2("szVarName",quest);
}

global func SetQuestVar( string szName, string szVarName, vValue )
{
	var quest = FindQuest( szName );
	if(!quest) return "";

	if(!LocalN2(szVarName,quest)) CreateLocalN2(szVarName,quest,0);
	LocalN2(szVarName,quest) = vValue;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// lokale Funktionen

/* Beschreibung der Datenstruktur

[ array aQuestStage1, ... array aQuestStageN  ]

array aQuest
	[ szQuestName int iStartType , string/array vStartCondition ]

	szQuestName
		Name, mit dem die Quest im Questlog referenziert wird
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
	aQuestLog = [];

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
	//if( GetArrayItemPosition( pTarget, aPlayers ) > -1 ) return false; // der Spieler hat die Quest schon
	//AddPlayerSilent( pTarget );
	var index = GetArrayItemPosition( pTarget, aPlayers );
	if( index < 0 ) 
	{
		// spätestens jetzt ist die Quest aktiv!
		//bQuestActive = true;
		
		// Spieler speichern und Status auf 0 setzen
		PushBack( pTarget, aPlayers );
		PushBack( 1, aProgress );
		PushBack( [ [0,aQuest[0]]], aQuestLog);
	}
	else
	{
		aProgress[index] = 1; // wir haben die Quest aktiviert!
	}
	// spätestens jetzt ist die Quest aktiv!
	bQuestActive = true;

	var log = pTarget->~GetQuestLogEx();
	if(log) log->~AddQuest(szQuestName);

	DebugLog("Added Player %s/%s to Quest %s", GetName(pTarget),GetPlayerName(GetOwner(pTarget)),szQuestName);
	//ScheduleCall(this,"DoMonitorQuest",1,0,this,0,0);
}

public func AddPlayerSilent( object pTarget )
{
	if(!pTarget) return;
	if( GetArrayItemPosition( pTarget, aPlayers ) > -1 ) return false; // der Spieler hat die Quest schon

	// Spieler speichern und Status auf 0 setzen
	PushBack( pTarget, aPlayers );
	PushBack( 0, aProgress );
	PushBack( [ [0,aQuest[0]]], aQuestLog);

	DebugLog("Silent Added Player %s/%s to Quest %s", GetName(pTarget),GetPlayerName(GetOwner(pTarget)),szQuestName);
}

// das Array manipulieren

public func SetQuest( sNm, aQst, aStg )
{
	szQuestName = sNm;
	var story = FindObject(_STY);

	// Direkteingabe
	if( GetType(aQst) == C4V_Array )
		aQuest = aQst;
	// oder per Szenario-Script / System.c4g
	// sollte im Szenario-Script gesetzt werden, nicht im Editor, damit das Objekt
	// den korrekten Dialog aus System.c4g erhält!
	else if( aQst )
	{
		if( story )
			aQuest = ObjectCall( story, Format("DscQuest%s", aQst), false);
		else
			aQuest = GameCall(Format("DscQuest%s", aQst), false);
	}

	// Direkteingabe
	if( GetType(aStg) == C4V_Array )
		aQuestStages = aStg;
	// oder per Szenario-Script / System.c4g
	// sollte im Szenario-Script gesetzt werden, nicht im Editor, damit das Objekt
	// den korrekten Dialog aus System.c4g erhält!
	else if( aQst )
	{
		if( story )
			aQuestStages = ObjectCall( story, Format("DscQuest%s", aQst), true);
		else
		aQuestStages = GameCall(Format("DscQuest%s", aQst), true);
	}
	//ChangeEffect (string szEffectName, object pTarget, int iIndex, string szNewEffectName, int iNewTimer)

	ScheduleCall(this,"DoMonitorQuest",1,0,this,0,0);
	ScheduleCall(this,"DoMonitorQuest",2,0,this,0,0);
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

			// schon beendet!
			if( GetStage(pPlr) == gQuestStageFinished ) bSet = false;

			if( bSet )
			{
				aProgress[i]=iStage;
				if( iStage < 0)
				{
					var log = pPlr->~GetQuestLogEx();
					if(log)
					{
						if( iStage == gQuestStageFinished )
							log->~FinishQuest(szQuestName);
						else
							log->~FailQuest(szQuestName);
					}
				}
			}
		}
	}
}

public func GetStage( object pPlayer, bool bGlobal, string szFunc, vP1, vP2 )
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

			if( bSet )
			{
				return aProgress[i];
			}
		}
	}

	return 0;
}

public func AddLog( array aMessage, object pPlayer, bool bGlobal, string szFunc, vP1, vP2 )
{
	// loggt, ohne die Quest ins Buch aufzunehmen!
	if( GetArrayItemPosition( pPlayer, aPlayers ) < 0 ) AddPlayerSilent( pPlayer );
	// jetzt den Leuten das Zeug ins Log schreiben
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

			if( bSet ) if(GetType( aQuestLog ) == C4V_Array)
			{
					PushBack(aMessage,aQuestLog[i]);

					if( GetStage( pPlr ) > 0  )
					{
						var log = pPlr->~GetQuestLogEx();
						if(log) log->~UpdateQuest(szQuestName);
					}
			}
		}
	}
}

public func GetLog( object pPlayer)
{
	var index = GetArrayItemPosition( pPlayer, aPlayers);
	if(index < 0) return "";

	return aQuestLog[index];
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
	DoMonitorQuest( pTarget, iEffectNumber, iEffectTime);
}

protected func DoMonitorQuest(object pTarget, int iEffectNumber, int iEffectTime)
{
	var players = GetHumanPlayersEx();

	if( GetType(players) == C4V_Array )
	for( pPlr in players )
	{
		var iIndex = GetArrayItemPosition( pPlr, aPlayers );

			//Log("Check Player %d", iIndex);
			// wenn der Spieler die Quest bereits hat
			if( iIndex > -1 )
			{

				var bFinished = true;

				for( var stg in aQuestStages )
				{
					var iStage = 0;
					if( GetType(stg) == C4V_Array ) iStage = stg[0];

					//Log("CheckStage %d",iStage);

					// allgemeine Phase oder spezielle Phase behandeln
					if( !iStage || iStage == aProgress[iIndex] )
					if( aProgress[iIndex] >= 0 ) // neu eingefügt
					{
						 bFinished = false; // neu eingefügt
						//Log("QuestStage %d",iStage);
						var aConditions = stg[1];
						var aEvents = stg[2];
						var iTimer = stg[3];

						if(!iTimer) iTimer = 36;

						if( !(iEffectTime%iTimer))
							if( CheckConditions( aConditions, pPlr, false ) )
								ProcessEvents( aEvents, pPlr );
					}

					//Log("Not finished, I hope");
					//if( aProgress[iIndex] >= 0 ) bFinished = false;
				}

				if( bFinished ){ DebugLog("Quest finished"); return -1;} // fertig :)
			}
			// wenn eine Bedingung zu erfüllen ist
			else if( aQuest[1])
			{
				CheckConditions( aQuest[2], pPlr, true);
			}

	}
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
					if( GetType( aConditions ) == C4V_String )
					{
						if(!CheckCondition( aConditions, pActivePlayer/*, aClonks[i]*/ ))
						{
							fFulfilled = false;

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

			if( GetType( aEvents ) == C4V_String )
				CheckCondition( aEvents, pActivePlayer, bPlayer );
			else
			{
				for( aEv in aEvents )
					CheckCondition( aEv, pActivePlayer, bPlayer );
			}
}

protected func CheckCondition( aCondition, object pActivePlayer, bool bPlayer )
{
	// keine Bedingungen sind immer erfüllt
	if(!aCondition) return true;
	if( GetType(aCondition) != C4V_Array && GetType(aCondition) != C4V_String ) return true;

	var szEval = aCondition;

	if(bPlayer) szEval = ReplaceAll(szEval,"pPlayer",Format("Object(%d)",ObjectNumber(pActivePlayer)));
	szEval = ReplaceAll(szEval,"pActivePlayer",Format("Object(%d)",ObjectNumber(pActivePlayer)));
	szEval = ReplaceAll(szEval,"pQuest", "this");

	DebugLog("Evaluating Quest Condition \"%s\", %d",szEval, FrameCounter());

	var result = eval( szEval );
	DebugLog("Evaluation: %v", result);

	return result;
}
