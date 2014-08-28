/*-- Day night cycle  --*/

#strict 2

local aDayNightCycle;

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

local pTarget;

public func UpdateTime(int hoursNew, int hoursOld)
{
	if (GetType(aDayNightCycle) != C4V_Array)
		return;
	
	var hoursPerDay = 24;
	
	EvaluateCyclePhase(-1);
	
	for (var i = 0; i < hoursPerDay; i++)
	{
		var hourToCheck = (hoursOld + i) % hoursPerDay;
		
		// do something
		
		EvaluateCyclePhase(hourToCheck);
		
		// stop doing something
		
		// end
		if (hourToCheck == hoursNew)
			break;
	}
}

protected func EvaluateCyclePhase(int hourToCheck)
{
	for (var j = 0; j < GetLength(aDayNightCycle); j++)
	{
		if (aDayNightCycle[j][0] != hourToCheck)
			continue;
		
		var conditionsFulfilled = CheckConditions(aDayNightCycle[j][1], GetTargetString(), pTarget, GetUserString(), this);
		
		if (conditionsFulfilled)
		{
			DebugLog("* Conditions fulfilled, processing events");
			ProcessEvents(aDayNightCycle[j][2], GetTargetString(), pTarget, GetUserString(), this);
		}
		else
		{
			DebugLog("* Conditions not fulfilled, do nothing");
		}
	}
}

// das Array manipulieren

public func SetDayNightCycle(aCycle)
{
	// Direkteingabe
	if (GetType(aCycle) == C4V_Array)
		aDayNightCycle = aCycle;
	else if (aCycle)
	{
		var story = FindObject(_STY);
		if (story)
			aDayNightCycle = ObjectCall(story, Format("DayNightCycle%s", aCycle));
		else
			aDayNightCycle = GameCall(Format("DayNightCycle%s", aCycle));
	}
}

public func AddDayNightCyclePhase(aOption)
{
	if (GetType(aDayNightCycle) == C4V_Array)
		PushBack(aOption, aDayNightCycle);
}

public func GetUnusedDNCIndex()
{
	var aIndices = [];
	for (aOption in aDayNightCycle) 
		PushBack(aOption[0], aIndices);
	
	var i = 0;
	for (i = 0; i <= GetLength(aDayNightCycle); i++)
		if (GetArrayItemPosition(i, aIndices) == -1)
			break;
	
	return i;
}


public func GetUserString()
{
	return "pCycle";
}
public func GetTarget()
{
	return pTarget;
}
public func GetTargetString()
{
	return "pTarget";
}

