/*--
 Helper object that can be used to build behaviour descriptions for say and night cycles. The advantage in using this object is,
 that you do not have to remember the order of entries in the behaviour array. It saves a lot
 of time.
 @title Behaviour builder
 @author Marky
 @version 0.2.0
--*/

#strict 2

local definition;

//AddBehaviour(object target, string name, int interval, array data, int priority)

static const gBehaviour_FxDef_Target =   0;
static const gBehaviour_FxDef_Name = 	 1;
static const gBehaviour_FxDef_Interval = 2;
static const gBehaviour_FxDef_Data = 	 3;
static const gBehaviour_FxDef_Priority = 4;

public func Initialize()
{
	SetPosition();
	definition = [];
	
	// default values
	definition[gBehaviour_FxDef_Target]   = "pTarget";
	definition[gBehaviour_FxDef_Name]     = 0;
	definition[gBehaviour_FxDef_Interval] = 30;
	definition[gBehaviour_FxDef_Data]     = 0;
	definition[gBehaviour_FxDef_Priority] = gBehaviour_Priority_Idle;
}

public func &Definition()
{
	return LocalN("definition");
}

//"AddBehaviour(pTarget, gBehaviour_Wander_Area, 20, [284,336,1,15,75], gBehaviour_Priority_Idle)""

public func Priority(int priority)
{
	definition[gBehaviour_FxDef_Priority] = priority;
	return this;
}

public func Interval(int interval)
{
	definition[gBehaviour_FxDef_Interval] = interval;
	return this;
}

public func Add()
{
	// "AddBehaviour(pTarget, gBehaviour_Wander_Area, 20, [284,336,1,15,75], gBehaviour_Priority_Idle)"
	return Format("AddBehaviour(%s, %v, %d, %v, %d)", 
					definition[gBehaviour_FxDef_Target],
					definition[gBehaviour_FxDef_Name],
					definition[gBehaviour_FxDef_Interval],
					definition[gBehaviour_FxDef_Data],
					definition[gBehaviour_FxDef_Priority]);
}

public func Remove()
{
	//"RemoveBehaviour(pTarget, gBehaviour_Wander_Area, gBehaviour_Priority_Idle)"
	return Format("RemoveBehaviour(%s, %v, %d)", 
					definition[gBehaviour_FxDef_Target],
					definition[gBehaviour_FxDef_Name],
					definition[gBehaviour_FxDef_Priority]);
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
global func BehaviourWanderArea(int x_left, int x_right, int min_pause, int max_pause, int speed_percent, int interval)
{
	var data = [];
	data[gBehaviour_Wander_Index_xLeft] = x_left;
	data[gBehaviour_Wander_Index_xRight] = x_right;
	data[gBehaviour_Wander_Index_minPause] = min_pause;
	data[gBehaviour_Wander_Index_maxPause] = max_pause;
	data[gBehaviour_Wander_Index_Speed] = speed_percent;
	
	if (interval == 0) interval = 20;
	
	var builder = CreateObject(ID_Helper_BehaviourBuilder, 0, 0, NO_OWNER);
	
	builder->Definition()[gBehaviour_FxDef_Name] = gBehaviour_Wander_Area;
	builder->Definition()[gBehaviour_FxDef_Data] = data;
	builder->Definition()[gBehaviour_FxDef_Interval] = interval;
	return builder;
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
 */
global func BehaviourWanderPoint(int x, int y, int radius, int min_pause, int max_pause, int speed_percent, int interval)
{
	var data = [];
	data[gBehaviour_Wander_Index_Point] = [x, y];
	data[gBehaviour_Wander_Index_Radius] = radius;
	data[gBehaviour_Wander_Index_minPause] = min_pause;
	data[gBehaviour_Wander_Index_maxPause] = max_pause;
	data[gBehaviour_Wander_Index_Speed] = speed_percent;

	if (interval == 0) interval = 20;
	
	var builder = CreateObject(ID_Helper_BehaviourBuilder, 0, 0, NO_OWNER);
	
	builder->Definition()[gBehaviour_FxDef_Name] = gBehaviour_Wander_Point;
	builder->Definition()[gBehaviour_FxDef_Data] = data;
	builder->Definition()[gBehaviour_FxDef_Interval] = interval;
	return builder;
}