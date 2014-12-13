/*--
Basic script for quest functionality.

@title Quest
@author Marky
@version 0.2.0
--*/

#strict 2

// TODO: translate
// TODO: quest builder object

local quest_data, active_stage, active_characters, active_log;
local quest_stages;
local quest_identifier;
local quest_is_active;

static const gQuest_Effect_Monitor_Name = "MonitorQuest";
static const gQuest_Effect_Monitor_Priority = 50;
static const gQuest_Effect_Monitor_DefaultInterval = 150;

static const gQuest_Stage_Silent = 0;
static const gQuest_Stage_Default = 1;

static const gQuestStageFinished = -1;
static const gQuestStageFailed = -2;

/**
 Creates a quest monitor for the specified quest. The quest monitor tracks
 the status of the quest for each player. It allows setting the status globally
 and locally, so that the following situations become possible:@br
 - every player can finish or fail the quest individually@br
 - players can finish the quest cooperatively: One finishes - all finish@br
 - players can finish the quest competitively: One finishes - others fail@br
 - a mix: One player causes an event which globally changes the quest, then
   the progress is handled individually again
 @par identifier This is a unique string that the quest will be referred to.
 @par description This is a description of the quest. See TODO.
 @par stages These are the quest stages. See TODO.
 @version 0.1.0
 */
global func CreateQuest(string identifier, array description, array stages)
{
	var obj;
	if (!(obj = FindQuest(identifier)))
	{
		var obj = CreateObject(ID_QuestHandler, 0, 0, -1);
		obj->SetPosition(0, 0);
	}

	if (GetType(description) == C4V_Array)
		obj->~SetQuest(identifier, description, stages);
	else
		obj->~SetQuest(identifier, identifier); // try it with scenario script
}

/**
 Finds the quest monitor of the quest.
 @par identifier The unique quest identifier.
 @return The object, if it could be found
 @note The quest has to be created with CreateQuest() first
 @version 0.1.0
 */
global func FindQuest(string identifier)
{
	return FindObject2(Find_ID(ID_QuestHandler), Find_Func("CompareQuestName", identifier));
}

/**
 Activates a quest for an object.
 @par identifier The quest identifier.
 @par crew The crew member that this quest will be activated for.
 @par check_interval [since v0.2.0] The quest checks its conditions every {@c check_interval} frames.
 @return bool false, if there the quest was not found
 @note The quest has to be created with CreateQuest() first. Initially
 the quest is inactive, which means that it does not check for conditions
 of the quest stages. When activating a quest, the quest checks its conditions
 every {@c check_interval} frames.
 @version 0.1.0
 */
global func ActivateQuest(string identifier, object target, int check_interval)
{
	var quest = FindQuest(identifier);
	if (!quest) return false;

	ChangeEffect(gQuest_Effect_Monitor_Name, quest, 0, gQuest_Effect_Monitor_Name, Max(1, check_interval));
	quest->AddPlayer(target);
	return true;
}

global func FinishQuest(string identifier, object crew, bool set_status_globally, string function_call, vP1, vP2)
{
//	var quest = FindQuest(identifier);
//	if (!quest)
//		return false;
//	
//	quest->SetStage(gQuestStageFinished, crew, set_status_globally, function_call, vP1, vP2);
	return SetQuestStage(identifier, gQuestStageFinished, crew, set_status_globally, function_call, vP1, vP2);
}

global func FailQuest(string identifier, object crew, bool set_status_globally, string function_call, vP1, vP2)
{
//	var quest = FindQuest(identifier);
//	if (!quest)
//		return false;
//	
//	quest->SetStage(gQuestStageFailed, crew, set_status_globally, function_call, vP1, vP2);
	return SetQuestStage(identifier, gQuestStageFailed, crew, set_status_globally, function_call, vP1, vP2);
}

/**
 @par identifier The quest identifier.
 @par stage_index The new quest stage.
 @par crew The crew member that this quest will be activated for.
 @par set_status_globally [optional] If true, then the status will be set for all active players
 @par function_call [optional] This function call is executed by the object in question.
 If it returns false, then the quest stage is not updated.
 @par vP1 [optional] First parameter for {@c function_call}
 @par vP2 [optional] Second parameter for {@c function_call}
 @example
 {@code
 // advances all knights, that have "Quest1", to quest stage 3
 SetQuestStage("Quest1", 3, 0, true, "IsKnight");
 // Advances the "Quest2" to stage 5 for player 1, if his selected has "Quest2" and he has a shield.
 SetQuestStage("Quest2", 5, GetCursor(0), false, "HasShield");
 }
 @note
 The quest stage will not be set if the quest was already finished for this player
 @version 0.1.0
 */
global func SetQuestStage(string identifier, int stage_index, object crew, bool set_status_globally, string function_call, vP1, vP2)
{
	var quest = FindQuest(identifier);
	if (!quest)
		return false;
	
	quest->SetStage(stage_index, crew, set_status_globally, function_call, vP1, vP2);
}

global func GetQuestStage(string identifier, object crew, bool set_status_globally, string function_call, vP1, vP2)
{
	var quest = FindQuest(identifier);
	if (!quest)
		return false;
	
	return quest->GetStage(crew, set_status_globally, function_call, vP1, vP2);
}

global func AddQuestLog(string identifier, array log_message, object crew, bool set_status_globally, string function_call, vP1, vP2)
{
	var quest = FindQuest(identifier);
	if (!quest)
		return false;
	
	quest->AddLog(log_message, crew, set_status_globally, function_call, vP1, vP2);
}

global func GetQuestLog(string identifier, object crew)
{
	var quest = FindQuest(identifier);
	if (!quest)
		return "";
	
	return quest->GetLog(crew);
}

global func & GetQuestVar(string identifier, string var_name)
{
	var quest = FindQuest(identifier);
	if (!quest)
		return "";
	
	return LocalN2("szVarName", quest);
}

global func SetQuestVar(string identifier, string var_name, value)
{
	var quest = FindQuest(identifier);
	if (!quest)
		return "";
	
	if (!LocalN2(var_name, quest))
		CreateLocalN2(var_name, quest, 0);
	LocalN2(var_name, quest) = value;
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
	// active characters and active stage are basically a map
	active_characters = [];
	active_stage = [];
	active_log = [];
	
	// data
	quest_data = [];
	quest_is_active = false;
	
	AddEffect(gQuest_Effect_Monitor_Name, this, gQuest_Effect_Monitor_Priority, gQuest_Effect_Monitor_DefaultInterval, this);
}

public func CompareQuestName(string identifier)
{
	return quest_identifier == identifier;
}

public func AddPlayer(object crew)
{
	if (!crew) return;

	var index = GetArrayItemPosition(crew, active_characters);
	if (index < 0)
	{		
		AddPlayerToArray(crew, gQuest_Stage_Default);
	}
	else
	{
		active_stage[index] = gQuest_Stage_Default; // activate the quest
	}
	// set quest to active
	quest_is_active = true;
	
	var log = crew->~GetQuestLogEx();
	if (log)
		log->~AddQuest(quest_identifier);
	
	DebugLog("Added Player %s/%s to Quest %s", GetName(crew), GetPlayerName(GetOwner(crew)), quest_identifier);
}

public func AddPlayerSilent(object crew)
{
	if (!crew) return;
	if (GetArrayItemPosition(crew, active_characters) > -1) return false; // player already has the quest
	
	AddPlayerToArray(crew, gQuest_Stage_Silent);
	
	DebugLog("Silent Added Player %s/%s to Quest %s", GetName(crew), GetPlayerName(GetOwner(crew)), quest_identifier);
}

private func AddPlayerToArray(object crew, int stage)
{
		// Save players and initialize default status
		PushBack(crew, active_characters);
		PushBack(stage, active_stage);
		PushBack([[0, quest_data[0]]], active_log);
}

// das Array manipulieren

public func SetQuest(string identifier, description, stages)
{
	quest_identifier = identifier;
	var story = GetStory();
	
	// direct input
	if (GetType(description) == C4V_Array)
	{
		quest_data = description;
	}
	else if (description)
	{
		if (story)
			quest_data = ObjectCall(story, Format("DscQuest%s", description), false);
		else
			quest_data = GameCall(Format("DscQuest%s", description), false);
	}
	
	// direct input
	if (GetType(stages) == C4V_Array)
	{
		quest_stages = stages;
	}
	else if (description)
	{
		if (story)
			quest_stages = ObjectCall(story, Format("DscQuest%s", description), true);
		else
			quest_stages = GameCall(Format("DscQuest%s", description), true);
	}
	
	ScheduleCall(this, "DoMonitorQuest", 1, 0, this, 0, 0);
	ScheduleCall(this, "DoMonitorQuest", 2, 0, this, 0, 0);
}

public func SetStage(int stage_index, object crew, bool set_status_globally, string function_call, vP1, vP2)
{
	for (var i = 0; i < GetLength(active_characters); i++)
	{
		var current_crew = active_characters[i];
		
		if (set_status_globally || current_crew == crew)
		{
			var set_stage_for_current_crew = false;
			
			if (function_call)
			{
				set_stage_for_current_crew = ObjectCall(current_crew, function_call, vP1, vP2);
			}
			else
			{
				set_stage_for_current_crew = true;
			}
			
			// already finished?
			// if (GetStage(current_crew) == gQuestStageFinished) this does the same loop again -> ineffective!!
			if (active_stage[i] == gQuestStageFinished)
				set_stage_for_current_crew = false;
			// TODO: what about failed quests?????
			
			if (set_stage_for_current_crew)
			{
				active_stage[i] = stage_index;
				
				if (stage_index < 0) // finished or failed? Notify the quest log
				{
					var log = current_crew->~GetQuestLogEx();
					if (log)
					{
						if (stage_index == gQuestStageFinished)
							log->~FinishQuest(quest_identifier);
						else
							log->~FailQuest(quest_identifier);
					}
				}
			}
		}
	}
}

public func GetStage(object crew, bool get_status_globally, string function_call, vP1, vP2)
{
    // TODO
    // not sure why the get function should have a filter :/
	for (var i = 0; i < GetLength(active_characters); i++)
	{
		var current_crew = active_characters[i];
		
		if (get_status_globally || current_crew == crew)
		{
			var return_stage = false;
			if (function_call)
			{
				if (ObjectCall(current_crew, function_call, vP1, vP2))
					return_stage = true;
			}
			else
				return_stage = true;
			
			if (return_stage)
			{
				return active_stage[i];
			}
		}
	}
	
	return 0;
}

public func AddLog(array log_message, object crew, bool set_status_globally, string function_call, vP1, vP2)
{
	// logs the quest without adding it to the players quest log
	// if it is not active for the player yet.
	// this has one very important rease: you should be able to get quest infos, without knowing
	// that there will be a quest!
	// once the quest is active it can be looked up in the quest log
	if (GetArrayItemPosition(crew, active_characters) < 0) AddPlayerSilent(crew);
	
	// write to the log
	for (var i = 0; i < GetLength(active_characters); i++)
	{
		var current_crew = active_characters[i];
		
		if (set_status_globally || current_crew == crew)
		{
			var write_to_log = false;
			if (function_call)
			{
				if (ObjectCall(current_crew, function_call, vP1, vP2))
					write_to_log = true;
			}
			else
				write_to_log = true;
			
			if (write_to_log)
				if (GetType(active_log) == C4V_Array)
				{
					PushBack(log_message, active_log[i]);
					
					if (GetStage(current_crew) > 0) // update the log only if the quest is not finished yet and if the quest is active
					{
						var log = current_crew->~GetQuestLogEx();
						if (log)
							log->~UpdateQuest(quest_identifier);
					}
				}
		}
	}
}

public func GetLog(object crew)
{
	var index = GetArrayItemPosition(crew, active_characters);
	if (index < 0)
		return "";
	
	return active_log[index];
}

public func GiveReward(object crew, events_reward_local, events_reward_global)
{
	if (GetArrayItemPosition(crew, active_characters) >= 0)
		ProcessEvents(events_reward_local, crew, true);
	
	for (var obj in active_characters) 
		ProcessEvents(events_reward_global, obj, true);
}

public func AddPhase(stage)
{
	if (GetType(quest_stages) == C4V_Array)
		PushBack(stage, quest_stages);
}

/* This should be removed
public func GetUnusedDlgIndex()
{
	var aIndices = [];
	for (aOption in aQuest) 
		PushBack(aOption[0], aIndices);
	
	var i = 0;
	for (i = 0; i <= GetLength(aQuest); i++)
		if (GetArrayItemPosition(i, aIndices) == -1)
			break;
	
	return i;
}
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Quest-Mechaniken

protected func GetHumanPlayersEx()
{
	var aClonks = [];
	
	for (var i = 0; i < GetPlayerCount(C4PT_User); i++)
		PushBack(GetHiRank(GetPlayerByIndex(i, C4PT_User)), aClonks);
	
	return aClonks;
}

protected func FxMonitorQuestTimer(object target, int effect_nr, int time)
{
	DoMonitorQuest(target, effect_nr, time);
}

protected func DoMonitorQuest(object target, int effect_nr, int time)
{
	var players = GetHumanPlayersEx();
	
	if (GetType(players) != C4V_Array) return;

	for (crew in players) 
	{
		var crew_index = GetArrayItemPosition(crew, active_characters);
		
		//Log("Check Player %d", iIndex);
		// if the player already has the quest
		if (crew_index > -1)
		{
			
			var is_finished = true;
			
			for (var stage in quest_stages) 
			{
				var stage_index = 0;
				if (GetType(stage) == C4V_Array) stage_index = stage[0];
				
				//Log("CheckStage %d",iStage);
				
				// handle general or specific stage
				if (!stage_index || stage_index == active_stage[crew_index])
				{
					// newly added
					if (active_stage[crew_index] >= 0)
					{
						is_finished = false;
						
						//Log("QuestStage %d",iStage);
						var stage_conditions = stage[1];
						var stage_events = stage[2];
						var stage_timer = stage[3];
						
						if (!stage_timer) stage_timer = 36;
						
						if ((time % stage_timer) == 0 && CheckConditions(stage_conditions, crew, false))
						{
								ProcessEvents(stage_events, crew);
						}
					}
				}
				
				//Log("Not finished, I hope");
				//if (aProgress[iIndex] >= 0) bFinished = false;
			}
			
			if (is_finished)
			{
				DebugLog("Quest finished");
				return -1;
			} // done :)
		}
		else if (quest_data[1])
		{
			CheckConditions(quest_data[2], crew, true);
		}
	}

}


protected func CheckConditions(conditions, object active_crew, bool should_activate)
{
	var is_fulfilled = true; // im Zweifel für den Angeklagten
	var should_check = true;
	
	// no conditions are always fulfilled, no need to check anything here
	if (!conditions) should_check = false;
	if (GetType(conditions) == C4V_Array && GetLength(conditions) < 1) should_check = false;
	
	if (should_check)
	{
		if (GetType(conditions) == C4V_String)
		{
			if (!CheckCondition(conditions, active_crew))
			{
				is_fulfilled = false;
			}
		}
		else
		{
			for (var condition in conditions) 
			{
				if (!CheckCondition(condition, active_crew))
				{
					is_fulfilled = false;
					break;
				}
			}
		}
	}
	
	if (is_fulfilled && should_activate && active_crew != this)
	{
		ChangeEffect(gQuest_Effect_Monitor_Name, this, 0, gQuest_Effect_Monitor_Name, 1);
		AddPlayer(active_crew);
	}
	
	return is_fulfilled;
}

protected func ProcessEvents(events, object active_crew, bool should_activate)
{
	// no events can alway be processed
	if (!events) return true;
	if (GetType(events) == C4V_Array && GetLength(events) < 1) return true;
	
	if (GetType(events) == C4V_String)
	{
		CheckCondition(events, active_crew, should_activate);
	}
	else
	{
		for (var event in events) 
		{
			CheckCondition(event, active_crew, should_activate);
		}
	}
}

protected func CheckCondition(condition, object active_crew, bool should_activate)
{
	// no condition is always fulfilled
	if (!condition) return true;
	//if (GetType(condition) != C4V_Array && GetType(condition) != C4V_String) return true;
	if (GetType(condition) != C4V_Array && GetType(condition) != C4V_String) return true;
	
	if (GetType(condition) == C4V_Array)
	{
		ErrorLog("Condition %v type is invalid - expected C4V_String, but got C4V_Array", condition);
		return false;
	}
	
	var script = condition;
	
	if (should_activate)
	{
		script = ReplaceAll(script, "pPlayer", Format("Object(%d)", ObjectNumber(active_crew)));
	}
	script = ReplaceAll(script, "pActivePlayer", Format("Object(%d)", ObjectNumber(active_crew)));
	script = ReplaceAll(script, "pQuest", "this");
	
	DebugLog("Evaluating Quest Condition \"%s\", %d", script, FrameCounter());
	
	var result = eval(script);
	DebugLog("Evaluation: %v", result);
	
	return result;
}
