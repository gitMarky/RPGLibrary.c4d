/*--
 Helper object that can be used to build quests. The advantage in using this object is,
 that you do not have to remember the order of entries in the quest stage array. It saves a lot
 of time.
 @title Quest builder
 @author Marky
 @version 0.2.0
--*/

#strict 2

local quest_stage;
local quest_data;
local quest_conditions;
local quest_events;
local is_stage_definition;

public func Initialize()
{
	SetPosition();
	quest_stage = [];
	quest_data = [];
	quest_conditions = [];
	quest_events = [];
	is_stage_definition = false;
}

/**
 Initializes a default dialogue option.
 @par index The dialogue index. This has to be a unique integer for the specific dialogue.
 @par parent The option will be displayed as a child of this dialogue index. Passing a single
        int makes this option available only under one option. Passing an array of int makes
        the option available under all the specified options. Parent -1 means, that the option will
        be displayed under every option in the dialogue, except itself.
 @par option The dialogue option is usually a menu item in a menu. This is the caption of the menu item.
 @return object Returns the helper object, such that the dialogue option can be further modified.
 */
global func QstStage(int index)
{
	var builder = CreateObject(ID_Helper_QuestBuilder);
	
	builder->Stage(index);

	return builder;
}

global func QstInfo(string title)
{
	var builder = CreateObject(ID_Helper_QuestBuilder);
	
	builder->Title(title);
	
	return builder;
}

public func Create()
{
	if (is_stage_definition)
	{
		quest_stage[gQuest_Stages_Condition] = quest_conditions;
		quest_stage[gQuest_Stages_Events] = quest_events;
	}
	else
	{
	}

	RemoveObject(this);
	
	if (is_stage_definition)
	{
		return quest_stage;
	}
	else
	{
		return quest_data;
	}
}

protected func Stage(int index)
{
	is_stage_definition = true;
	quest_stage[gQuest_Stages_Index] = index;
}

protected func Title(string title)
{
	quest_data[gQuest_Data_Title] = title;
}

public func Conditions(conditions)
{
	if (GetType(conditions) == C4V_String)
	{
		PushBack(conditions, quest_conditions);
	}
	else if (GetType(conditions) == C4V_Array)
	{
		for (var condition in conditions)
			PushBack(condition, quest_conditions);
	}
	
	return this;
}

public func Events(events)
{
	if (GetType(events) == C4V_String)
	{
		PushBack(events, quest_events);
	}
	else if (GetType(events) == C4V_Array)
	{
		for (var event in events)
			PushBack(event, quest_events);
	}

	return this;
}

public func Timer(int interval)
{
	if (!is_stage_definition)
	{
		ErrorLog("Timer() can be used only in quest stage definitions");
		return 0; // let the engine fail :)
	}
	
	quest_stage[gQuest_Stages_Timer] = interval;
	
	return this;
}

public func StartsOnConditions()
{
	if (is_stage_definition)
	{
		ErrorLog("StartsOnConditions() can be used only in quest info definitions");
		return 0; // let the engine fail :)
	}
	
	quest_data[gQuest_Data_Activation] = true;
	
	return this;
}

