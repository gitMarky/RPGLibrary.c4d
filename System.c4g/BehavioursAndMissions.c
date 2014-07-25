/*-- Behaviours and Missions --*/

#strict 2

/*
Steuert das Verhalten von KI-Objekten. Es gibt 3 Stufen von KI-Handhabung, sortiert nach aufsteigender Priorität:
- Behaviour: Eine einfache Verhaltensweise ohne besondere Ziele.
- Mission: Eine Komplexe Aufgabe. Besteht aus einzelnen Tasks. Kann an andere KI-Objekte abgegeben werden.
- Task: Eine einfache Aufgabe. Kann an andere KI-Objekte abgegeben werden.
 */

static const gBehaviour_EffectVar_Data = 0;

static const gBehaviour_Wander_Index_xLeft = 0;
static const gBehaviour_Wander_Index_xRight = 1;
static const gBehaviour_Wander_Index_minPause = 2;
static const gBehaviour_Wander_Index_maxPause = 3;

static const gBehaviour_Wander = "Wander";

global func AddBehaviour(object pTarget, string name, int interval, array data)
{
	var behaviour = Format("IntBehaviour%s",name);
	RemoveAllEffects(pTarget, behaviour);

	return AddEffect (behaviour, pTarget, 100, interval, 0, 0, data);
}

global func RemoveBehaviour(object pTarget, string name)
{
	var behaviour = Format("IntBehaviour%s",name);
	RemoveAllEffects(pTarget, behaviour);
}









global func BehaviourWander(int xLeft, int xRight, int minPause, int maxPause)
{
	var data = [];
	data[gBehaviour_Wander_Index_xLeft] = xLeft;
	data[gBehaviour_Wander_Index_xRight] = xRight;
	data[gBehaviour_Wander_Index_minPause] = minPause;
	data[gBehaviour_Wander_Index_maxPause] = maxPause;
}

global func FxIntBehaviourWanderStart(object pTarget, int iEffectNumber, int iTemp, data)
{
	if (iTemp) return;

	EffectVar(gBehaviour_EffectVar_Data, pTarget, iEffectNumber) = data;
}

global func FxIntBehaviourWanderTimer(object obj, int nr, int time)
{
	if (!obj) return -1;
	if (GetCommand(obj)) return;

	var data = EffectVar(gBehaviour_EffectVar_Data, obj, nr);
	var xLeft = data[gBehaviour_Wander_Index_xLeft];
	var xRight = data[gBehaviour_Wander_Index_xRight];
	var minPause = data[gBehaviour_Wander_Index_minPause];
	var maxPause = data[gBehaviour_Wander_Index_maxPause];

	AddCommand(obj, "MoveTo", 0, RandomX(xLeft, xRight), GetY(obj), 0, 500, 0, 1, C4CMD_SilentBase);
	AppendCommand(obj, "Wait", 0, 0, 0, 0, 0, RandomX(1 + minPause, maxPause), 1, C4CMD_SilentBase);
}
