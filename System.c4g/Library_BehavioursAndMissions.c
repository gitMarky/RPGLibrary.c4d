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
static const gBehaviour_Wander_Index_Speed = 4;
static const gBehaviour_Wander_Index_PhysMod = 5;
static const gBehaviour_Wander_Index_Point = 0;
static const gBehaviour_Wander_Index_Radius = 1;

static const gBehaviour_WanderArea = "WanderArea";
static const gBehaviour_WanderPoint = "WanderPoint";

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

	DebugLog("--> Remove Behaviour %s %v - array: %v", name, pTarget, behaviours);

	var iEffect;
	var i = GetEffectCount(0, pTarget);
	while (i--)
	if (iEffect = GetEffect(Format("IntBehaviour%s",name), pTarget, i))
	{
		DebugLog("--> Remove Behaviour - Check effect %d", iEffect);
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

global func BehaviourWanderArea(int xLeft, int xRight, int minPause, int maxPause, int iSpeedPercent)
{
	var data = [];
	data[gBehaviour_Wander_Index_xLeft] = xLeft;
	data[gBehaviour_Wander_Index_xRight] = xRight;
	data[gBehaviour_Wander_Index_minPause] = minPause;
	data[gBehaviour_Wander_Index_maxPause] = maxPause;
	data[gBehaviour_Wander_Index_Speed] = iSpeedPercent;
}

global func BehaviourWanderPoint(int x, int y, int iRadius, int minPause, int maxPause, int iSpeedPercent)
{
	var data = [];
	data[gBehaviour_Wander_Index_Point] = [x, y];
	data[gBehaviour_Wander_Index_Radius] = iRadius;
	data[gBehaviour_Wander_Index_minPause] = minPause;
	data[gBehaviour_Wander_Index_maxPause] = maxPause;
	data[gBehaviour_Wander_Index_Speed] = iSpeedPercent;
}

/*******************************************************************************************************************
 *
 * The actual scripted behaviours
 *
 *******************************************************************************************************************/


/*
 * Wander in an area, left to right.
 */

global func FxIntBehaviourWanderAreaStart(object pTarget, int iEffectNumber, int iTemp, data, int priority)
{
	if (iTemp) return;

	EffectVar(gBehaviour_EffectVar_Data, pTarget, iEffectNumber) = data;
	EffectVar(gBehaviour_EffectVar_Priority, pTarget, iEffectNumber) = priority;
}

global func FxIntBehaviourWanderAreaTimer(object obj, int nr, int time)
{
	if (!obj) return -1;
	if (GetCommand(obj)) return;

	var behaviours = GetBehaviours(obj, EffectVar(gBehaviour_EffectVar_Priority, obj, nr), +1);
	if (GetLength(behaviours) > 0)
	{
		FxIntBehaviourWanderSpeed(obj, nr, -1);
		return;
	}


	var data = EffectVar(gBehaviour_EffectVar_Data, obj, nr);
	var xLeft  = Max(data[gBehaviour_Wander_Index_xLeft],  obj->GetX() -50);
	var xRight = Min(data[gBehaviour_Wander_Index_xRight], obj->GetX() +50);
	var minPause = data[gBehaviour_Wander_Index_minPause];
	var maxPause = data[gBehaviour_Wander_Index_maxPause];
	var speed = data[gBehaviour_Wander_Index_Speed];

	FxIntBehaviourWanderSpeed(obj, nr, speed);

	AddCommand(obj, "MoveTo", 0, RandomX(xLeft, xRight), GetY(obj), 0, 500, 0, 1, C4CMD_SilentBase);
	AppendCommand(obj, "Wait", 0, 0, 0, 0, 0, RandomX(1 + minPause, maxPause), 1, C4CMD_SilentBase);
}

global func FxIntBehaviourWanderAreaStop(object pTarget, int iEffect, int iReason, bool fTemp)
{
	if (fTemp) return;

	FxIntBehaviourWanderSpeed(pTarget, iEffect, -1);
}


/*
 * Wander around a spot, return if you are too far away.
 */

global func FxIntBehaviourWanderPointStart(object pTarget, int iEffectNumber, int iTemp, data, int priority)
{
	if (iTemp) return;

	EffectVar(gBehaviour_EffectVar_Data, pTarget, iEffectNumber) = data;
	EffectVar(gBehaviour_EffectVar_Priority, pTarget, iEffectNumber) = priority;
}

global func FxIntBehaviourWanderPointTimer(object obj, int nr, int time)
{
	if (!obj) return -1;
	if (GetCommand(obj)) return;

	var behaviours = GetBehaviours(obj, EffectVar(gBehaviour_EffectVar_Priority, obj, nr), +1);
	if (GetLength(behaviours) > 0)
	{
		FxIntBehaviourWanderSpeed(obj, nr, -1);
		return;
	}


	var data = EffectVar(gBehaviour_EffectVar_Data, obj, nr);
	var point  = data[gBehaviour_Wander_Index_Point];
	var radius = data[gBehaviour_Wander_Index_Radius];
	var speed = data[gBehaviour_Wander_Index_Speed];

	FxIntBehaviourWanderSpeed(obj, nr, speed);

	if (Distance(point[0],point[1],obj->GetX(), obj->GetY()) > radius)
	{
		AddCommand(obj, "MoveTo", 0, point[0], point[1], 0, 500, 0, 1, C4CMD_SilentBase);
	}
	else
	{
		AddCommand(obj, "MoveTo", 0, RandomX(GetX(obj)-radius/3, GetX(obj)+radius/3), GetY(obj), 0, 500, 0, 1, C4CMD_SilentBase);
		//AppendCommand(obj, "Wait", 0, 0, 0, 0, 0, RandomX(1 + minPause, maxPause), 1, C4CMD_SilentBase);
	}
}

global func FxIntBehaviourWanderPointStop(object pTarget, int iEffect, int iReason, bool fTemp)
{
	if (fTemp) return;

	FxIntBehaviourWanderSpeed(pTarget, iEffect, -1);
}


global func FxIntBehaviourWanderSpeed(object pTarget, int iEffect, int iSpeed)
{
	var speed = iSpeed;
	if (speed <= 0) speed = 100;

	if (speed == 100)
    {
		if (EffectVar(gBehaviour_EffectVar_Data,pTarget,iEffect)[gBehaviour_Wander_Index_PhysMod] == true)
		{
			ResetPhysical(pTarget,"Walk");
			ResetPhysical(pTarget,"Jump");

			EffectVar(gBehaviour_EffectVar_Data,pTarget,iEffect)[gBehaviour_Wander_Index_PhysMod] = false;
		}
    }
	else
	{
		if (!EffectVar(gBehaviour_EffectVar_Data,pTarget,iEffect)[gBehaviour_Wander_Index_PhysMod])
		{
			EffectVar(gBehaviour_EffectVar_Data,pTarget,iEffect)[gBehaviour_Wander_Index_PhysMod] = true;

			var physicalWalk = speed * GetPhysical("Walk",PHYS_Current,pTarget) / 100;
			var physicalJump = speed * GetPhysical("Jump",PHYS_Current,pTarget) / 100;

			DebugLog("Adjusted physicals: %d %d", physicalWalk, physicalJump);
			SetPhysical("Walk", physicalJump, PHYS_StackTemporary, pTarget);
			SetPhysical("Jump", physicalJump, PHYS_StackTemporary, pTarget);
		}
	}
}

/*
 * Stop what you are doing and stand still
 */

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
