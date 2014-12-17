/*-- 
Provides functionality for cyclic behavior per day.

@title Day night cycle
@author Marky
@version 0.1.0
 --*/

#strict 2

local cycle_target_object;
local cycle_definition_array;

static const gDayNightCycle_ARRAYPOS_Index 		= 0;
static const gDayNightCycle_ARRAYPOS_Conditions = 1;
static const gDayNightCycle_ARRAYPOS_Events 	= 2;

/** Gets called by the time object when the hour variable changes.
 *
 * @par hours_new the current time
 * @par hours_old the time before the change.
 */
public func UpdateTime(int hours_new, int hours_old)
{
	if (GetType(cycle_definition_array) != C4V_Array)
		return;
	
	EvaluateCyclePhase(-1);
	
	for (var i = 0; i < g_TIME_Hours_per_Day; i++)
	{
		var hour_to_check = (hours_old + i) % g_TIME_Hours_per_Day;
		
		// do something
		EvaluateCyclePhase(hour_to_check);
		
		// stop doing something
		if (hour_to_check == hours_new)
			break;
	}
}

/**
 Goes through the cycle array and decides which behavior to execute.
 @par ihourToCheck The current hour
 @ignore Useful for understanding this object only.
 */
protected func EvaluateCyclePhase(int hour_to_check)
{
	DebugLog("Evaluating cycle phase for hour %d", hour_to_check);
	
	for (var j = 0; j < GetLength(cycle_definition_array); j++)
	{
		// the wrong hour?
		if (cycle_definition_array[j][gDayNightCycle_ARRAYPOS_Index] != hour_to_check)
			continue;
		
		var conditions_fulfilled = CheckConditions
		(
			cycle_definition_array[j][gDayNightCycle_ARRAYPOS_Conditions],
			GetTargetString(),
			cycle_target_object,
			GetUserString(),
			this
		);
		
		if (conditions_fulfilled)
		{
			DebugLog("* Conditions fulfilled, processing events");
			ProcessEvents
			(
				cycle_definition_array[j][gDayNightCycle_ARRAYPOS_Events],
				GetTargetString(),
				cycle_target_object,
				GetUserString(),
				this
			);
		}
		else
		{
			DebugLog("* Conditions not fulfilled, do nothing");
		}
	}
}


/**
 Assigns a new day-night-cycle-array.

 @par cycle this can either be@br
 - an array in the right format,@br
 - a string. A function "DayNightCycle" + "cycle" has to exist@br
 in the story object or as a global function
 */
public func SetDayNightCycle(cycle)
{
	// direct input?
	if (GetType(cycle) == C4V_Array)
		cycle_definition_array = cycle;
	else if (cycle)
	{
		var story = GetStory();
		if (story)
			cycle_definition_array = ObjectCall(story, Format("DayNightCycle%s", cycle));
		else
			cycle_definition_array = GameCall(Format("DayNightCycle%s", cycle));
	}
}

/**
 Adds an option to the end of the day-night-cycle-array.

 @par aOption the new option.
 */
public func AddDayNightCyclePhase(option)
{
	if (GetType(cycle_definition_array) == C4V_Array)
		PushBack(option, cycle_definition_array);
}

/**
 The script system replaces this string with a reference to the day-night-cycle object.
 @return String that is used in the script language: "pCycle".
 */
public func GetUserString()
{
	return "pCycle";
}

/**
 The day-night-cycle is assigned to this object.
 @return object The object that is controlled by the day-night-cycle.
 */
public func GetTarget()
{
	return cycle_target_object;
}

/**
 The script system replaces this string with a reference to the target object.
 @return String that is used in the script language: "pTarget".
 */
public func GetTargetString()
{
	return "pTarget";
}
