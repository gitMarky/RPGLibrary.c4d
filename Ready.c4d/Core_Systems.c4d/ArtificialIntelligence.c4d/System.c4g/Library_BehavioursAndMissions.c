/*--
Controls the behaviour of AI objects. There are 3 levels of behavior, sorted by priority in ascending order:@br
- Behaviour: A simple behaviour without any goals, usually used when the object is idle.@br
- Mission: A complex behaviour that consists of a collection of tasks, i.e. "bake cookies". Can be exchanged between AI objects.@br
- Task: A simple behaviour with a single goal, i.e. "mix dough". Can be exchanged between AI objects.
@title Behaviours and Missions
@author Marky
@version 0.1.0
--*/

#strict 2

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

static const gBehaviour_Wander_Area = "WanderArea";
static const gBehaviour_Wander_Point = "WanderPoint";

static const gBehaviour_Priority_All = -1;
static const gBehaviour_Priority_Idle = 10;
static const gBehaviour_Priority_Mission = 20;
static const gBehaviour_Priority_Task = 30;
static const gBehaviour_Priority_Override = 50;

static const gBehaviour_Operator_eq   = 0;   //: only the selected priority
static const gBehaviour_Operator_lt   = -1;  //: less than the selected priority
static const gBehaviour_Operator_gt   = +1;  //: greater than the selected priority
static const gBehaviour_Operator_lteq = -10; //: less than or equal
static const gBehaviour_Operator_gteq = +10 ;//: greater than or equal


/*******************************************************************************************************************
 *
 * Manipulate Behaviours
 *
 *******************************************************************************************************************/

/**
 Adds a behaviour to an AI object.
 @par target The object that gets the new behaviour.
 @par name The name of the behaviour. See TODO.
 @par interval The interval in frames between function calls of the behaviour.
 @par data An array with additional data for the behavior specification. Functions 
 such as BehaviourWanderArea() create such an array based on the input.
 @par priority The priority of the behaviour. This has to be a unique value. Setting the
 priority twice will result in the first behaviour being forgotten.
 @return int The method creates an effect with the {@c name} in the {@c target}. Return value is
 that of AddEffect()
 @link RemoveAllBehaviours , RemoveBehaviour , BehaviourStop , BehaviourWanderArea, BehaviourWanderPoint
 */
global func AddBehaviour(object target, string name, int interval, array data, int priority)
{
	if (!target) if(!(target = this)) { ErrorLog("Cannot add behaviour %s because no target was specified: %v", name, target); return;}

	var behaviour = Format("IntBehaviour%s",name);
	//RemoveAllEffects(pTarget, behaviour);
	RemoveBehaviour(target, name, priority); // replace current behaviour in this priority

	return AddEffect (behaviour, target, 100, interval, 0, 0, data, priority);
}

/**
Removes all behaviours from the object with a certain name and priority.
 @par target The behaviour will be removed from this object.
 @par name The name of the behaviour.
 @par priority The priority of the behaviour.
 @par operator See GetBehaviours(). No value or 0 means that the priorities have to be equal.
 @example The operator can be used to remove behaviours of certain priority:
 {@code
    RemoveBehaviour(this, gBehaviour_Wander_Area, 30); // removes the behaviour if it has priority 30
    RemoveBehaviour(this, gBehaviour_Wander_Area, 30, gBehaviour_Operator_lt); // removes the behavior if it has priority 0 to 29
    RemoveBehaviour(this, gBehaviour_Wander_Area, 30, gBehaviour_Operator_gteq); // removes the behaviour if it has priority 30 or more 
 }
 @link RemoveAllBehaviours , BehaviourStop , BehaviourWanderArea, BehaviourWanderPoint
*/
global func RemoveBehaviour(object target, string name, int priority, int operator)
{
	if (!target) if(!(target = this)) { ErrorLog("Cannot remove behaviour %s because no target was specified: %v", name, target); return;}

	var behaviours = GetBehaviours(target, priority, operator);
	//RemoveAllEffects(pTarget, behaviour);

	DebugLog("--> Remove Behaviour %s %v - array: %v", name, target, behaviours);

	var effect_number;
	var i = GetEffectCount(0, target);
	while (i--)
	if (effect_number = GetEffect(Format("IntBehaviour%s", name), target, i))
	{
		DebugLog("--> Remove Behaviour - Check effect %d", effect_number);
		if (GetArrayItemPosition(effect_number, behaviours) > -1)
		      RemoveEffect(0, target, effect_number);
	}
}

/**
 Removes all behaviours of a target.
 @par target The behaviours will be removed from this object.
 @par name The name of the behaviour.
 @link RemoveBehaviour , BehaviourStop , BehaviourWanderArea, BehaviourWanderPoint
 */
global func RemoveAllBehaviours(object target, string name)
{
	var behaviour = Format("IntBehaviour%s",name);
	RemoveAllEffects(target, behaviour);
}

/**
 Gets all behaviours of specific priority.
 @par target The object that has the behaviours.
 @par priority The priority to compare to
 @par operator No value or 0 means that the priorities have to be equal.
 The following operators are available:@br
 {@table
	 {@tr {@th Constant} {@td Value} {@td Description}}
	 {@tr {@td gBehaviour_Operator_eq}   {@td 0}   {@td Priority of behaviour == specified priority}}
	 {@tr {@td gBehaviour_Operator_lt}   {@td -1}  {@td Priority of behaviour < specified priority}}
	 {@tr {@td gBehaviour_Operator_gt}   {@td +1}  {@td Priority of behaviour > specified priority}}
	 {@tr {@td gBehaviour_Operator_lteq} {@td -10} {@td Priority of behaviour <= specified priority}}
	 {@tr {@td gBehaviour_Operator_gteq} {@td +10} {@td Priority of behaviour >= specified priority}}
 }
 */
global func GetBehaviours(object target, int priority, int operator)
{
	var effects = [];
	var effect_number;

	// count down, because the effects are being removed
	var i = GetEffectCount(0, target);
	while (i--)
	if (effect_number = GetEffect("IntBehaviour*", target, i))
	{
		if (   (EffectVar(gBehaviour_EffectVar_Priority, target, effect_number) == priority
		        || priority == gBehaviour_Priority_All)
		    && (operator%10 == 0))
		{
			PushBack(effect_number, effects);
		}

		if (operator < 0 && EffectVar(gBehaviour_EffectVar_Priority, target, effect_number) < priority)
		{
			PushBack(effect_number, effects);
		}

		if (operator > 0 && EffectVar(gBehaviour_EffectVar_Priority, target, effect_number) > priority)
		{
			PushBack(effect_number, effects);
		}
	}
	return effects;
}





/*******************************************************************************************************************
 *
 * Behaviour definitions
 *
 *******************************************************************************************************************/

/**
 Data array for {@c gBehaviour_Wander_Area}. The object moves between a left and right boundary:
 @par x_left The left boundary of the area, in absolute coordinates.
 @par x_right The right boundary of the area, in absolute coordinates.
 @par min_pause The object waits a random amount of time after walking a 
      short distance. It waits at least this many frames.
 @par max_pause The object waits a random amount of time after walking a 
      short distance. It waits at most this many frames.
 @par speed_percent The object walks at this many percent of its maximum speed. 100 is full speed.
 */
global func BehaviourWanderArea(int x_left, int x_right, int min_pause, int max_pause, int speed_percent)
{
	var data = [];
	data[gBehaviour_Wander_Index_xLeft] = x_left;
	data[gBehaviour_Wander_Index_xRight] = x_right;
	data[gBehaviour_Wander_Index_minPause] = min_pause;
	data[gBehaviour_Wander_Index_maxPause] = max_pause;
	data[gBehaviour_Wander_Index_Speed] = speed_percent;
}

/**
 Data array for {@c gBehaviour_Wander_Point}. The object moves in a circle area around a point:
 @par x The x coordinate of the circle center, in absolute coordinates.
 @par y The y coordinate of the circle center, in absolute coordinates.
 @par radius The radius of the circle.
 @par min_pause The object waits a random amount of time after walking a 
      short distance. It waits at least this many frames.
 @par max_pause The object waits a random amount of time after walking a 
      short distance. It waits at most this many frames.
 @par speed_percent The object walks at this many percent of its maximum speed. 100 is full speed.
 */]
global func BehaviourWanderPoint(int x, int y, int radius, int min_pause, int max_pause, int speed_percent)
{
	var data = [];
	data[gBehaviour_Wander_Index_Point] = [x, y];
	data[gBehaviour_Wander_Index_Radius] = radius;
	data[gBehaviour_Wander_Index_minPause] = min_pause;
	data[gBehaviour_Wander_Index_maxPause] = max_pause;
	data[gBehaviour_Wander_Index_Speed] = speed_percent;
}

/*******************************************************************************************************************
 *
 * The actual scripted behaviours
 *
 *******************************************************************************************************************/


/*
 * Wander in an area, left to right.
 */

global func FxIntBehaviourWanderAreaStart(object target, int effect_number, int temp, data, int priority)
{
	if (temp) return;

	EffectVar(gBehaviour_EffectVar_Data, target, effect_number) = data;
	EffectVar(gBehaviour_EffectVar_Priority, target, effect_number) = priority;
}

global func FxIntBehaviourWanderAreaTimer(object target, int effect_number, int time)
{
	if (!target) return -1;
	if (GetCommand(target)) return;

	var behaviours = GetBehaviours(target, EffectVar(gBehaviour_EffectVar_Priority, target, effect_number), +1);
	if (GetLength(behaviours) > 0)
	{
		FxIntBehaviourWanderSpeed(target, effect_number, -1);
		return;
	}


	var data = EffectVar(gBehaviour_EffectVar_Data, target, effect_number);
	var x_left  = Max(data[gBehaviour_Wander_Index_xLeft],  target->GetX() -50);
	var x_right = Min(data[gBehaviour_Wander_Index_xRight], target->GetX() +50);
	var min_pause = data[gBehaviour_Wander_Index_minPause];
	var max_pause = data[gBehaviour_Wander_Index_maxPause];
	var speed = data[gBehaviour_Wander_Index_Speed];

	FxIntBehaviourWanderSpeed(target, effect_number, speed);

	AddCommand(target, "MoveTo", 0, RandomX(x_left, x_right), GetY(target), 0, 500, 0, 1, C4CMD_SilentBase);
	AppendCommand(target, "Wait", 0, 0, 0, 0, 0, RandomX(1 + min_pause, max_pause), 1, C4CMD_SilentBase);
}

global func FxIntBehaviourWanderAreaStop(object target, int effect_number, int reason, bool temp)
{
	if (temp) return;

	FxIntBehaviourWanderSpeed(target, effect_number, -1);
}


/*
 * Wander around a spot, return if you are too far away.
 */

global func FxIntBehaviourWanderPointStart(object target, int effect_number, int temp, data, int priority)
{
	if (temp) return;

	EffectVar(gBehaviour_EffectVar_Data, target, effect_number) = data;
	EffectVar(gBehaviour_EffectVar_Priority, target, effect_number) = priority;
}

global func FxIntBehaviourWanderPointTimer(object target, int effect_number, int time)
{
	if (!target) return -1;
	if (GetCommand(target)) return;

	var behaviours = GetBehaviours(target, EffectVar(gBehaviour_EffectVar_Priority, target, effect_number), +1);
	if (GetLength(behaviours) > 0)
	{
		FxIntBehaviourWanderSpeed(target, effect_number, -1);
		return;
	}


	var data = EffectVar(gBehaviour_EffectVar_Data, target, effect_number);
	var point  = data[gBehaviour_Wander_Index_Point];
	var radius = data[gBehaviour_Wander_Index_Radius];
	var speed = data[gBehaviour_Wander_Index_Speed];

	FxIntBehaviourWanderSpeed(target, effect_number, speed);

	if (Distance(point[0],point[1],target->GetX(), target->GetY()) > radius)
	{
		AddCommand(target, "MoveTo", 0, point[0], point[1], 0, 500, 0, 1, C4CMD_SilentBase);
	}
	else
	{
		AddCommand(target, "MoveTo", 0, RandomX(GetX(target)-radius/3, GetX(target)+radius/3), GetY(target), 0, 500, 0, 1, C4CMD_SilentBase);
		//AppendCommand(obj, "Wait", 0, 0, 0, 0, 0, RandomX(1 + minPause, maxPause), 1, C4CMD_SilentBase);
	}
}

global func FxIntBehaviourWanderPointStop(object target, int effect_number, int reason, bool temp)
{
	if (temp) return;

	FxIntBehaviourWanderSpeed(target, effect_number, -1);
}


global func FxIntBehaviourWanderSpeed(object target, int effect_number, int speed)
{
	//var speed = iSpeed;
	if (speed <= 0) speed = 100;

	if (speed == 100)
    {
		if (EffectVar(gBehaviour_EffectVar_Data,target,effect_number)[gBehaviour_Wander_Index_PhysMod] == true)
		{
			ResetPhysical(target,"Walk");
			ResetPhysical(target,"Jump");

			EffectVar(gBehaviour_EffectVar_Data,target,effect_number)[gBehaviour_Wander_Index_PhysMod] = false;
		}
    }
	else
	{
		if (!EffectVar(gBehaviour_EffectVar_Data,target,effect_number)[gBehaviour_Wander_Index_PhysMod])
		{
			EffectVar(gBehaviour_EffectVar_Data,target,effect_number)[gBehaviour_Wander_Index_PhysMod] = true;

			var physicalWalk = speed * GetPhysical("Walk",PHYS_Current,target) / 100;
			var physicalJump = speed * GetPhysical("Jump",PHYS_Current,target) / 100;

			DebugLog("Adjusted physicals: %d %d", physicalWalk, physicalJump);
			SetPhysical("Walk", physicalJump, PHYS_StackTemporary, target);
			SetPhysical("Jump", physicalJump, PHYS_StackTemporary, target);
		}
	}
}

/*
 * Stop what you are doing and stand still
 */

global func FxIntBehaviourStopStart(object target, int effect_number, int temp, data, int priority)
{
	if (temp) return;

	EffectVar(gBehaviour_EffectVar_Priority, target, effect_number) = priority;
}

global func FxIntBehaviourStopTimer(object target, int effect_number, int time)
{
	if (!target) return -1;

	var behaviours = GetBehaviours(target, EffectVar(gBehaviour_EffectVar_Priority, target, effect_number), +1);
	if (GetLength(behaviours) > 0) return;

	SetCommand(target, "None");
	SetComDir(COMD_Stop, target);
}
