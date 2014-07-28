/*-- Behaviours and Missions --*/

#strict 2

/*
Steuert das Verhalten von KI-Objekten. Es gibt 3 Stufen von KI-Handhabung, sortiert nach aufsteigender Priorität:
- Behaviour: Eine einfache Verhaltensweise ohne besondere Ziele.
- Mission: Eine Komplexe Aufgabe. Besteht aus einzelnen Tasks. Kann an andere KI-Objekte abgegeben werden.
- Task: Eine einfache Aufgabe. Kann an andere KI-Objekte abgegeben werden.
 */

static const gBehaviour_EffectVar_Data = 0;
static const gBehaviour_EffectVar_Priority = 1;

static const gBehaviour_Wander_Index_xLeft = 0;
static const gBehaviour_Wander_Index_xRight = 1;
static const gBehaviour_Wander_Index_minPause = 2;
static const gBehaviour_Wander_Index_maxPause = 3;

static const gBehaviour_Wander = "Wander";

static const gBehaviour_Priority_All = -1;
static const gBehaviour_Priority_Idle = 10;
static const gBehaviour_Priority_Mission = 20;
static const gBehaviour_Priority_Task = 30;
static const gBehaviour_Priority_Override = 50;


/*******************************************************************************************************************
 *
 * Manipulate Behaviours
 *
 *******************************************************************************************************************/


global func AddBehaviour(object pTarget, string name, int interval, array data, int priority)
{
	var behaviour = Format("IntBehaviour%s",name);
	//RemoveAllEffects(pTarget, behaviour);
	RemoveBehaviour(pTarget, name, priority); // replace current behaviour in this priority

	return AddEffect (behaviour, pTarget, 100, interval, 0, 0, data, priority);
}

global func RemoveBehaviour(object pTarget, string name, int priority, int operator)
{
	var behaviours = GetBehaviours(pTarget, priority, operator);
	//RemoveAllEffects(pTarget, behaviour);

	Log("--> Remove Behaviour %s %v - array: %v", name, pTarget, behaviours);

	var iEffect;
	var i = GetEffectCount(0, pTarget);
	while (i--)
	if (iEffect = GetEffect(Format("IntBehaviour%s",name), pTarget, i))
	{
		Log("--> Remove Behaviour - Check effect %d", iEffect);
		if (GetArrayItemPosition(iEffect, behaviours) > -1)
		      RemoveEffect(0, pTarget, iEffect);
	}
}

global func RemoveAllBehaviours(object pTarget, string name)
{
	var behaviour = Format("IntBehaviour%s",name);
	RemoveAllEffects(pTarget, behaviour);
}

/*
 * operator == 0: only the selected priority
 * operator == -1: less than the selected priority
 * operator == +1: greater than the selected priority
 * operator == -10: less than or equal
 * operator == +10: greater than or equal
 */
global func GetBehaviours(object pTarget, int iPriority, int operator)
{
	var effects = [];
	var iEffect;
	  // Von Effektzahl abwärts zählen, da Effekte entfernt werden

	var i = GetEffectCount(0, pTarget);
	while (i--)
	if (iEffect = GetEffect("IntBehaviour*", pTarget, i))
	{
		if (   (EffectVar(gBehaviour_EffectVar_Priority, pTarget, iEffect) == iPriority
		        || iPriority == gBehaviour_Priority_All)
		    && (operator%10 == 0))
		{
			PushBack(iEffect, effects);
		}

		if (operator < 0 && EffectVar(gBehaviour_EffectVar_Priority, pTarget, iEffect) < iPriority)
		{
			PushBack(iEffect, effects);
		}

		if (operator > 0 && EffectVar(gBehaviour_EffectVar_Priority, pTarget, iEffect) > iPriority)
		{
			PushBack(iEffect, effects);
		}
	}
	return effects;
}





/*******************************************************************************************************************
 *
 * Behaviour definitions
 *
 *******************************************************************************************************************/




global func BehaviourWander(int xLeft, int xRight, int minPause, int maxPause)
{
	var data = [];
	data[gBehaviour_Wander_Index_xLeft] = xLeft;
	data[gBehaviour_Wander_Index_xRight] = xRight;
	data[gBehaviour_Wander_Index_minPause] = minPause;
	data[gBehaviour_Wander_Index_maxPause] = maxPause;
}






/*******************************************************************************************************************
 *
 * The actual scripted behaviours
 *
 *******************************************************************************************************************/

global func FxIntBehaviourWanderStart(object pTarget, int iEffectNumber, int iTemp, data, int priority)
{
	if (iTemp) return;

	EffectVar(gBehaviour_EffectVar_Data, pTarget, iEffectNumber) = data;
	EffectVar(gBehaviour_EffectVar_Priority, pTarget, iEffectNumber) = priority;
}

global func FxIntBehaviourWanderTimer(object obj, int nr, int time)
{
	if (!obj) return -1;
	if (GetCommand(obj)) return;

	var behaviours = GetBehaviours(obj, EffectVar(gBehaviour_EffectVar_Priority, obj, nr), +1);
	if (GetLength(behaviours) > 0) return;

	var data = EffectVar(gBehaviour_EffectVar_Data, obj, nr);
	var xLeft = data[gBehaviour_Wander_Index_xLeft];
	var xRight = data[gBehaviour_Wander_Index_xRight];
	var minPause = data[gBehaviour_Wander_Index_minPause];
	var maxPause = data[gBehaviour_Wander_Index_maxPause];

	AddCommand(obj, "MoveTo", 0, RandomX(xLeft, xRight), GetY(obj), 0, 500, 0, 1, C4CMD_SilentBase);
	AppendCommand(obj, "Wait", 0, 0, 0, 0, 0, RandomX(1 + minPause, maxPause), 1, C4CMD_SilentBase);
}

global func FxIntBehaviourStopStart(object pTarget, int iEffectNumber, int iTemp, data, int priority)
{
	if (iTemp) return;

	EffectVar(gBehaviour_EffectVar_Priority, pTarget, iEffectNumber) = priority;
}

global func FxIntBehaviourStopTimer(object obj, int nr, int time)
{
	if (!obj) return -1;

	var behaviours = GetBehaviours(obj, EffectVar(gBehaviour_EffectVar_Priority, obj, nr), +1);
	if (GetLength(behaviours) > 0) return;

	SetCommand(obj, "None");
	SetComDir(COMD_Stop, obj);
}
