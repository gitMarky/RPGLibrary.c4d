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

// TODO: english documentation

/*
Format für Tagesabläufe: eine eigene kleine Scriptsprache

Der Zyklus selbst ist ein Array. Darin enthalten sind weitere Arrays, die nach folgendem Format aufgebaut sind:

[ int iHour, string/array aConditions, array aEvents ]

Dokumentation:

int iHour:
	bestimmt die Stunde, ab welcher die Bedingungen zutreffen.

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
 * Goes through the cycle array and decides which behavior to execute.
 * @par ihourToCheck The current hour
 * @ignore Useful for understanding this object only.
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
		var story = FindObject(ID_Helper_Story);
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
public func AddDayNightCyclePhase(aOption)
{
	if (GetType(cycle_definition_array) == C4V_Array)
		PushBack(aOption, cycle_definition_array);
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
